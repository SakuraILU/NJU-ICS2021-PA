#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <isa.h>

static int is_batch_mode = false;

// void init_regex();
// void init_wp_pool();

void isa_reg_display();
word_t vaddr_read(vaddr_t addr, int len);
// word_t expr(char *e, bool *success);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_END;
  return -1;
}

static int cmd_si(char *args){
  if(args != NULL) cpu_exec(atoi(args));
  else cpu_exec(1);

  return 0;
}

void traverse_wp();
static int cmd_info(char *args){
  //in case, input w will case segmentation fault
  //strcmp will dereference args, *NULL cause segmentation fault
  if(args == NULL) return 0;

  if(strcmp(args,"r") == 0) isa_reg_display();
  else if(strcmp(args,"w") == 0) traverse_wp();
  else printf("Invalid command!\n");
  return 0;
}


static int cmd_x(char *args){
  char *cnt_str = strtok(NULL, " ");
  if(cnt_str != NULL){
    int cnt = atoi(cnt_str);

    char *addr_str = strtok(NULL, " ");
    if(addr_str != NULL){
      if(strlen(addr_str)>=2 && addr_str[0] == '0' && addr_str[1] == 'x'){
        int addr = (int)strtol(addr_str+2,NULL,16);
        printf("%-14s%-28s%-s\n","Address","Hexadecimal","Decimal");
        for(int i = 0; i < cnt; ++i){
          printf("0x%-12x0x%02x  0x%02x  0x%02x  0x%02x",(addr),vaddr_read(addr,1),vaddr_read(addr+1,1),vaddr_read(addr+2,1),vaddr_read(addr+3,1));
          printf("\t  %04d  %04d  %04d  %04d\n",vaddr_read(addr,1),vaddr_read(addr+1,1),vaddr_read(addr+2,1),vaddr_read(addr+3,1));
          addr += 4;
        }
      }
      else
        printf("the result of the given expression is NOT hexadecimal!");
    }
  }
  return 0;
}

static int cmd_p(char *args){
  if(args == NULL) return 0;

  bool success = false;
  int res = expr(args,&success);
  if(success == false) printf("Invalid Expression\n");
  else printf(">  %u\n", res);
  return 0;
}

bool new_wp(char *e);

static int cmd_w(char *args){
  if(args == NULL) return 0;

  if(!new_wp(args)) printf("the number of watch point is out of bound!\n");

  return 0;
}

void free_wp(int n);

static int cmd_d(char *args){
  if(args == NULL) return 0;

  free_wp(atoi(args));
  return 0;
}

// extern NEMUState nemu_state;
int save_mem(FILE *fp);
int save_regs(FILE* fp);
#define FILENAME_LEN 128
static int cmd_save(char *args){
  if(args == NULL) return 0;
  char *history_name = strtok(NULL, " ");

  char *home_path = getenv("NEMU_HOME");
  char filename[FILENAME_LEN];
  strcpy(filename, home_path);
  strcat(filename, "/src/monitor/nemu_history/");
  strcat(filename, history_name);

  FILE* fp = fopen(filename, "w");
  int ret = save_regs(fp);
  ret = save_mem(fp);
  
  return ret;
}
int load_mem(FILE *fp);
int load_regs(FILE* fp);
static int cmd_load(char *args){
  if(args == NULL) return 0;
  char *history_name = strtok(NULL, " ");

  char *home_path = getenv("NEMU_HOME");
  char filename[FILENAME_LEN];
  strcpy(filename, home_path);
  strcat(filename, "/src/monitor/nemu_history/");
  strcat(filename, history_name);

  FILE* fp = fopen(filename, "r");
  int ret = load_regs(fp);
  ret = load_mem(fp);
  
  return ret;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* TODO: Add more commands */
  { "si", "Let the program single step through N instructions and pause execution, When N is not given, it defaults to 1", cmd_si },
  { "info", "print register status or watchpoint information", cmd_info},
  { "x" , "Evaluate the expression EXPR, using the result as the starting memory address, output N consecutive 4-bytes in hexadecimal", cmd_x},
  { "p", "Evaluate the expression EXPR", cmd_p},
  { "w", "Suspend program execution when the value of the expression EXPR changes", cmd_w},
  { "d", "delete the watchpoint with sequence number n", cmd_d},
  { "save", "save the snapshot of nemu", cmd_save},
  { "load", "load the snapshot of nemu", cmd_load},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }
    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
