#include <stdio.h>
#include <string.h>

#include "commands.h"

void print_children_ls(branch *b){
    printf("%s\n", (char *)b->first_child);
    return;
}

void print_children_l_ls(branch *b){

    int number_of_files;

    for(int i=0; (char *)(b->first_child) == NULL; i++){
        number_of_files = i;
    }

    printf("total %d\n", number_of_files);
    printf("%s\n", (char *)b->first_child);
    return;
}

int minishell_ls(Shell *shell, int argc, char **argv){

/*   for(int i=0; i<argc; i++){
        printf("arg : %s\n", (char*)argv);
    } */
    if((char *)argv != NULL){
        if(strcmp((char *)argv, "-l") == 0){
            print_children_l_ls(shell->cwd);
            return 0;
        }
    }
/*     else{
        printf("no argument\n");
    } */

    print_children_ls(shell->cwd);
    return 0;
}

