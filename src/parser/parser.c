#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "parser_internal.h"

#define CHUNK 128

char *buffer_allocation(void) {
  char read;
  char *buffer;
  size_t cap = CHUNK;
  size_t len = 0;

  if ((buffer = (char *)malloc(sizeof(char) * cap)) == NULL) {
    fprintf(stderr, "buffer_allocation: erreur allocation buffer");
    return NULL;
  }

  while ((read = getchar()) != EOF) {
    if (len + 1 >= cap) {
      char *temp;

      cap *= 2;
      if ((temp = (char *)realloc(buffer, sizeof(char) * cap)) == NULL) {
        free(buffer);
        fprintf(stderr, "buffer_allocation: erreur reallocation buffer");
        return NULL;
      }
      buffer = temp;
    }

    buffer[len++] = read;

    if (read == '\n')
      break;
  }
  buffer[len] = '\0';

  if (len == 0) {
    free(buffer);
    return NULL;
  }

  return buffer;
}

Node *Node_add(Node *n, const char *s) {
  if (!s) {
    return n;
  }

  Node *new_node, *temp;
  size_t len = strlen(s) + 1;

  if ((new_node = (Node *)malloc(sizeof(Node))) == NULL) {
    fprintf(stderr, "Node_add: erreur allocation new_node");
    return n;
  }
  if ((new_node->data = (char *)malloc(sizeof(char) * len)) == NULL) {
    fprintf(stderr, "Node_add: erreur allocation new_node->data");
    return n;
  }
  new_node->data = memcpy(new_node->data, s, len);
  new_node->next = NULL;

  if (!n) {
    return new_node;
  }

  for (temp = n; temp->next; temp = temp->next)
    ;
  temp->next = new_node;

  return n;
}

void Node_free(Node *n) {
  if (!n) {
    return;
  }

  Node_free(n->next);
  free(n->data);
  free(n);
}

Node *parser() {}

void parser_free() {}
