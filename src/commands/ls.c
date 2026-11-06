#include <stdio.h>
#include <string.h>

#include "commands.h"

void print_children_ls(branch *b){
    branch * fs = b->first_child;

    printf("%s", (char *)fs);
    while(fs != NULL){
        fs = fs->next_sibling;
        if(fs != NULL){
            printf("    %s", (char *)fs);
        }
        else{
            break;
        }
        
    }
    printf("\n");
    return;
}

void print_children_ls_l(branch *b){

    int number_of_files;
    int i=0;
    branch * fs = b->first_child;

    while(fs != NULL){
        fs = fs->next_sibling;
        i++;
    }

    number_of_files = i;
    printf("total %d\n", number_of_files);

    fs = b->first_child;

    printf("%s", (char *)fs);

    while(fs != NULL){
        fs = fs->next_sibling;

        if(fs != NULL){
            printf("    %s", (char *)fs);
        }
    }
    
    printf("\n");
    return;
}

int minishell_ls(Shell *shell, int argc, char **argv){

    if(argc > 0){

        if(strcmp((char *)argv, "-l") == 0){
            print_children_ls_l(shell->cwd);
            return 0;
        }
    }

    print_children_ls(shell->cwd);
    return 0;
}

