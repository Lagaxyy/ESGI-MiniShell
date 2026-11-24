#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "tree/tree.h"

int minishell_mission(Shell *shell, int argc, char **argv) {
  int active;
  branch *b, *bs;
  FILE *f = NULL;
  char *path = NULL, *m = NULL, *tree = NULL, *message = NULL, *monde = NULL,
       *mission = NULL, *start = NULL;
  char *file_suivi = "missions/suivi.mission";
  int rcode = EXIT_SUCCESS;

  /* Checks de début de fonction */
  if (argc != 1) {
    fprintf(stderr, "mission: mauvais nombre d'arguments\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  if ((f = fopen(file_suivi, "r")) == NULL) {
    fprintf(stderr,
            "mission: erreur lors de l'ouverture du fichier \"%s\" en mode "
            "lecture\n",
            file_suivi);
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }

  /* Récupère les informations du fichier de suivi */
  if (fscanf(f, "active %d \\n", &active) != 1) {
    fprintf(stderr, "mission: erreur lors de la lecture du suivi (active)\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  if (fscanf(f, "suivi %ms %ms", &monde, &mission) != 2) {
    fprintf(stderr, "mission: erreur lors de la lecture du suivi (suivi)\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  fclose(f);
  f = NULL;

  /* Construis les chemins vers les fichiers */
  if ((path = (char *)calloc(256, sizeof(char))) == NULL) {
    fprintf(stderr, "mission: erreur allocation path\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  if ((m = (char *)calloc(256, sizeof(char))) == NULL) {
    fprintf(stderr, "mission: erreur allocation m\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  if ((tree = (char *)calloc(256, sizeof(char))) == NULL) {
    fprintf(stderr, "mission: erreur allocation tree\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  if ((message = (char *)calloc(256, sizeof(char))) == NULL) {
    fprintf(stderr, "mission: erreur allocation message\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  strcat(path, "missions/");
  strcat(path, monde);
  strcat(path, "/");
  strcat(path, mission);
  strcat(path, "/");
  strcat(path, mission);
  strcat(m, path);
  strcat(m, ".mission");
  strcat(tree, path);
  strcat(tree, ".starter.tree");
  strcat(message, path);
  strcat(message, ".txt");

  /* Récupère l'arborescence de départ de la mission */
  if ((f = fopen(m, "r")) == NULL) {
    fprintf(stderr,
            "mission: erreur lors de l'ouverture du fichier \"%s\" en mode "
            "lecture\n",
            m);
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  if (fscanf(f, "start %ms \\n", &start) != 1) {
    fprintf(stderr,
            "mission: erreur lors de la lecture de la mission (start)\n");
    rcode = EXIT_FAILURE;
    goto minishell_mission_end;
  }
  fclose(f);
  f = NULL;

  if (!strcmp(argv[0], "start")) {
    /* Sauvegarde l'arborescence actuelle dans le fichier de base */
    if (!active) {
      if (!branch_save(shell->root, FILE_BASE_TREE)) {
        fprintf(
            stderr,
            "mission: erreur lors de la sauvegarde de l'arborescence dans le "
            "fichier \"%s\"\n",
            FILE_BASE_TREE);
        rcode = EXIT_FAILURE;
        goto minishell_mission_end;
      }
    }

    /* Charge l'arborescence de départ */
    if ((b = branch_load(tree)) == NULL) {
      fprintf(stderr,
              "mission: erreur lors du chargement de l'arborescence dans le "
              "fichier \"%s\"\n",
              tree);
      rcode = EXIT_FAILURE;
      goto minishell_mission_end;
    }
    branch_free(shell->root);
    shell->root = b;
    shell->cwd = ((bs = branch_find(b, start))) ? bs : b;

    /* Mise à jour du fichier de suivi */
    FILE *ff;
    if ((ff = fopen(file_suivi, "w")) == NULL) {
      fprintf(stderr,
              "mission: erreur lors de l'ouverture du fichier \"%s\" en mode "
              "écriture\n",
              file_suivi);
      rcode = EXIT_FAILURE;
      goto minishell_mission_end;
    }
    fprintf(ff, "active 1\nsuivi %s %s", monde, mission);
    fclose(ff);
    ff = NULL;

    /* Affichage du message de la mission */
    char c;
    if ((ff = fopen(message, "r")) == NULL) {
      fprintf(stderr,
              "mission, erreur lors de l'ouverture du fichier \"%s\" en mode "
              "lecture\n",
              message);
      rcode = EXIT_FAILURE;
      goto minishell_mission_end;
    }
    while ((c = fgetc(ff)) != EOF)
      putchar(c);
    fclose(ff);
  } else if (!strcmp(argv[0], "quit")) {
    /* Charge l'arborescence du fichier de base */
    if ((b = branch_load(FILE_BASE_TREE)) == NULL) {
      fprintf(stderr,
              "mission: erreur lors du chargement de l'arborescence dans le "
              "fichier \"%s\"\n",
              FILE_BASE_TREE);
      rcode = EXIT_FAILURE;
      goto minishell_mission_end;
    }
    branch_free(shell->root);
    shell->root = b;
    shell->cwd = b;

    /* Mise à jour du fichier de suivi */
    FILE *ff;
    if ((ff = fopen(file_suivi, "w")) == NULL) {
      fprintf(stderr,
              "mission: erreur lors de l'ouverture du fichier \"%s\" en mode "
              "écriture\n",
              file_suivi);
      rcode = EXIT_FAILURE;
      goto minishell_mission_end;
    }
    fprintf(ff, "active 0\nsuivi %s %s", monde, mission);
    fclose(ff);
  } else {
    fprintf(stderr, "mission: argument inconnu\n");
    rcode = EXIT_FAILURE;
  }

minishell_mission_end:
  if (f)
    fclose(f);
  free(path);
  free(m);
  free(tree);
  free(message);
  free(monde);
  free(mission);
  free(start);

  return rcode;
}
