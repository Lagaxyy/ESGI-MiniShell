#ifndef COMMANDS_H
#define COMMANDS_H

#define FILE_BASE_TREE "tests/shell.tree"

typedef struct branch branch;

typedef struct Shell {
  branch *root;
  branch *cwd;
  int debug_mode;
} Shell;

typedef int (*command_func)(Shell *shell, int argc, char **argv);

typedef struct Command {
  char *name;
  char *description;
  command_func f;
  char *usage;
  char *args;
} Command;

const Command *command_find(const char *name);
int command_help(Shell *shell, int argc, char **argv);

/* Prototypes des commandes */
int minishell_pwd(Shell *shell, int argc, char **argv);
int minishell_ls(Shell *shell, int argc, char **argv);
int minishell_cd(Shell *shell, int argc, char **argv);
int minishell_touch(Shell *shell, int argc, char **argv);
int minishell_submit(Shell *shell, int argc, char **argv);
int minishell_mission(Shell *shell, int argc, char **argv);

#endif /* COMMANDS_H */
