#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>

#include "tree/tree.h"

TestSuite(tree, .description = "Unit tests for the branch tree API");

Test(tree, create_returns_a_branch_for_a_lieu) {
  branch *b = branch_create("root", Lieu);

  cr_assert_not_null(b);

  branch_free(b);
}

Test(tree, create_returns_a_branch_for_an_objet) {
  branch *b = branch_create("file.txt", Objet);

  cr_assert_not_null(b);

  branch_free(b);
}

Test(tree, create_rejects_an_empty_name) {
  branch *lieu = branch_create("", Lieu);
  branch *objet = branch_create("", Objet);

  cr_expect_null(lieu);
  cr_expect_null(objet);

  branch_free(lieu);
  branch_free(objet);
}

Test(tree, create_rejects_a_name_of_64_characters) {
  char name[65];
  branch *b;

  memset(name, 'n', sizeof(name) - 1);
  name[sizeof(name) - 1] = '\0';

  b = branch_create(name, Lieu);

  cr_expect_null(b);

  branch_free(b);
}

Test(tree, create_accepts_a_name_of_63_characters) {
  char name[64];
  branch *root = branch_create("root", Lieu);
  branch *child;

  memset(name, 'n', sizeof(name) - 1);
  name[sizeof(name) - 1] = '\0';

  child = branch_create(name, Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find_child(root, name), child);

  branch_free(root);
}

Test(tree, create_preserves_special_characters) {
  branch *root = branch_create("root", Lieu);
  branch *child = branch_create("a b/c-d_e.txt (1)", Objet);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find_child(root, "a b/c-d_e.txt (1)"), child);

  branch_free(root);
}

Test(tree, find_child_on_an_empty_parent_returns_null) {
  branch *root = branch_create("root", Lieu);

  cr_assert_not_null(root);
  cr_expect_null(branch_find_child(root, "child"));
  cr_expect_null(branch_find_child(root, "root"));
  cr_expect_null(branch_find_child(root, ""));

  branch_free(root);
}

Test(tree, find_child_finds_each_sibling) {
  branch *root = branch_create("root", Lieu);
  branch *one = branch_create("one", Lieu);
  branch *two = branch_create("two", Lieu);
  branch *three = branch_create("three", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(one);
  cr_assert_not_null(two);
  cr_assert_not_null(three);
  cr_expect_eq(branch_add_child(root, one), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, two), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, three), EXIT_SUCCESS);

  cr_expect_eq(branch_find_child(root, "one"), one);
  cr_expect_eq(branch_find_child(root, "two"), two);
  cr_expect_eq(branch_find_child(root, "three"), three);

  branch_free(root);
}

Test(tree, find_child_returns_null_for_an_unknown_name) {
  branch *root = branch_create("root", Lieu);
  branch *child = branch_create("child", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);

  cr_expect_null(branch_find_child(root, "unknown"));
  cr_expect_null(branch_find_child(root, "childs"));
  cr_expect_null(branch_find_child(root, "Child"));

  branch_free(root);
}

Test(tree, find_child_is_case_sensitive) {
  branch *root = branch_create("root", Lieu);
  branch *child = branch_create("Alpha", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);

  cr_expect_eq(branch_find_child(root, "Alpha"), child);
  cr_expect_null(branch_find_child(root, "alpha"));
  cr_expect_null(branch_find_child(root, "ALPHA"));

  branch_free(root);
}

Test(tree, find_child_requires_an_exact_name) {
  branch *root = branch_create("root", Lieu);
  branch *child = branch_create("foo", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);

  cr_expect_eq(branch_find_child(root, "foo"), child);
  cr_expect_null(branch_find_child(root, "fo"));
  cr_expect_null(branch_find_child(root, "foobar"));
  cr_expect_null(branch_find_child(root, "foo "));
  cr_expect_null(branch_find_child(root, " foo"));

  branch_free(root);
}

Test(tree, find_child_searches_the_whole_subtree) {
  branch *root = branch_create("root", Lieu);
  branch *left = branch_create("left", Lieu);
  branch *right = branch_create("right", Lieu);
  branch *left_leaf = branch_create("left_leaf", Lieu);
  branch *right_leaf = branch_create("right_leaf", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(left);
  cr_assert_not_null(right);
  cr_assert_not_null(left_leaf);
  cr_assert_not_null(right_leaf);
  cr_expect_eq(branch_add_child(root, left), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, right), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(left, left_leaf), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(right, right_leaf), EXIT_SUCCESS);

  cr_expect_eq(branch_find_child(root, "left"), left);
  cr_expect_eq(branch_find_child(root, "right"), right);
  cr_expect_eq(branch_find_child(root, "left_leaf"), left_leaf);
  cr_expect_eq(branch_find_child(root, "right_leaf"), right_leaf);
  cr_expect_eq(branch_find_child(left, "left_leaf"), left_leaf);
  cr_expect_null(branch_find_child(left, "right_leaf"));
  cr_expect_null(branch_find_child(root, "absent"));

  branch_free(root);
}

Test(tree, add_child_returns_exit_success_when_it_links_a_child) {
  branch *root = branch_create("root", Lieu);
  branch *child = branch_create("child", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find_child(root, "child"), child);

  branch_free(root);
}

Test(tree, add_child_accepts_an_objet_child) {
  branch *root = branch_create("root", Lieu);
  branch *child = branch_create("file", Objet);

  cr_assert_not_null(root);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find_child(root, "file"), child);

  branch_free(root);
}

Test(tree, add_child_rejects_a_child_of_an_objet_parent) {
  branch *parent = branch_create("file", Objet);
  branch *child = branch_create("child", Lieu);

  cr_assert_not_null(parent);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(parent, child), EXIT_FAILURE);
  cr_expect_null(branch_find_child(parent, "child"));

  branch_free(child);
  branch_free(parent);
}

Test(tree, add_child_rejects_a_duplicate_name) {
  branch *root = branch_create("root", Lieu);
  branch *first = branch_create("dup", Lieu);
  branch *second = branch_create("dup", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(first);
  cr_assert_not_null(second);

  cr_expect_eq(branch_add_child(root, first), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, second), EXIT_FAILURE);

  cr_expect_eq(branch_find_child(root, "dup"), first);
  cr_expect_neq(branch_find_child(root, "dup"), second);

  branch_free(root);
  branch_free(second);
}

Test(tree, add_child_duplicate_check_is_case_sensitive) {
  branch *root = branch_create("root", Lieu);
  branch *lower = branch_create("name", Lieu);
  branch *upper = branch_create("Name", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(lower);
  cr_assert_not_null(upper);

  cr_expect_eq(branch_add_child(root, lower), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, upper), EXIT_SUCCESS);

  cr_expect_eq(branch_find_child(root, "name"), lower);
  cr_expect_eq(branch_find_child(root, "Name"), upper);

  branch_free(root);
}

Test(tree, add_child_allows_the_same_name_under_different_parents) {
  branch *root = branch_create("root", Lieu);
  branch *left = branch_create("left", Lieu);
  branch *right = branch_create("right", Lieu);
  branch *left_child = branch_create("same", Lieu);
  branch *right_child = branch_create("same", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(left);
  cr_assert_not_null(right);
  cr_assert_not_null(left_child);
  cr_assert_not_null(right_child);

  cr_expect_eq(branch_add_child(root, left), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, right), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(left, left_child), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(right, right_child), EXIT_SUCCESS);

  cr_expect_eq(branch_find_child(left, "same"), left_child);
  cr_expect_eq(branch_find_child(right, "same"), right_child);

  branch_free(root);
}

Test(tree, add_child_allows_a_child_with_the_same_name_as_its_parent) {
  branch *root = branch_create("root", Lieu);
  branch *child = branch_create("root", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find_child(root, "root"), child);

  branch_free(root);
}

Test(tree, add_child_rejected_elsewhere_can_be_attached_to_another_parent) {
  branch *objet = branch_create("file", Objet);
  branch *child = branch_create("kept", Lieu);
  branch *root = branch_create("root", Lieu);

  cr_assert_not_null(objet);
  cr_assert_not_null(child);
  cr_assert_not_null(root);

  cr_expect_eq(branch_add_child(objet, child), EXIT_FAILURE);
  cr_expect_null(branch_find_child(objet, "kept"));

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find_child(root, "kept"), child);

  branch_free(root);
  branch_free(objet);
}

Test(tree, add_child_builds_a_deep_hierarchy) {
  branch *root = branch_create("root", Lieu);
  branch *n1 = branch_create("n1", Lieu);
  branch *n2 = branch_create("n2", Lieu);
  branch *n3 = branch_create("n3", Lieu);
  branch *n4 = branch_create("n4", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(n1);
  cr_assert_not_null(n2);
  cr_assert_not_null(n3);
  cr_assert_not_null(n4);

  cr_expect_eq(branch_add_child(root, n1), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n1, n2), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n2, n3), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n3, n4), EXIT_SUCCESS);

  cr_expect_eq(branch_find_child(root, "n4"), n4);

  branch_free(root);
}

Test(tree, free_null_is_a_noop) {
  branch_free(NULL);
  branch_free(NULL);
}

Test(tree, free_a_leaf) {
  branch *b = branch_create("leaf", Objet);

  cr_assert_not_null(b);

  branch_free(b);
}

Test(tree, free_a_parent_with_several_children) {
  branch *root = branch_create("root", Lieu);
  branch *one = branch_create("one", Lieu);
  branch *two = branch_create("two", Lieu);
  branch *three = branch_create("three", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(one);
  cr_assert_not_null(two);
  cr_assert_not_null(three);

  cr_expect_eq(branch_add_child(root, one), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, two), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, three), EXIT_SUCCESS);

  branch_free(root);
}

Test(tree, free_a_nested_tree) {
  branch *root = branch_create("root", Lieu);
  branch *n1 = branch_create("n1", Lieu);
  branch *n2 = branch_create("n2", Lieu);
  branch *n3 = branch_create("n3", Lieu);

  cr_assert_not_null(root);
  cr_assert_not_null(n1);
  cr_assert_not_null(n2);
  cr_assert_not_null(n3);

  cr_expect_eq(branch_add_child(root, n1), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n1, n2), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n2, n3), EXIT_SUCCESS);

  branch_free(root);
}
