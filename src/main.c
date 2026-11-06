#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

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
  int i, exit_code, return_code, is_debug;
  char *command;
  const Command *c;
  Node *tokens, *token;
  Shell *shell;
  branch *n1, *n2, *n3, *n4;

  exit_code = 0;
  is_debug = argc >= 2 && !strcmp(argv[1], "--debug");

  /* TEST: Construction d'un arbre */
  if ((shell = (Shell *)malloc(sizeof(Shell))) == NULL) {
    return 1;
  }
  if ((n1 = branch_create("n1", Folder)) == NULL) {
    return 1;
  }
  if ((n2 = branch_create("n2", Folder)) == NULL) {
    return 1;
  }
  if ((n3 = branch_create("n3", File)) == NULL) {
    return 1;
  }
    if ((n4 = branch_create("n4", File)) == NULL) {
    return 1;
  }
  branch_add_child(n1, n2);
  branch_add_child(n2, n3);
  branch_add_child(n2, n4);
  shell->root = n1;
  shell->cwd = n2;
  shell->debug_mode = is_debug;

  while (1) {
    printf("minish $");
    tokens = lexer();
    command = tokens->data;

    char **user_arg = tokens->next ? (char **)tokens->next->data : NULL;
    
/*     if (user_arg && user_arg[0]) {
      printf("arg : [%s]\n", (char *)user_arg);
    }
    else {
      printf("no argument\n");
    } */
    

    if (quit_shell(tokens, &exit_code)) {
      lexer_free(&tokens);
      break;
    }

    c = command_find(command);

    Node * command_name = tokens;
    int arg_number=0;

    while(command_name->next != NULL){
      command_name = command_name->next;
      arg_number++;
    }
    
/*     printf("%d\n", arg_number);
 */
    return_code = c ? c->f(shell, arg_number, (char **)user_arg) : 127;

    if (return_code == 127) {
      printf("command not found: %s\n\n", command);
    }

    /* NOTE: --debug en 1er argument du minishell */
    if (is_debug) {
      i = 0;
      printf(">> tokens: ");
      for (token = tokens; token; token = token->next) {
        if (i)
          printf(" -> ");
        printf("(%s)", token->data);
        i++;
      }
      printf("\n>> return code: %d\n\n", return_code);
    }

    lexer_free(&tokens);
  }

  branch_free(shell->root);
  free(shell);

  return exit_code;
}
