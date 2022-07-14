/*
 * this is the first process in the OS:
 *   display a splash screen
 *   display a boot menu and receive input
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_bmp.h>
#include <SDL_bdf.h>

const char *font_fname = "/share/fonts/Courier-7.bdf";
static BDF_Font *font;
static SDL_Surface *screen = NULL;
static SDL_Surface *logo_sf = NULL;

static void display_menu(int n);

struct MenuItem
{
  const char *name, *bin, *arg1;
} items[] = {
    {"NJU Terminal", "/bin/nterm", NULL},
    {"NSlider", "/bin/nslider", NULL},
    {"FCEUX (Super Mario Bros)", "/bin/fceux", "/share/games/nes/mario.nes"},
    {"FCEUX (100 in 1)", "/bin/fceux", "/share/games/nes/100in1.nes"},
    {"Flappy Bird", "/bin/bird", NULL},
    {"PAL - Xian Jian Qi Xia Zhuan", "/bin/pal", NULL},
    {"NPlayer", "/bin/nplayer", NULL},
    {"coremark", "/bin/coremark", NULL},
    {"dhrystone", "/bin/dhrystone", NULL},
    {"typing-game", "/bin/typing-game", NULL},
    {"ONScripter", "/bin/onscripter", NULL},
};

#define nitems (sizeof(items) / sizeof(items[0]))
#define MAX_PAGE ((nitems - 1) / 10)
#define MAX_IDX_LAST_PAGE ((nitems - 1) % 10)

static int page = 0;
static int i_max = 0;

static void set_i_max()
{
  i_max = (page == MAX_PAGE ? MAX_IDX_LAST_PAGE : 9);
  printf("page = %d, MAX_PAGE = %d, MAX_IDX_LAST_PAGE = %d\n", page, MAX_PAGE, MAX_IDX_LAST_PAGE);
}
static void next()
{
  if (page < MAX_PAGE)
  {
    page++;
    set_i_max();
  }
}

static void prev()
{
  if (page > 0)
  {
    page--;
    set_i_max();
  }
}

static void clear_display(void)
{
  SDL_FillRect(screen, NULL, 0xffffff);
}

static void free_resource(void)
{
  delete font;
  SDL_FreeSurface(logo_sf);
  SDL_FreeSurface(screen);
}

int main(int argc, char *argv[], char *envp[])
{
  SDL_Init(0);
  screen = SDL_SetVideoMode(0, 0, 32, SDL_HWSURFACE);

  font = new BDF_Font(font_fname);
  logo_sf = SDL_LoadBMP("/share/pictures/projectn.bmp");
  assert(logo_sf);
  set_i_max();

  while (1)
  {

    display_menu(i_max);

    SDL_Event e;
    do
    {
      SDL_WaitEvent(&e);
    } while (e.type == SDL_KEYDOWN);

    int i = -1;
    switch (e.key.keysym.sym)
    {
    case SDLK_0:
      i = 0;
      break;
    case SDLK_1:
      i = 1;
      break;
    case SDLK_2:
      i = 2;
      break;
    case SDLK_3:
      i = 3;
      break;
    case SDLK_4:
      i = 4;
      break;
    case SDLK_5:
      i = 5;
      break;
    case SDLK_6:
      i = 6;
      break;
    case SDLK_7:
      i = 7;
      break;
    case SDLK_8:
      i = 8;
      break;
    case SDLK_9:
      i = 9;
      break;
    case SDLK_LEFT:
      prev();
      break;
    case SDLK_RIGHT:
      next();
      break;
    case SDLK_Q:
      clear_display();
      SDL_UpdateRect(screen, 0, 0, 0, 0);
      SDL_Quit();
      exit(0);
      break;
    }

    if (i != -1 && i <= i_max)
    {
      i += page * 10;
      auto *item = &items[i];
      const char *exec_argv[3];
      exec_argv[0] = item->bin;
      exec_argv[1] = item->arg1;
      exec_argv[2] = NULL;
      clear_display();
      SDL_UpdateRect(screen, 0, 0, 0, 0);
      execve(exec_argv[0], (char **)exec_argv, (char **)envp);
      fprintf(stderr, "\033[31m[ERROR]\033[0m Exec %s failed.\n\n", exec_argv[0]);
    }
    else
    {
      fprintf(stderr, "Choose a number between %d and %d\n\n", 0, i_max);
    }
  }
  return -1;
}

static void draw_ch(BDF_Font *font, int x, int y, char ch, uint32_t fg, uint32_t bg)
{
  SDL_Surface *s = BDF_CreateSurface(font, ch, fg, bg);
  SDL_Rect dstrect = {.x = (int16_t)x, .y = (int16_t)y};
  SDL_BlitSurface(s, NULL, screen, &dstrect);
  SDL_FreeSurface(s);
}

static void draw_str(BDF_Font *font, int x, int y, char *str, uint32_t fp, uint32_t bg)
{
  while (*str)
  {
    draw_ch(font, x, y, *str, fp, bg);
    x += font->w;
    str++;
  }
}

static void draw_text_row(char *s, int r)
{
  r += 3;
  puts(s);
  // printf("draw h is %d\n",r*font->h);
  draw_str(font, 0, r * font->h, s, 0x123456, 0xffffff);
}

static void display_menu(int n)
{
  clear_display();
  SDL_Rect rect = {.x = (int16_t)(screen->w - logo_sf->w), .y = 0};
  // printf("log size is %d,%d\n", screen->w, logo_sf->w);

  SDL_BlitSurface(logo_sf, NULL, screen, &rect);
  // SDL_UpdateRect(screen, 0, 0, 0, 0);

  printf("Available applications:\n");
  char buf[80];
  int i;
  for (i = 0; i <= n; i++)
  {
    auto *item = &items[page * 10 + i];
    sprintf(buf, "  [%d] %s", i, item->name);
    printf("%s", buf);
    draw_text_row(buf, i);
  }

  i = 11;

  sprintf(buf, "  page = %2d, #total apps = %d", page, nitems);
  draw_text_row(buf, i);
  i++;

  sprintf(buf, "  help:");
  draw_text_row(buf, i);
  i++;

  sprintf(buf, "  <-  Prev Page");
  draw_text_row(buf, i);
  i++;

  sprintf(buf, "  ->  Next Page");
  draw_text_row(buf, i);
  i++;

  sprintf(buf, "  0-9 Choose");
  draw_text_row(buf, i);
  i++;

  sprintf(buf, "  q Exit");
  draw_text_row(buf, i);
  i++;

  printf("========================================\n");
  printf("Please Choose.\n");
  SDL_UpdateRect(screen, 0, 0, 0, 0);

  fflush(stdout);
}
