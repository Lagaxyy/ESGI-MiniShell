#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdio.h>

#include "parser/parser.h"
#include "parser/parser_internal.h"

Test(buffer_allocation, basique) {
  FILE *f;
  char *expected = "hello world";
  char *filename = "build/buffer_allocation.basique.criterion.test";

  cr_assert_not_null(f = fopen(filename, "w"));

  fprintf(f, expected);
  fclose(f);

  freopen(filename, "r", stdin);

  char *got = buffer_allocation();

  cr_expect_str_eq(got, expected, "Got: %s, Expected: %s\n", got, expected);

  free(got);
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
