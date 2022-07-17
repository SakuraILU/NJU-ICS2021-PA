#include <proc.h>
#include <klib.h>
#include <fs.h>
#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
Context *context_kload(PCB *pcb, void (*entry)(void *), void *arg);
Context *context_uload(PCB *pcb, char *filename, char *const argv[], char *const envp[]);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    // Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
    j++;
    yield();
  }
}

static char args_hello_fun[] = "process 1";
// static char *args1[] = {"/bin/pal", "--skip", NULL};
// static char *envp1[] = {"PATH=/bin", "PATH=/usr/bin", NULL};
static char *args_menu[] = {"/bin/menu", NULL};
// static char *args_pal[] = {"/bin/pal", "--skip", NULL};
// static char *args_exec_test[] = {"/bin/exec-test", "1000", NULL};
// static char arg2[] = "process 2";
void init_proc()
{
  switch_boot_pcb();

  Log("Initializing processes...");

  // naive_uload(NULL, "/bin/menu");

  pcb[0].cp = context_kload(&pcb[0], hello_fun, args_hello_fun);
  // pcb[1].cp = context_uload(&pcb[1], "/bin/pal", args_pal, NULL);
  pcb[1].cp = context_uload(&pcb[1], "/bin/menu", args_menu, NULL);
  // pcb[1].cp = context_uload(&pcb[1], "/bin/exec-test", args_exec_test, NULL);

  switch_boot_pcb();

  // load program here
}

Context *schedule(Context *prev)
{
  // printf("schedule a new process\n");
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = (current == &pcb[0]) ? &pcb[1] : &pcb[0];

  // then return the new context
  // printf("entry hello_fun is %p\n", (int)hello_fun);
  return current->cp;
  // return NULL;
}

int execve(const char *pathname, char *const argv[], char *const envp[])
{
  // printf("pathname is %s\n", pathname);
  int fd = fs_open(pathname, 0, 0);
  if (fd == -1)
  {
    return -1;
  }
  else
    fs_close(fd);

  // char* pathname2 = "/bin/pal";
  // printf("brk is %p\n",heap_brk);
  // naive_uload(NULL, pathname);
  // if (argv != NULL)
  //   for (int i = 0; argv[i] != NULL; ++i)
  //     printf("argv%d is %s\n", i, argv[i]);
  // if (envp != NULL)
  //   for (int i = 0; envp[i] != NULL; ++i)
  //     printf("envp%d is %s\n", i, envp[i]);

  // printf("envp %s\n", envp[0]);
  pcb[1].cp = context_uload(&pcb[1], (char *)pathname, argv, envp);
  switch_boot_pcb();
  yield();

  return 0;
}