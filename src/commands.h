#ifndef COMMANDS_H
#define COMMANDS_H

#include "lexer/lexer.h"
#include "tree/tree.h"

typedef struct Shell {
  branch *root;
  branch *cwd; // current position
  int debug_mode;
} Shell;

typedef int (*command_func)(Shell *shell, int argc, char **argv);

typedef struct Command {
  char *name;
  char *description;
  command_func f;
  char *usage;
} Command;

const Command *command_find(const char *name);
int command_help(Shell *shell, int argc, char **argv);

/* Prototypes des commandes */
int minishell_pwd(Shell *shell, int argc, char **argv);

int minishell_ls(Shell *shell, int argc, char **argv);

#endif /* COMMANDS_H */
