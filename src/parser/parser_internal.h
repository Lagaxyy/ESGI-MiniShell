#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

struct Node;
typedef struct Node Node;

char *buffer_allocation();
Node *Node_add(Node *n, const char *s);
void Node_free(Node *n);

#endif /* PARSER_INTERNAL_H */
