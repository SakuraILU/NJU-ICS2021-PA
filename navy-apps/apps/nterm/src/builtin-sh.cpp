#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
#include <string.h>

#define NR_CMD sizeof(cmd_table) / sizeof(cmd_table[0])
typedef int (*handler_t)(char *);

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...)
{
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner()
{
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt()
{
  sh_printf("sh> ");
}

static int cmd_exit(char *args)
{
  exit(0);
  return 0;
}

static int cmd_echo(char *args)
{
  // printf("arg is %s\n", args);
  if (args == NULL)
  {
    sh_printf("\n");
    return 0;
  }

  // args = strtok(NULL, "");
  if (strlen(args) == 1 && (args[0] == '\"' || args[0] == '\''))
    return -1;

  if ((args[0] == '\'' && args[strlen(args) - 1] == '\'') ||
      (args[0] == '\"' && args[strlen(args) - 1] == '\"'))
  {
    memmove(args, args + 1, strlen(args));
    strcpy(args + strlen(args) - 1, "\n");
    sh_printf(args);
  }
  else if ((args[0] != '\'' && args[0] != '\"') && (args[strlen(args) - 1] != '\'' && args[strlen(args) - 1] != '\"'))
  {
    strcat(args, "\n");
    sh_printf(args);
  }
  else
    return -1;

  return 0;
}

static struct
{
  const char *cmd_name;
  handler_t handler;
} cmd_table[] = {
    {"exit", cmd_exit},
    {"echo", cmd_echo},
};

static size_t get_argc(char *str)
{
  size_t i = 0;
  if (strtok(str, " ") == NULL)
    return i;
  else
    i++;

  while (strtok(NULL, " ") != NULL)
    i++;
  return i;
}

static void get_argv(char *cmd, char **argv)
{
  int argc = 0;

  // printf("cmd is %s\n", cmd);
  argv[argc++] = strtok(cmd, " ");
  // printf("argv0 %s\n", argv[0]);

  while (true)
  {
    argv[argc++] = strtok(NULL, " ");
    if (argv[argc - 1] == NULL)
      break;
    // printf("argv%d %s\n", argc, argv[argc]);
  }
  return;
}

static void sh_handle_cmd(const char *cmd)
{
  printf("\n");
  // printf("here\n");
  // printf("cmd is %s\n", cmd);
  if (cmd[0] == '\n')
    return;

  char cmd_cpy[strlen(cmd) + 1];
  char *cmd_extract = strtok(strcpy(cmd_cpy, cmd), "\n");
  char *cmd_name = strtok(cmd_extract, " ");
  char *args = strtok(NULL, "");
  // char *cmd_end = cmd_extract + strlen(cmd_extract);

  // char *args = cmd_name + strlen(cmd_name) + 1;
  // printf("cmd is %s, args is %s\n", cmd_name, args);
  // if (args > cmd_end)
  //   args = NULL;

  for (size_t i = 0; i < NR_CMD; ++i)
  {
    // printf("compare %s with %s\n", cmd_name, cmd_table[i].cmd_name);
    if (strcmp(cmd_name, cmd_table[i].cmd_name) == 0)
    {
      if (cmd_table[i].handler(args) < 0)
        sh_printf("sh: invalid command\n");
      return;
    }
  }

  cmd_extract = strtok(strcpy(cmd_cpy, cmd), "\n");
  int argc = get_argc(cmd_extract);
  // printf("argc is %d\n", argc);

  char *(argv[argc + 1]) = {NULL};
  cmd_extract = strtok(strcpy(cmd_cpy, cmd), "\n");

  get_argv(cmd_extract, argv);
  // printf("argv0 is %s\n", argv[0]);

  if (execvp(argv[0], argv) < 0)
    sh_printf("sh: command not found: %s\n", argv[0]);

  return;
}

void builtin_sh_run()
{
  sh_banner();
  sh_prompt();

  setenv("PATH", "/bin:/usr/bin", 0);
  while (1)
  {
    SDL_Event ev;
    if (SDL_PollEvent(&ev))
    {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN)
      {
        const char *res = term->keypress(handle_key(&ev));
        // printf("res is %s\n", res);
        if (res)
        {
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
