#include <proc.h>
#include <klib.h>
#include <fs.h>
#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
Context *context_kload(PCB *pcb, void (*entry)(void *), void *arg);
Context *context_uload(PCB *pcb, char *filename, char *const argv[], char *const envp[]);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
uint32_t NR_PROC = 0;
static PCB pcb_boot = {};
PCB *current = NULL;
PCB *fg_pcb = NULL;

void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
    j++;
    // yield();
  }
}

#define TASKS(_) _(HELLO), _(MENU), _(NTERM), _(NSLIDER), _(BIRD), _(PAL)
#define TASK_NAME(name) TASK_##name
enum
{
  TASKS(TASK_NAME)
};

// static char args_khello_1[] = "process 1";
// static char args_khello_2[] = "process 2";

static char *args_hello[] = {"/bin/hello", NULL};
static char *args_menu[] = {"/bin/nterm", NULL};
static char *args_nterm[] = {"/bin/nterm", NULL};
static char *args_nslider[] = {"/bin/nslider", NULL};
static char *args_bird[] = {"/bin/bird", NULL};
static char *args_pal[] = {"/bin/pal", "--skip", NULL};
typedef struct Task
{
  char *name;
  char **args;
  char **envp;
} Task;
Task utask_table[] = {
    [TASK_HELLO] = {"/bin/hello", args_hello, NULL},
    [TASK_MENU] = {"/bin/menu", args_menu, NULL},
    [TASK_NTERM] = {"/bin/nterm", args_nterm, NULL},
    [TASK_NSLIDER] = {"/bin/nslider", args_nslider, NULL},
    [TASK_BIRD] = {"/bin/bird", args_bird, NULL},
    [TASK_PAL] = {"/bin/pal", args_pal, NULL},
};

void init_proc()
{
  switch_boot_pcb();

  Log("Initializing processes...");
  // naive_uload(NULL, "/bin/menu");
  // pcb[0].cp = context_kload(&pcb[0], hello_fun, args_hello_fun1);
  pcb[0].cp = context_uload(&pcb[0], utask_table[TASK_HELLO].name, utask_table[TASK_HELLO].args, utask_table[TASK_HELLO].envp);
  assert(NR_PROC >= 0 && NR_PROC <= MAX_NR_PROC);
  pcb[1].cp = context_uload(&pcb[1], utask_table[TASK_NTERM].name, utask_table[TASK_NTERM].args, utask_table[TASK_NTERM].envp);
  assert(NR_PROC >= 0 && NR_PROC <= MAX_NR_PROC);
  pcb[2].cp = context_uload(&pcb[2], utask_table[TASK_BIRD].name, utask_table[TASK_BIRD].args, utask_table[TASK_BIRD].envp);
  assert(NR_PROC >= 0 && NR_PROC <= MAX_NR_PROC);
  pcb[3].cp = context_uload(&pcb[3], utask_table[TASK_PAL].name, utask_table[TASK_PAL].args, utask_table[TASK_PAL].envp);
  assert(NR_PROC >= 0 && NR_PROC <= MAX_NR_PROC);
  fg_pcb = &pcb[1];

  switch_boot_pcb();

  // load program here
}

static uint64_t vttime[2] = {15, 1};
static uint64_t duration[2] = {0};
static uint32_t cur_pcb_id = 0;
#define argmin(a, b) (((a) < (b)) ? 0 : 1)
Context *schedule(Context *prev)
{
  // printf("schedule a new process\n");
  // save the context pointer
  current->cp = prev;

  // printf("current process is %d\n", process_id);
  // always select pcb[0] as the new process
  duration[cur_pcb_id] += vttime[cur_pcb_id];
  // current = (current == &pcb[0]) ? fg_pcb : &pcb[0];
  cur_pcb_id = argmin(duration[0], duration[1]);
  current = (cur_pcb_id == 0) ? &pcb[0] : fg_pcb;
  // process_id = (process_id + 1) % NR_PROC;
  // current = &pcb[process_id];

  // Log("current process id is %d\n", process_id);
  // uint32_t process_id = next_process();
  // current = &pcb[process_id];
  // schedule_cnt[process_id] += virtual_cnt[process_id];
  // next_process();
  // process_id = next_process();
  // if (current == &pcb[0])
  // printf("change to process %d\n", (current == &pcb[0]) ? 0 : 1);

  // then return the new context
  // printf("entry hello_fun is %p\n", (int)hello_fun);
  // printf("change to ctx %p\n", current->cp);
  // printf("epc is %p\n", current->cp->mepc);
  // printf("sp and ksp is %p, %p\n", current->cp, current->cp->mscratch);
  return current->cp;
  // return NULL;
}

// uint32_t uarray_min(uint64_t array)
// {
//   uint32_t min_pos = 0;
//   for (size_t i = 0; i < NR_PROC; ++i)
//   {
//     if (array[i] < array[min_pos])
//       min_pos = i;
//   }
//   return i;
// }

int execve(const char *pathname, char *const argv[], char *const envp[])
{
  // char *envp1[] = {NULL};
  // printf("execve pathname is %s\n", pathname);
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
  current->cp = context_uload(current, (char *)pathname, argv, envp);
  NR_PROC--;
  assert(NR_PROC >= 0 && NR_PROC <= MAX_NR_PROC);
  switch_boot_pcb();
  // printf("yield for execve\n");
  // printf("return ctx address is %p\n", pcb[1].cp);
  // pre_process();
  yield();
  // printf("return address is %p\n", pcb[1].cp);
  // schedule(NULL);

  return 0;
}

void exit(int status)
{
  if (status == 0)
  {
    execve("/bin/nterm", NULL, NULL);
  }
  else
    halt(status);
}

void set_fg_pcb(uint32_t process_id)
{
  fg_pcb = &pcb[process_id];
  if (duration[1] < duration[0])
    duration[1] = duration[0];
}