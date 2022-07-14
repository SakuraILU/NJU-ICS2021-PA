#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
#include <string.h>

#define NR_CMD sizeof(cmd_table)/sizeof(cmd_table[0])
typedef int (*handler_t)(char*);

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}


static int cmd_exit(char* args) {
  exit(0);
  return 0;
}

static int cmd_echo(char* args) {
  args = strtok(NULL, " ");
  if( (args[0] == '\'' && args[strlen(args)-1] == '\'') 
      ||
      (args[0] == '\"' && args[strlen(args)-1] == '\"')){
    memmove(args, args+1, strlen(args));
    strcpy(args + strlen(args) -1, "\n");
    sh_printf(args);
  }
  else if( (args[0] != '\'' && args[0] != '\"') && (args[strlen(args)-1] != '\'' && args[strlen(args)-1] != '\"') ){
    strcat(args,"\n");
    sh_printf(args);
  }
  else return -1;
  
  return 0;
}

static struct 
{
  const char* cmd_name;
  handler_t handler;
} cmd_table[] = {
                  {"exit", cmd_exit},
                  {"echo", cmd_echo},
                };


static void sh_handle_cmd(const char *cmd) {
  // printf("cmd is %s\n",cmd);
  if(cmd[0] == '\n') return;

  char *cmd_str = (char*)cmd;
  cmd_str = strtok(cmd_str,"\n");
  char* cmd_end = cmd_str + strlen(cmd_str);

  cmd = strtok(cmd_str, " ");
  char* args = cmd_str + strlen(cmd_str) + 1;
  if(args > cmd_end) args = NULL;

  // printf("cmd is %s, args is %s\n",cmd, args);
  for(size_t i = 0; i < NR_CMD; ++i){
    if(strcmp(cmd, cmd_table[i].cmd_name) == 0){
      if(cmd_table[i].handler(args) < 0) sh_printf("  > Invalid Command\n");
      else return;
    }
  }

  execvp(cmd, NULL);
  
  return;
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  setenv("PATH", "/bin", 0);
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    // printf("here\n");
    refresh_terminal();
    // printf("here\n");
  }
}
