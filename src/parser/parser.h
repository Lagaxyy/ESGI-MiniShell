#ifndef PARSER_H
#define PARSER_H

struct Node {
  char *data;
  struct Node *next;
};
typedef struct Node Node;

Node *parser();
void parser_free();

#endif /* PARSER_H */
