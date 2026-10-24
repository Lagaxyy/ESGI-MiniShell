#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "parser_internal.h"

struct Node {

  char *data;
  int *next;
};

char *buffer_allocation(void) {

  char *buffer = malloc(64);
  if (buffer != NULL) {
    printf("Buffer allocation OK\n");
    printf("%p\n", &buffer);
  }

  printf(">>> ");
  fgets(buffer, 64, stdin);
  printf("%s\n", buffer);

  free(buffer);
}

int add_link() {}

int remove_link() {}

Node *parser() {}

void parser_free() {}

