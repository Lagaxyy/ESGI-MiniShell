#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"

void rm_cur(char *argv, branch *b) {

  branch_free(b->first_child);
  b = NULL;
}

int rm_root(char *argv, branch *b) {

  Node *tokens = lexer(argv, "/");
  branch *cur = b;

  Node *t = NULL;

  Node *prev = NULL;
  Node *last = NULL;

  for (t = tokens; t != NULL; t = t->next) {
    prev = last;
    last = t;
  }

  for (t = tokens; t != last; t = t->next) {

    branch *target = branch_find_child(cur, t->data);

    if (!target) {
      printf("null\n");
      return 1;
    }

    cur = target;

    if (strcmp(prev->data, target->nom) == 0) {
      branch_free(target);
      target = NULL;
    }
  }
  printf("\n");

  free(tokens);
  return 0;
}

int rm_cwd_path(char *argv, branch *b) {

  Node *t;
  Node *tokens = lexer(argv, "/");

  Node *last_t = NULL;
  Node *prev = NULL;

  branch *cur = b;

  for (t = tokens; t != NULL; t = t->next) {
    prev = last_t;
    last_t = t;
  }

  for (t = tokens; t != last_t; t = t->next) {

    branch *fs = cur->first_child;

    while (fs != NULL && strcmp(fs->nom, t->data) != 0) {
      fs = fs->next_sibling;
    }
    if (fs == NULL) {
      printf(
          "rm : impossible d'accéder à \"%s\" : Aucun dossier de ce type\n",
          argv);
      free(tokens);
      return 127;
    }

    cur = fs;

    if (strcmp(fs->nom, prev->data) == 0) {
      branch_free(fs);
      fs = NULL;
    }
  }

  free(tokens);
  return 0;
}

int minishell_rm(Shell *shell, int argc, char **argv) {
  if (argv == NULL) {
    printf("rm: opérande de fichier manquant\n");
    printf("Saisissez \"help\" pour plus d'informations.\n");
    return -1;
  } else if (strstr(*argv, "/") && *argv[0] != '/') {
    rm_cwd_path(*argv, shell->cwd);
  } else if (argc == 1 && *argv[0] != '/') {
    rm_cur(argv[0], shell->cwd);
    return 0;
  } else if (*argv[0] == '/') {
    rm_root(argv[0], shell->root);
  }
  return 0;
}
