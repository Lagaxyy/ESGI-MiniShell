#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdio.h>

#include "parser/parser.h"
#include "parser/parser_internal.h"

void Node_print(Node *node) {
  Node *n;
  int i = 0;

  for (n = node; n; n = n->next) {
    if (i)
      printf(" -> ");
    printf("(%s)", n->data);
    i++;
  }
  putchar('\n');
}

Test(buffer_allocation, basique) {
  FILE *f;
  char *expected = "hello world\n";
  char *filename = "build/buffer_allocation.basique.criterion.test";

  cr_assert_not_null(f = fopen(filename, "w"));

  fprintf(f, expected);
  fclose(f);

  freopen(filename, "r", stdin);

  char *got = buffer_allocation();

  cr_expect_str_eq(got, expected, "Got: %s, Expected: %s\n", got, expected);

  free(got);
}

Test(buffer_allocation, newline_seulement) {
  FILE *f;
  char *expected = "\n";
  char *filename = "build/buffer_allocation.newline_seulement.criterion.test";

  cr_assert_not_null(f = fopen(filename, "w"));

  fprintf(f, expected);
  fclose(f);

  freopen(filename, "r", stdin);

  char *got = buffer_allocation();

  cr_expect_str_eq(got, expected, "Got: %s, Expected: %s\n", got, expected);

  free(got);
}

Test(buffer_allocation, input_vide) {
  FILE *f;
  char *expected = "";
  char *filename = "build/buffer_allocation.input_vide.criterion.test";

  cr_assert_not_null(f = fopen(filename, "w"));

  fprintf(f, expected);
  fclose(f);

  freopen(filename, "r", stdin);

  char *got = buffer_allocation();

  cr_expect_null(got, "Got: %s, Expected: %s\n", got, NULL);

  free(got);
}

Test(Node, basique) {
  int i;
  Node *n;
  Node *node = NULL;
  char *data[] = {"hello", "world", "42"};

  for (i = 0; i < 3; i++) {
    node = Node_add(node, data[i]);
  }

  i = 0;
  for (n = node; n; n = n->next) {
    cr_expect_str_eq(n->data, data[i], "Got: %s, Expected: %s\n", n->data,
                     data[i]);
    i++;
  }

  Node_free(node);
  node = NULL;
}

Test(Node, string_vide) {
  int i;
  Node *n;
  Node *node = NULL;
  char *data[] = {"hello", "", "42"};

  for (i = 0; i < 3; i++) {
    node = Node_add(node, data[i]);
  }

  i = 0;
  for (n = node; n; n = n->next) {
    cr_expect_str_eq(n->data, data[i], "Got: %s, Expected: %s\n", n->data,
                     data[i]);
    i++;
  }

  Node_free(node);
  node = NULL;
}

Test(Node, string_null) {
  int i;
  Node *n;
  Node *node = NULL;
  char *data[] = {"hello", "world", NULL};
  char *expected[] = {"hello", "world"};

  for (i = 0; i < 3; i++) {
    node = Node_add(node, data[i]);
  }

  Node_print(node);

  i = 0;
  for (n = node; n; n = n->next) {
    cr_expect_str_eq(n->data, expected[i], "Got: %s, Expected: %s\n", n->data,
                     expected[i]);
    i++;
  }

  Node_free(node);
  node = NULL;
}

// Test(parser, basique) {
//   Node *n;
//   FILE *f;
//   char *filename = "./parser.basique.criterion.test";
//   char *input = "mkdir -p /toto/foo";
//   char *expected[] = {"mkdir", "-p", "/toto/foo"};
//   int size = 3;
//   int i = 0, count = 0;
//
//   cr_assert_not_null(f = fopen(filename, "w"));
//
//   fprintf(f, input);
//   fclose(f);
//
//   freopen(filename, "r", stdin);
//
//   Node *got = parser();
//
//   for (n = got; n; n = n->next) {
//     if (i < size) {
//       cr_expect_str_eq(n->data, expected[i], "Got: %s, Expected: %s\n",
//       n->data,
//                        expected[i]);
//       i++;
//     }
//     count++;
//   }
//
//   parser_free();
// }
