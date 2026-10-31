#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"

int main(void) {
  Node *node, *n;
  int i;

  while (1) {
    printf("minish $");
    node = lexer();

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

    lexer_free(&n);
  }

  return EXIT_SUCCESS;
}
