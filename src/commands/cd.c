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

  branch *current_branch;

  if (argv[0][0] == '/')
    current_branch = shell->root;
  else
    current_branch = shell->cwd;

  Node *first_word = lexer(argv[0], "/");
  Node *current_word;

  for (current_word = first_word; current_word != NULL; current_word = current_word->next ) {
    // cd.
    if (!strcmp(current_word->data, ".")){
        // on fait rien, trouver un autre moyen -- peut-être un continue pour rendre joli
    }
    // cd ..
    else if (!strcmp(current_word->data, "..")) {
        if (current_branch->parent)
            current_branch = current_branch->parent;
    }
    // cd [chemin]
    else {
        branch *target = branch_find_child(current_branch, current_word->data);

        if (!target) {
            printf("cd : \"%s\" est introuvable\n", current_word->data);
            lexer_free(&first_word);
            return EXIT_FAILURE;
        }

        if (target->type != Folder) {
            printf("cd : \"%s\" n'est pas un dossier\n", current_word->data);
            lexer_free(&first_word);
            return EXIT_FAILURE;
        }

        current_branch = target;
    }
  }
  shell->cwd = current_branch;

  lexer_free(&first_word);

  if (shell->debug_mode)
    printf("[TRACE] Fin fonction minishell_cd\n");
  return EXIT_SUCCESS;
}
