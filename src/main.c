#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "lexer/lexer.h"
#include "tree/tree.h"

/**
 * Affiche la liste chaînée. Pour débugger.
 *
 * @param node la liste chaînée
 */
void Node_print(Node *node) {
  Node *n;
  int i = 0;

  for (n = node; n; n = n->next) {
    if (i)
      printf(" -> ");
    printf("(%s)", n->data);
    i++;
  }
  putchar('\n');
}

/**
 * Quitte la simulation du shell avec ces mots-clés: "bye", "quit", "exit"
 *
 * @param node la ligne de commande
 * @param exit_code pointe vers la valeur de sortie du programme
 *
 * @return 1 si la fonction s'est bien déroulée, 0 sinon
 */
int quit_shell(Node *node, int *exit_code) {
  size_t i = 0;
  static const char *keywords[] = {"bye", "quit", "exit", NULL};
  const char *keyword = *keywords;
  char *word = node->data;

  while (keyword) {
    if (!strcmp(keyword, word)) {
      if (node->next) {
        *exit_code = atoi(node->next->data);
      } else {
        *exit_code = 0;
      }

      return 1;
    }

    keyword = *(keywords + ++i);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  int exit_code, return_code, is_debug;
  char *command;
  const Command *c;
  Node *tokens, *n;
  Shell *shell;
  branch *root;

  exit_code = 0;
  is_debug = argc >= 2 && !strcmp(argv[1], "--debug");

  /* TEST: Construction d'un arbre */
  if ((root = branch_load(FILE_BASE_TREE)) == NULL) {
    fprintf(stderr, "minishell: erreur chargement arborescence depuis \"%s\"\n",
            FILE_BASE_TREE);
    return 1;
  }
  if ((shell = (Shell *)malloc(sizeof(Shell))) == NULL) {
    fprintf(stderr, "minishell: erreur allocation shell\n");
    return 1;
  }
  shell->root = root;
  shell->cwd = root;
  if (is_debug) {
    printf("Arborescence chargée depuis \"%s\":\n", FILE_BASE_TREE);
    branch_print(shell->root, 0);
    printf("\n");
  }

  /* NOTE: Simulation du shell */
  while (1) {
    printf("minish $");
    tokens = lexer(NULL, NULL);
    command = tokens->data;

    if (quit_shell(tokens, &exit_code)) {
      lexer_free(&tokens);
      break;
    }

    c = command_find(command);

    int arg_count = 0;
    for (n = tokens->next; n != NULL; n = n->next)
      arg_count++;

    char **args = NULL;
    if (arg_count > 0) {
      args = malloc((size_t)(arg_count + 1) * sizeof(*args));
      if (args == NULL) {
        lexer_free(&tokens);
        continue;
      }

      int k = 0;
      for (n = tokens->next; n != NULL; n = n->next)
        args[k++] = n->data;
      args[arg_count] = NULL;
    }

    return_code = c ? c->f(shell, arg_count, args) : 127;

    if (return_code == 127) {
      printf("command not found: %s\n\n", command);
    }

    /* INFO: --debug en 1er argument du minishell (ou via make debug), affiche
     * des informations supplémentaires */
    if (is_debug) {
      printf(">> tokens: ");
      Node_print(tokens);
      printf(">> return code: %d\n\n", return_code);
    }

    free(args);
    lexer_free(&tokens);
  }

  branch_free(shell->root);
  free(shell);

  return exit_code;
}
