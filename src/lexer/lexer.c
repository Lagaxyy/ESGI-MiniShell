#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "lexer_internal.h"

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

int is_quote(char c) { return c == '\'' || c == '"'; }

char *extract_token(char *buffer, int *cursor) {
  char c;
  char *token;
  int isQuote = 0;
  int s = *cursor;
  size_t i = 0;
  size_t capacity = 16;

  if ((token = (char *)malloc(sizeof(char) * capacity)) == NULL) {
    fprintf(stderr, "extract_token: erreur allocation token");
    return NULL;
  }

  if (is_quote(buffer[s])) {
    s++;
    isQuote = 1;
  }

  c = buffer[s + i];
  while (c != '\0' && ((isQuote) ? !is_quote(c) : !isspace(c))) {
    if (i + 1 >= capacity) {
      char *temp;

      capacity *= 2;
      if ((temp = realloc(token, sizeof(char) * capacity)) == NULL) {
        fprintf(stderr, "extract_token: erreur reallocation token");
        free(token);
        return NULL;
      }
      token = temp;
    }

    token[i++] = c;
    c = buffer[s + i];
  }
  token[i] = '\0';
  if (isQuote) {
    if (is_quote(c)) {
      i++;
    } else {
      free(token);
      return NULL;
    }
  }

  *cursor = s + i;
  return token;
}

Node *lexer() {
  char c;
  Node *n = NULL;
  int i = 0;

  char *input = buffer_allocation();
  if (!input)
    return NULL;

  while ((c = input[i]) != '\0') {
    char *token;

    if (isspace(c)) {
      i++;
      continue;
    }

    if ((token = extract_token(input, &i)) == NULL) {
      break;
    }
    n = Node_add(n, token);
    free(token);
  }

  free(input);
  return n;
}

void lexer_free(Node **n) {
  Node *node = *n;

  if (!node)
    return;

  Node_free(node);
  *n = NULL;
}
