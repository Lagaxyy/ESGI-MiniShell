#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "lexer/lexer.h"
#include "tree/tree.h"

void touch_cur(char *argv, branch *b) {

  branch *nb = branch_create(argv, File);
  branch_add_child(b, nb);
}

int touch_root(char *argv, branch *b) {

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

    if (target->type == Folder && strcmp(prev->data, target->nom) == 0) {
      branch *nb = branch_create(last->data, File);
      branch_add_child(target, nb);
      printf("created\n");
      break;
    }
  }
  printf("\n");

  free(tokens);
  return 0;
}

int touch_cwd_path(char *argv, branch *b) {

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
          "touch : impossible d'accéder à \"%s\" : Aucun dossier de ce type\n",
          argv);
      free(tokens);
      return 127;
    }

    cur = fs;

    if (fs->type == Folder && strcmp(fs->nom, prev->data) == 0) {
      branch *nb = branch_create(last_t->data, File);
      branch_add_child(fs, nb);
    }
  }

  free(tokens);
  return 0;
}

int minishell_touch(Shell *shell, int argc, char **argv) {
  if (argv == NULL) {
    printf("touch: opérande de fichier manquant\n");
    printf("Saisissez \"help\" pour plus d'informations.\n");
    return -1;
  } else if (strstr(*argv, "/") && *argv[0] != '/') {
    touch_cwd_path(*argv, shell->cwd);
  } else if (argc == 1 && *argv[0] != '/') {
    touch_cur(argv[0], shell->cwd);
    return 0;
  } else if (*argv[0] == '/') {
    touch_root(argv[0], shell->root);
  }

  return 0;
}
