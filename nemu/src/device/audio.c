#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>


#define min(x,y) ( (x < y) ? x : y )

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static uint32_t sbuf_pos = 0;

SDL_AudioSpec s = {};

static bool is_init = false;
void sdl_audio_callback(void *udata, uint8_t *stream, int len){
  memset(stream, 0, len);
  uint32_t used_cnt = audio_base[reg_count];
  if(len > used_cnt) len = used_cnt;
  
  uint32_t sbuf_size = audio_base[reg_sbuf_size]/sizeof(uint8_t);
  if( (sbuf_pos + len) > sbuf_size ){
    SDL_MixAudio(stream, sbuf + sbuf_pos, sbuf_size - sbuf_pos , SDL_MIX_MAXVOLUME);
    SDL_MixAudio(stream +  (sbuf_size - sbuf_pos), sbuf +  (sbuf_size - sbuf_pos), len - (sbuf_size - sbuf_pos) , SDL_MIX_MAXVOLUME);
  }
  else 
    SDL_MixAudio(stream, sbuf + sbuf_pos, len , SDL_MIX_MAXVOLUME);
  sbuf_pos = (sbuf_pos + len) % sbuf_size;
  audio_base[reg_count] -= len;
  
}

void init_sound() {
  s.format = AUDIO_S16SYS;  // 假设系统中音频数据的格式总是使用16位有符号数来表示
  s.userdata = NULL;        // 不使用
  s.freq = audio_base[reg_freq];
  s.channels = audio_base[reg_channels];
  s.samples = audio_base[reg_samples];
  s.callback = sdl_audio_callback;
  int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);
  if(ret==0){
    SDL_OpenAudio(&s, NULL);
    SDL_PauseAudio(0);
  }
}


static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  if(audio_base[reg_init]==1){
    init_sound();
    is_init = true;
    audio_base[reg_init] = 0;
  }
}


void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
  
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
  audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
}
