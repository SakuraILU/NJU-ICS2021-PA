#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#include<limits.h>
word_t vaddr_read(vaddr_t addr, int len);

enum {
  TK_NOTYPE = 256, TK_EQ, 

  /* TODO: Add more token types */
  TK_NUM, TK_UEQ, TK_AND, TK_REG, TK_HEX, TK_DEF,
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"==", TK_EQ},        // equal
  {"\\+", '+'},         // plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\b[0-9]+\\b", TK_NUM},
  {"\\(", '('},
  {"\\)", ')'},
  {"!=", TK_UEQ},
  {"&&", TK_AND},
  {"\\$(a[0-7]|t[0-6p]|s[0-9p]|s1[0-1]|ra|gp|pc|mtvec|mstatus|mepc|mcause)", TK_REG},
  {"\\b0x[0-9a-fA-F]+\\b", TK_HEX},
};


#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          case TK_HEX:
          case TK_REG:
          case TK_NUM:{
            Assert(substr_len < sizeof(tokens[nr_token].str)/sizeof(tokens[nr_token].str[0]), "The Token length is out of bound!");
            // memset(tokens[nr_token].str, '\0', sizeof(tokens[nr_token].str));
            // important bug!
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
          }
          default:{
            tokens[nr_token].type = rules[i].token_type;
            nr_token ++;
          };
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      // printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // Log("num is %d",nr_token);
  // for(int i = 0; i < nr_token; ++i)
  //   printf("[%d, %s]", tokens[i].type, tokens[i].str);
  return true;
}

typedef struct result
{
  uint32_t exp_data;
  bool is_valid;
  /* data */
} Result;
static Result eval(Token *tokens, int p,int q);


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  // /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || !(tokens[i - 1].type == TK_NUM || tokens[i - 1].type == ')' 
                                            || tokens[i - 1].type == TK_HEX || tokens[i-1].type == TK_REG) 
                                  )
       ) {
      tokens[i].type = TK_DEF;
    }
  }


  Result res = eval(tokens, 0, nr_token - 1);
  *success = res.is_valid;
  // Log("[%d,%d]",res.exp_data,res.is_valid);
  return res.exp_data;
}


static bool check_parentheses(Token *tokens, int p, int q){
  if(!(tokens[p].type == '(' && tokens[q].type == ')')) return false;

  int match_cnt = 0;
  for(int i = p; i <= q; ++i){
    if(tokens[i].type == '(') match_cnt ++;
    else if(tokens[i].type == ')') match_cnt --;
  }

  return match_cnt == 0;
}

static int check_order(Token *token){
  if(token->type == TK_AND) return 0;
  else if(token->type == TK_EQ || token->type == TK_UEQ) return 1;
  else if(token->type == '+' || token->type == '-') return 2;
  else if(token->type == '*' || token->type == '/') return 3;
  else if(token->type == TK_DEF) return 4;
  else return INT_MAX;
}

static Result eval(Token *tokens, int p,int q) {
  Result res;
  if (p > q) {
    /* Bad expression */
    res.is_valid = 0;
    return res;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    
    if(tokens[p].type == TK_NUM) {res.exp_data = atoi(tokens[p].str);}
    else if(tokens[p].type == TK_HEX) { res.exp_data = strtol(tokens[p].str + 2, NULL, 16);}
    else if(tokens[p].type == TK_REG){
      bool success = false;
      res.exp_data = isa_reg_str2val(tokens[p].str , &success);
      if(success == false){/* printf("Invalid register!");*/ res.is_valid = false; }
      // Log("reg %s is %d\n", tokens[p].str, isa_reg_str2val(tokens[p].str , &success));
    }
    res.is_valid = true;
    return res;
  }
  else if (check_parentheses(tokens, p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(tokens, p + 1, q - 1);
  }
  else {
    // op = the position of 主运算符 in the token expression;
    int match_cnt = 0;
    int op = p;
    for(int i = p; i <= q; ++ i){
      if(tokens[i].type == '(') match_cnt ++;
      else if(tokens[i].type == ')') match_cnt --;
      else if(match_cnt == 0){
        if(match_cnt == 0 && (check_order(tokens + op) >= check_order(tokens + i)) ){
          op = i;
        }
      }
    }

    // Log("operator is %c at %d",tokens[op].type, op);
    if(tokens[op].type == TK_DEF){
      Result val = eval(tokens, op + 1, q);
      res.is_valid = val.is_valid;
      res.exp_data = vaddr_read(val.exp_data,4);
      return res;
    }
    else{
      if(check_order(tokens + op) == INT_MAX){ res.is_valid = false; return res; }
      Result val1 = eval(tokens, p, op - 1);
      Result val2 = eval(tokens, op + 1, q);
      // Log("val1 is %d, val2 is %d ", val1.exp_data, val2.exp_data);
      if(val1.is_valid && val2.is_valid) res.is_valid = true;
      else{ res.is_valid = false; return res; }

      switch (tokens[op].type) {
        case '+': {res.exp_data = val1.exp_data + val2.exp_data; break;}
        case '-': {res.exp_data = val1.exp_data - val2.exp_data; break;}
        case '*': {res.exp_data = val1.exp_data * val2.exp_data; break;}
        case '/': {res.exp_data = val1.exp_data / val2.exp_data; break;}
        case TK_UEQ : {res.exp_data = (val1.exp_data != val2.exp_data); break;}
        case TK_EQ : {res.exp_data = (val1.exp_data == val2.exp_data); break;}
        case TK_AND : {res.exp_data = (val1.exp_data && val2.exp_data); break;}
        default: {Log("INvalid Operator!"); assert(0);/*printf("Invalid operator in expression!");*/ res.is_valid = false; break;}
      }
      return res;
    }
  }
}
