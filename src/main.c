#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
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
 * Fonction annexe à minishell_save.
 *
 * @param b le noeud à écrire
 * @param f le stream du fichier
 * @param depth la profondeur du noeud
 *
 * @return 1 si la fonction s'est bien déroulée, 0 sinon
 */
int minishell_save_recursive(branch *b, FILE *f, int depth) {
  int i;
  branch *br;
  int rcode = 1;

  if (!b) {
    return 1;
  }

  for (i = 0; i < depth; i++) {
    fputc('\t', f);
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
    rcode = rcode && minishell_save_recursive(br, f, depth + 1);
  }
  for (i = 0; i < depth; i++) {
    fputc('\t', f);
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
int minishell_save(branch *b, const char *path) {
  FILE *f;

  if ((f = fopen(path, "w")) == NULL) {
    fprintf(stderr,
            "branch_save: erreur lors de l'ouverture du fichier \"%s\"\n",
            path);
    return 0;
  }

  if (!minishell_save_recursive(b, f, 0)) {
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
 * Fonction annexe à minishell_load.
 *
 * @param tokens pointe vers les tokens à lire
 *
 * @return le noeud correspondant
 */
branch *minishell_load_recursive(Node **tokens) {
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
            "minishell_load_recursive: erreur lecture type du noeud \"%s\"\n",
            cur->data);
    return NULL;
  }
  cur = cur->next;
  if (!cur) {
    fprintf(stderr, "minishell_load_recursive: erreur lecture nom du noeudd\n");
    return NULL;
  }
  name = cur->data;

  if ((new_b = branch_create(name, type)) == NULL) {
    fprintf(stderr, "minishell_load_recursive: erreur création noeud\n");
    return NULL;
  }

  cur = cur->next;
  if (cur && !strcmp(cur->data, "[")) {
    cur = cur->next;

    while (cur && strcmp(cur->data, "]") != 0) {
      branch *b, *new_child;

      if ((new_child = minishell_load_recursive(&cur)) == NULL) {
        fprintf(stderr,
                "minishell_load_recursive: erreur création noeud fils\n");
        branch_free(new_b);
        return NULL;
      }

      if (!new_b->first_child) {
        new_b->first_child = new_child;
      } else {
        for (b = new_b->first_child; b->next_sibling; b = b->next_sibling)
          ;
        b->next_sibling = new_child;
      }
    }

    if (strcmp(cur->data, "]") != 0) {
      fprintf(stderr, "minishell_load_recursive: erreur crochet manquant\n");
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
branch *minishell_load(const char *path) {
  branch *b = NULL;
  char c, *buf;
  FILE *f;
  Node *tokens, *cur;
  int i = 0, cap = 64;

  if ((buf = (char *)malloc(sizeof(char) * cap)) == NULL) {
    fprintf(stderr, "minishell_load: erreur allocation buf\n");
    return NULL;
  }
  if ((f = fopen(path, "r")) == NULL) {
    fprintf(stderr,
            "minishell_load: erreur lors de l'ouverture du fichier \"%s\"\n",
            path);
    return NULL;
  }

  while ((c = fgetc(f)) != EOF) {
    if (i >= cap) {
      char *temp;

      cap *= 2;
      if ((temp = (char *)realloc(buf, sizeof(char) * cap)) == NULL) {
        fprintf(stderr, "minishell_load: erreur reallocation buf\n");
        goto minishell_load_end;
      }
      buf = temp;
    }

    buf[i++] = c;
  }
  buf[i] = '\0';

  tokens = lexer(buf, NULL);
  cur = tokens;
  b = minishell_load_recursive(&cur);

minishell_load_end:
  lexer_free(&tokens);
  free(buf);
  fclose(f);
  return b;
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
  Node *tokens;
  Shell *shell;
  branch *root;
  char *starter = "tests/test.minish";

  exit_code = 0;
  is_debug = argc >= 2 && !strcmp(argv[1], "--debug");

  /* TEST: Construction d'un arbre */
  if ((root = minishell_load(starter)) == NULL) {
    fprintf(stderr, "minishell: erreur chargement arborescence depuis \"%s\"\n",
            starter);
    return 1;
  }
  if ((shell = (Shell *)malloc(sizeof(Shell))) == NULL) {
    fprintf(stderr, "minishell: erreur allocation shell\n");
    return 1;
  }
  shell->root = root;
  shell->cwd = root;
  if (is_debug) {
    printf("Arborescence chargée depuis \"%s\":\n", starter);
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
    for (Node *n = tokens->next; n != NULL; n = n->next)
      arg_count++;

    char **args = NULL;
    if (arg_count > 0) {
      args = malloc((size_t)(arg_count + 1) * sizeof(*args));
      if (args == NULL) {
        lexer_free(&tokens);
        continue;
      }

      int k = 0;
      for (Node *n = tokens->next; n != NULL; n = n->next)
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
