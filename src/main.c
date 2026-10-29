#include <stdio.h>
#include <stdlib.h>

#include "parser/parser.h"

int main(void) {
  Node *node, *n;
  int i;

  while (1) {
    printf("minish $");
    node = parser();

    i = 0;

    printf("\n>> ");
    for (n = node; n; n = n->next) {
      if (i)
        printf(" -> ");
      printf("(%s)", n->data);
      i++;
    }
    putchar('\n');
    putchar('\n');

    parser_free(&n);
  }

  return EXIT_SUCCESS;
}
