#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args)
{
  // printf("arg is %p\n", args);
  int argc = *((int *)args);

  char **pargs = (char **)args + 1;
  char **argv = pargs;
  // printf("argv0 is at %s\n", argv[0]);
  // printf("argv1 is at %s\n", argv[1]);

  while (*pargs != NULL)
    pargs++;

  // printf("arg is %p\n", args);
  pargs += 1;
  char **envp = (char **)pargs;
  // printf("arg is %s\n", argv[0]);
  // char *empty[] = {NULL};
  environ = envp;
  // if (environ != NULL)
  //   printf("env0 is at %s\n", environ[0]);
  // printf("env1 is at %s\n", environ[1]);
  // printf("arg is %p\n", args);
  exit(main(argc, argv, envp));
  assert(0);
}
