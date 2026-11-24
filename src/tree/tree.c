#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
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

/**
 * Fonction annexe à branch_save.
 *
 * @param b le noeud à écrire
 * @param f le stream du fichier
 * @param depth la profondeur du noeud
 *
 * @return 1 si la fonction s'est bien déroulée, 0 sinon
 */
int branch_save_recursive(branch *b, FILE *f, int depth) {
  int i;
  branch *br;
  int rcode = 1;

  if (!b) {
    return 1;
  }

  for (i = 0; i < depth; i++) {
    fprintf(f, "  ");
  }

  if (b->type == Folder) {
    fprintf(f, "FOLDER");
  }
  if (b->type == File) {
    fprintf(f, "FILE");
  }
  fprintf(f, " \"%s\"", b->nom);

  if (!b->first_child) {
    fputc('\n', f);
    return 1;
  }

  fprintf(f, " [\n");
  for (br = b->first_child; br; br = br->next_sibling) {
    rcode = rcode && branch_save_recursive(br, f, depth + 1);
  }
  for (i = 0; i < depth; i++) {
    fprintf(f, "  ");
  }
  fprintf(f, "]\n");

  return rcode;
}

/**
 * Écrit l'arborescence spécifiée dans le fichier au chemin donné.
 *
 * @param b l'arborescence à écrire
 * @param path le chemin du fichier
 *
 * @return 1 si la fonction s'est bien déroulée, 0 sinon
 */
int branch_save(branch *b, const char *path) {
  FILE *f;

  if ((f = fopen(path, "w")) == NULL) {
    fprintf(stderr,
            "branch_save: erreur lors de l'ouverture du fichier \"%s\"\n",
            path);
    return 0;
  }

  if (!branch_save_recursive(b, f, 0)) {
    fprintf(stderr,
            "branch_save: erreur lors de l'écriture de l'arborescence dans le "
            "fichier \"%s\"\n",
            path);
    return 0;
  }

  fclose(f);
  return 1;
}

/**
 * Fonction annexe à branch_load.
 *
 * @param tokens pointe vers les tokens à lire
 *
 * @return le noeud correspondant
 */
branch *branch_load_recursive(Node **tokens) {
  branch *new_b;
  branch_type type;
  char *name;
  Node *cur = *tokens;

  if (!strcmp("FOLDER", cur->data))
    type = Folder;
  else if (!strcmp("FILE", cur->data))
    type = File;
  else {
    fprintf(stderr,
            "branch_load_recursive: erreur lecture type du noeud \"%s\"\n",
            cur->data);
    return NULL;
  }
  cur = cur->next;
  if (!cur) {
    fprintf(stderr, "branch_load_recursive: erreur lecture nom du noeudd\n");
    return NULL;
  }
  name = cur->data;

  if ((new_b = branch_create(name, type)) == NULL) {
    fprintf(stderr, "branch_load_recursive: erreur création noeud\n");
    return NULL;
  }

  cur = cur->next;
  if (cur && !strcmp(cur->data, "[")) {
    cur = cur->next;

    while (cur && strcmp(cur->data, "]") != 0) {
      branch *new_child;

      if ((new_child = branch_load_recursive(&cur)) == NULL) {
        fprintf(stderr, "branch_load_recursive: erreur création noeud fils\n");
        branch_free(new_b);
        return NULL;
      }

      if (!branch_add_child(new_b, new_child)) {
        fprintf(stderr, "branch_load_recursive: erreur ajout noeud enfant\n");
        branch_free(new_child);
        branch_free(new_b);
        return NULL;
      }
    }

    if (strcmp(cur->data, "]") != 0) {
      fprintf(stderr, "branch_load_recursive: erreur crochet manquant\n");
      branch_free(new_b);
      return NULL;
    }
    cur = cur->next;
  }

  *tokens = cur;
  return new_b;
}

/**
 * Construit une arborescence depuis le contenu du fichier spécifié.
 *
 * @param path le chemin de la sauvegarde depuis la racine du projet
 *
 * @return l'arborescence construite
 */
branch *branch_load(const char *path) {
  branch *b = NULL;
  char c, *buf;
  FILE *f;
  Node *tokens, *cur;
  int i = 0, cap = 64;

  if ((buf = (char *)malloc(sizeof(char) * cap)) == NULL) {
    fprintf(stderr, "branch_load: erreur allocation buf\n");
    return NULL;
  }
  if ((f = fopen(path, "r")) == NULL) {
    fprintf(stderr,
            "branch_load: erreur lors de l'ouverture du fichier \"%s\"\n",
            path);
    return NULL;
  }

  while ((c = fgetc(f)) != EOF) {
    if (i >= cap) {
      char *temp;

      cap *= 2;
      if ((temp = (char *)realloc(buf, sizeof(char) * cap)) == NULL) {
        fprintf(stderr, "branch_load: erreur reallocation buf\n");
        goto branch_load_end;
      }
      buf = temp;
    }

    buf[i++] = c;
  }
  buf[i] = '\0';

  tokens = lexer(buf, NULL);
  cur = tokens;
  b = branch_load_recursive(&cur);

branch_load_end:
  lexer_free(&tokens);
  free(buf);
  fclose(f);
  return b;
}
