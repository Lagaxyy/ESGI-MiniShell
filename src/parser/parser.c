#include <stdio.h>
#include <stdlib.h>

typedef struct {

    char * data;
    int * next;

} Node;

int buffer_allocation(void){

    char *buffer = malloc(64);
    if (buffer != NULL){
        printf("Buffer allocation OK\n");
        printf("%p\n", &buffer);
    }

    printf(">>> ");
    fgets(buffer, 64, stdin);
    printf("%s\n", buffer);

    free(buffer);

}

int add_link(){



}

int remove_link(){



}

int main(void){

    buffer_allocation();

}