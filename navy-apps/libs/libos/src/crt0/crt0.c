#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args)
{
  // printf("arg is %p\n", args);
  // write(1, "here\n", 10);
  int argc = *((int *)args);

  char **pargs = (char **)args + 1;
  char **argv = pargs;
  // printf("\n\n\n\n");
  // printf("argv0 is  %s\n", argv[0]);
  // if (strcmp(argv[0], "/bin/hello"))
  //   exit(1);
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
