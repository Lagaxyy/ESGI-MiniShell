#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "commands.h"

void touch_cur(char *argv, branch *b)
{

    branch *nb = branch_create(argv, File);
    branch_add_child(b, nb);
}

void touch_root(char *argv, branch *b)
{

/* NE FONCTIONNE PAS */

    Node *tokens = lexer(argv, "/");

    Node *t = NULL;

    branch *path = b;

    Node *prev = NULL;
    Node *last = NULL;

    for (t = tokens; t != NULL; t = t->next)
    {
        prev = last;
        last = t;
    }

    for (t = tokens; t != NULL; t = t->next)
    {

        while (strcmp(path->nom, t->data) != 0)
        {
            path = path->first_child;
            if (path == NULL)
            {
                printf("Folder doesn't exist\n");
            }
        }
        while (strcmp(path->nom, t->data) != 0)
        {
            path = path->next_sibling;
            if (path == NULL)
            {
                printf("Folder doesn't exist\n");
            }
        }
        if (strcmp(prev->data, t->data) == 0)
        {
            branch *nb = branch_create(last->data, File);
            branch_add_child(path->first_child, nb);
            break;
        }
    }
}

int minishell_touch(Shell *shell, int argc, char **argv)
{
    if (argv == NULL)
    {
        printf("touch: opérande de fichier manquant\n");
        printf("Saisissez \"help\" pour plus d'informations.\n");
        return -1;
    }
    else if (argc == 1 && *argv[0] != '/')
    {
        printf("%d\n", argc);
        touch_cur(argv[0], shell->cwd);
        return 0;
    }
    else if (*argv[0] == '/')
    {
        touch_root(argv[0], shell->root);
    }

    return 0;
}