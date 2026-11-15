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

/*     NE FONCTIONNE PAS : tout finis dans n2 automatiquement
 */    
    Node *tokens = lexer(argv, "/");

    Node *t = NULL;
    Node *last_t = NULL;

    branch *path = b;

    for (t = tokens; t != NULL; t = t->next)
    {
        last_t = t;
    }

    for (t = tokens; t != NULL; t = t->next)
    {

        if (strcmp(t->data, path->nom) == 0)
        {
            path = path->first_child;
        }
        else
        {
            while (path->type != Folder)
            {
                path->first_child = path->next_sibling;
            }
            branch *nb = branch_create(last_t->data, File);
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