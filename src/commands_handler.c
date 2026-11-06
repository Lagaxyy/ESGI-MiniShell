#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "commands.h"

static const Command commands[] = {
    {"pwd", "Affiche le chemin absolu du répertoire courant", minishell_pwd,
     "pwd"},
    {"help", "Montre la liste des commandes", command_help, "help"},
    {"ls", "Affiche le contenu d'un répertoire", minishell_ls, "ls"},
    {NULL, NULL, NULL, NULL}
};

const Command *command_find(const char *name) {
  size_t i = 0;
  Command command = commands[0];

  while (command.name != NULL) {
    if (!strcmp(command.name, name)) {
      return commands + i;
    }

    command = commands[++i];
  }

  return NULL;
}

int command_help(Shell *shell, int argc, char **argv) {
  size_t i = 0;
  Command command = commands[0];

  printf("Liste des commandes:\n");
  while (command.name != NULL) {
    printf("\t%s: %s\n", command.name, command.description);
    printf("\t=> %s\n\n", command.usage);

    command = commands[++i];
  }
  printf(
      "\t[exit|quit|bye]: Ferme le minishell et renvoie le code de statut\n");
  printf("\t => [exit|quit|bye] <statut>\n\n");

  return 0;
}
