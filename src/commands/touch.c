#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "commands.h"

void touch_cur(char *argv, branch *b)
{

    branch *nb = branch_create(argv, File);
    branch_add_child(b, nb);
}

int touch_root(char *argv, branch *b)
{

    Node *tokens = lexer(argv, "/");
    branch *cur = b;

    Node *t = NULL;

    Node *prev = NULL;
    Node *last = NULL;

    for (t = tokens; t != NULL; t = t->next)
    {
        prev = last;
        last = t;
    }

    for (Node *t = tokens; t != prev; t = t->next)
    {

        branch *fs = cur->first_child;

        while (fs != NULL && strcmp(fs->nom, t->data) != 0)
        {
            fs = fs->next_sibling;
        }
        if (fs == NULL)
        {
            printf("ls : impossible d'accéder à \"%s\" : Aucun dossier de ce type\n", argv);
            free(tokens);
            return 127;
        }
        cur = fs;
    }

    for (branch *fs = cur->first_child; fs != NULL; fs = fs->next_sibling)
    {
        if (fs == NULL)
        {
            printf("ls : impossible d'accéder à \"%s\" : Aucun dossier de ce type\n", argv);
            free(tokens);
            return 127;
        }
        if (fs->type == Folder && strcmp(prev->data, fs->nom) == 0)
        {
            branch *nb = branch_create(last->data, File);
            branch_add_child(fs, nb);
            printf("created\n");
            break;
        }
    }
    printf("\n");

    free(tokens);
    return 0;
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