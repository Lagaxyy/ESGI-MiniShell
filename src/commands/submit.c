#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "tree/tree.h"

/**
 * Effectue un parcours en profondeur de l'arborescence pour comparer les
 * feuilles avec celles de l'arborescence du shell
 *
 * @param t l'arborescence avec les éléments de comparaison
 * @param shell l'objet shell
 *
 * @return 1 si la comparaison est validée, 0 sinon
 */
int find_leaves_and_compare(branch *t, Shell *shell) {
  branch *b;

  if (!t)
    return 1;

  for (b = t->first_child; b; b = b->next_sibling) {
    /* NOTE: comparer deux Folder ne tient pas en compte leurs noeuds enfants */
    if (b->type == File || (b->type == Folder && !b->first_child)) {
      branch *found = branch_find(shell->root, b->nom);

      if (!found || !branch_compare(found, b))
        return 0;

      continue;
    }

    if (!find_leaves_and_compare(b, shell))
      return 0;
  }

  return 1;
}

int minishell_submit(Shell *shell, int argc, char **argv) {
  int active;
  FILE *f = NULL;
  char *path = NULL, *buf = NULL, *monde = NULL, *mission = NULL, *start = NULL,
       *expect = NULL, *tree = NULL, *next = NULL;
  char *file_suivi = "missions/suivi.mission";
  int rcode = EXIT_SUCCESS;
  int success = 1;

  /* Récupère les informations du fichier de suivi */
  if ((f = fopen(file_suivi, "r")) == NULL) {
    fprintf(stderr, "submit: erreur lors de l'ouverture du fichier \"%s\"\n",
            file_suivi);
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  if (fscanf(f, "active %d \\n", &active) != 1) {
    fprintf(stderr, "submit: erreur lors de la lecture du suivi (active)\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  if (!active) {
    printf("Mode mission désactivé.\n");
    goto minishell_submit_end;
  }
  if (fscanf(f, "suivi %ms %ms", &monde, &mission) != 2) {
    fprintf(stderr, "submit: erreur lors de la lecture du suivi (suivi)\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  fclose(f);
  f = NULL;

  /* Construis les chemins vers les fichiers */
  if ((path = (char *)calloc(256, sizeof(char))) == NULL) {
    fprintf(stderr, "submit: erreur allocation path\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  if ((buf = (char *)calloc(256, sizeof(char))) == NULL) {
    fprintf(stderr, "submit: erreur allocation buf\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  strcat(path, "missions/");
  strcat(path, monde);
  strcat(path, "/");
  strcat(path, mission);
  strcat(path, "/");
  strcat(buf, path);
  strcat(buf, mission);
  strcat(buf, ".mission");

  /* Récupère les informations du fichier de mission */
  if ((f = fopen(buf, "r")) == NULL) {
    fprintf(stderr, "submit: erreur lors de l'ouverture du fichier \"%s\"\n",
            buf);
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  if (fscanf(f, "start %ms \\n", &start) != 1) {
    fprintf(stderr,
            "submit: erreur lors de la lecture de la mission (start)\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  if (fscanf(f, "expect %ms \\n", &expect) != 1) {
    fprintf(stderr,
            "submit: erreur lors de la lecture de la mission (expect)\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  if (fscanf(f, "tree %ms \\n", &tree) != 1) {
    fprintf(stderr, "submit: erreur lors de la lecture de la mission (tree)\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }
  if (fscanf(f, "next %ms \\n", &next) != 1) {
    fprintf(stderr, "submit: erreur lors de la lecture de la mission (next)\n");
    rcode = EXIT_FAILURE;
    goto minishell_submit_end;
  }

  /* Vérifie si l'état actuel remplit les conditions pour valider la mission */
  if (strcmp(expect, "none") != 0) {
    if (argc < 1 || strcmp(argv[0], expect) != 0)
      success = 0;
  }
  if (strcmp(tree, "none") != 0) {
    char *buffer;
    branch *t;

    if ((buffer = (char *)calloc(256, sizeof(char))) == NULL) {
      fprintf(stderr, "submit: erreur allocation buffer\n");
      rcode = EXIT_FAILURE;
      goto minishell_submit_end;
    }
    strcat(buffer, path);
    strcat(buffer, tree);

    t = branch_load(buffer);
    if (t->first_child && !find_leaves_and_compare(t, shell))
      success = 0;

    free(buffer);
    branch_free(t);
  }

  /* Valide si les conditions sont remplies */
  if (!success)
    printf("Mission non validée.\n");
  else {
    FILE *ff;

    printf("Mission validée ! Entrez \"mission start\" pour passer à la "
           "prochaine mission.\n");

    if ((ff = fopen(file_suivi, "w")) == NULL) {
      fprintf(stderr,
              "submit: erreur lors de l'ouverture du fichier \"%s\" (fin)\n",
              file_suivi);
      rcode = EXIT_FAILURE;
      goto minishell_submit_end;
    }
    fprintf(ff, "active 1\nsuivi %s %s\n", monde, next);
    fclose(ff);
  }

minishell_submit_end:
  if (f)
    fclose(f);
  free(monde);
  free(mission);
  free(start);
  free(expect);
  free(tree);
  free(next);
  free(buf);
  free(path);

  return rcode;
}
