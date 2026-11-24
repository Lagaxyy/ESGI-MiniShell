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
    return 0;
  }

  /* NOTE: évite ajout dossier ou fichier de même nom directement ici */
  if (branch_find_child(parent, child->nom))
    return 0;

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
  return 1;
}

void branch_print(branch *b, int depth) {
  int i;
  branch *n;

  for (i = 0; i < depth; i++) {
    printf("  ");
  }
  printf("%s%s\n", b->nom, (b->type == Folder) ? "/" : "");

  for (n = b->first_child; n != NULL; n = n->next_sibling) {
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

/**
 * Vérifie si les branches en argument sont identiques (même nom et même chemin
 * vers la racine)
 *
 * @param b1 la première branche
 * @param b2 la seconde branche
 *
 * @return 1 si les branches sont identiques, 0 sinon
 */
int branch_compare(branch *b1, branch *b2) {
  branch *n1 = b1, *n2 = b2;

  if (!b1 && !b2)
    return 1;
  if (!b1 || !b2)
    return 0;

  do {
    if (strcmp(n1->nom, n2->nom) != 0) {
      return 0;
    }

    n1 = n1->parent;
    n2 = n2->parent;
  } while (n1 && n2);

  return !n1 && !n2;
}
