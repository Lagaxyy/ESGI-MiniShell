#ifndef LEXER_H
#define LEXER_H

struct Node {
  char *data;
  struct Node *next;
};
typedef struct Node Node;

Node *lexer();
void lexer_free(Node **n);

#endif /* LEXER_H */
