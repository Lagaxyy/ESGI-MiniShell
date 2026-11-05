#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

struct Node;
typedef struct Node Node;

char *buffer_allocation();
Node *Node_add(Node *n, const char *s);
void Node_free(Node *n);

#endif /* LEXER_INTERNAL_H */
