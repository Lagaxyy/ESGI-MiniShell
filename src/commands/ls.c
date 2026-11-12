#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "commands.h"

void print_children_ls(branch *b)
{
  branch *fs = b->first_child;

  if (fs->type == Folder)
  {
    printf("\033[1;34m%s\033[0m", (char *)fs);
  }
  else if (fs->type == File)
  {
    printf("%s", (char *)fs);
  }

  while (fs != NULL)
  {
    fs = fs->next_sibling;
    if (fs != NULL)
    {

      if (fs->type == Folder)
      {
        printf("    \033[1;34m%s\033[0m", fs->nom);
      }
      else if (fs->type == File)
      {
        printf("    %s", fs->nom);
      }
    }
    else
    {
      break;
    }
  }

  printf("\n");
  return;
}

void print_children_ls_l(branch *b)
{

  int number_of_files;
  int i = 0;
  branch *fs = b->first_child;

  while (fs != NULL)
  {
    fs = fs->next_sibling;
    i++;
  }

  number_of_files = i;
  printf("total %d\n", number_of_files);

  fs = b->first_child;

  if (fs->type == Folder)
  {
    printf("\033[1;34m%s\033[0m", fs->nom);
  }
  else if (fs->type == File)
  {
    printf("%s", fs->nom);
  }

  while (fs != NULL)
  {
    fs = fs->next_sibling;

    if (fs != NULL)
    {
      if (fs->type == Folder)
      {
        printf("    \033[1;34m%s\033[0m", fs->nom);
      }
      else if (fs->type == File)
      {
        printf("    %s", fs->nom);
      }
    }
  }

  printf("\n");
  return;
}

int print_directory_ls(char *argv, branch *b)
{

  Node *tokens = lexer(argv, "/");
  branch *cur = b;
  int f_count = 0;

  for (Node *t = tokens; t != NULL; t = tokens->next)
  {

    branch *fs = cur->first_child;

    while (fs != NULL && strcmp(fs->nom, t->data))
    {
      fs = fs->next_sibling;
    }
    if (fs == NULL)
    {
      printf("ls : impossible d'accéder à \"%s\" : Aucun fichier ou dossier de ce type\n", argv);
      free(tokens);
      return 127;
    }
    cur = fs;
  }

  for (branch *fs = cur->first_child; fs != NULL; fs = fs->next_sibling)
  {
    if (fs->type == Folder)
    {
      if (f_count == 0)
      {
        printf("\033[1;34m%s\033[0m", fs->nom);
        f_count++;
      }
      else
      {
        printf("  \033[1;34m%s\033[0m", fs->nom);
        f_count++;
      }
    }
    else
    {
      if (f_count == 0)
      {
        printf("%s", fs->nom);
        f_count++;
      }
      else
      {
        printf("  %s", fs->nom);
        f_count++;
      }
    }
  }
  printf("\n");

  free(tokens);
  return 0;
}

int minishell_ls(Shell *shell, int argc, char **argv)
{

  if (argc > 0)
  {

    for (int i = 0; i < argc; i++)
    {
      if (strcmp(argv[i], "-l") == 0)
      {
        print_children_ls_l(shell->cwd);
        return 0;
      }
      else if (strstr(argv[i], "/") != NULL)
      {
        print_directory_ls(argv[i], shell->root);
        /*      Pourquoi un "ls /" donne une erreur ? (probleme avec le command handler)"
         */
      }
      else
      {
        printf("Argument inconnu\n");
        return 127;
      }
    }
  }
  if (argc == 0)
  {
    print_children_ls(shell->cwd);
  }

  return 0;
}
