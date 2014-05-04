#include "mpc.h"

#ifdef _WIN32

#include <string.h>

static char buffer[2048];

char *readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcopy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else

#include <editline/readline.h>

#endif

typedef struct {
  int type;
  long num;
  int err;
} lval;

long min(long x, long y) {
  if (x < y) { return x; }
  return y;
}

long max(long x, long y) {
  if (x > y) { return x; }
  return y;
}

long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  if (strcmp(op, "%") == 0) { return x % y; }
  if (strcmp(op, "min") == 0) { return min(x, y); }
  if (strcmp(op, "max") == 0) { return max(x, y); }
  return 0;
}

long eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) { return atoi(t->contents); }

  char* op = t->children[1]->contents;

  long x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}


int main(int argc, char** argv) {
  puts("FishLisp Version 0.0.0.0.2");
  puts("Press Ctrl+c to Exit\n");

  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                   \
    number   : /-?[0-9]+[.]?[0-9]*/ ;                   \
    operator : '+' | '-' | '*' | '/' | '%' | \"min\" | \"max\" ;            \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);

  while (1) {
    
    char* input = readline("lispy> ");

    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
