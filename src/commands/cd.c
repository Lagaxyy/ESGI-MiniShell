#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

int minishell_cd(Shell *shell, int argc, char **argv) {
  if (shell->debug_mode)
    printf("[TRACE] Début fonction minishell_cd\n");

  // garde-fou : trop d'arguments
    if (argc > 1) {
    printf("cd : only one argument expected\n");
    return EXIT_FAILURE;
  }

  // cd 
  if (argc == 0) {
    shell->cwd = shell->root;
    return EXIT_SUCCESS;
  }

  // cd .
  if (!strcmp(argv[0], "."))
    return EXIT_SUCCESS;

  // cd ..
  if (!strcmp(argv[0], "..")) {
    if (shell->cwd->parent)
        shell->cwd = shell->cwd->parent;
    return EXIT_SUCCESS;
  }

  // cd target
  branch *target;
  target = branch_find_child(shell->cwd, argv[0]);

  if (!target) {
    printf("cd : \"%s\" est introuvable\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (target->type != Folder) {
    printf("cd : \"%s\" n'est pas un dossier\n", target->nom);
    return EXIT_FAILURE;
  }

  shell->cwd = target;

  if (shell->debug_mode)
    printf("[TRACE] Fin fonction minishell_cd\n");
  return EXIT_SUCCESS;
}