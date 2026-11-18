#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

branch *branch_create(char *nom, branch_type type) {
  branch *p = malloc(sizeof(branch));

  if (p == NULL || !nom || strlen(nom) == 0 || strlen(nom) >= 64) {
    return NULL;
  }

  strcpy(p->nom, nom);
  p->type = type;
  p->parent = NULL;
  p->next_sibling = NULL;
  p->first_child = NULL;

  return p;
}

branch *branch_find_child(branch *parent, char *nom) {
  if (!parent)
    return NULL;

  branch *c;

  for (c = parent->first_child; c != NULL; c = c->next_sibling) {
    if (strcmp(c->nom, nom) == 0)
      return c;
  }

  return NULL;
}

branch *branch_find(branch *b, char *nom) {
  if (!b) {
    return NULL;
  }

  branch *c, *rec;

  if (strcmp(b->nom, nom) == 0) {
    return b;
  }

  for (c = b->first_child; c != NULL; c = c->next_sibling) {
    if ((rec = branch_find(c, nom)) != NULL) {
      return rec;
    }
  }

  return NULL;
}

int branch_add_child(branch *parent, branch *child) {
  if (parent->type != Folder) {
    return EXIT_FAILURE;
  }

  if (branch_find_child(parent, child->nom)) // évite ajout dossier ou fichier
                                             // de même nom directement ici
    return EXIT_FAILURE;

  if (parent->first_child == NULL) {
    parent->first_child = child;
    child->parent = parent;
  } else {
    branch *dernier = parent->first_child;
    while (dernier->next_sibling != NULL) {
      dernier = dernier->next_sibling;
    }
    child->parent = parent;
    dernier->next_sibling = child;
  }
  return EXIT_SUCCESS;
}

void branch_print(branch *b, int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
  printf("%s%s\n", b->nom, (b->type == Folder) ? "/" : "");

  for (branch *n = b->first_child; n != NULL; n = n->next_sibling) {
    branch_print(n, depth + 1);
  }
}

void branch_free(branch *p) {
  if (p == NULL) {
    return;
  }

  branch *c = p->first_child;
  while (c != NULL) {
    branch *suivant = c->next_sibling;
    branch_free(c);
    c = suivant;
  }
  free(p);
}
