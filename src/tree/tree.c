#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

struct branch {
  char nom[64];
  branch_type type;
  branch *parent;
  branch *next_sibling;
  branch *first_child;
};

branch *branch_create(char *nom, branch_type type) {
  branch *p = malloc(sizeof(branch));

  if (p == NULL) {
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
  for (branch *c = parent->first_child; c != NULL; c = c->next_sibling) {
    if (strcmp(c->nom, nom) == 0) {
      return c;
    }
  }
  return NULL;
}

int branch_add_child(branch *parent, branch *child) {
  if (parent->type != Lieu) {
    return 0;
  }
  if (branch_find_child(parent, child->nom) != NULL) {
    return 0;
  }

  child->parent = parent;

  if (parent->first_child == NULL) {
    parent->first_child = child;
  } else {
    branch *dernier = parent->first_child;
    while (dernier->next_sibling != NULL) {
      dernier = dernier->next_sibling;
    }
    dernier->next_sibling = child;
  }
  return EXIT_SUCCESS;
}

void branch_print(branch *b, int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
  printf("%s\n",
         b->nom); // à la fin du nom si c'est un dossier, ajouter un slash
  // printf("%s%s", b->nom, blabla)

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
