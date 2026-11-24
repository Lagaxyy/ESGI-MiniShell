#include <stdio.h>

#include "commands.h"
#include "tree/tree.h"

void recursive_print_pwd(branch *b) {
  if (!b || !b->parent) {
    return;
  }

  recursive_print_pwd(b->parent);
  printf("/%s", b->nom);
}

int minishell_pwd(Shell *shell, int argc, char **argv) {
  if (shell->debug_mode)
    printf("[TRACE] Début fonction minishell_pwd\n");

  if (shell->root == shell->cwd)
    putchar('/');
  else
    recursive_print_pwd(shell->cwd);

  if (shell->debug_mode)
    printf("\n[TRACE] Fin fonction minishell_pwd");

  printf("\n\n");
  return 0;
}
