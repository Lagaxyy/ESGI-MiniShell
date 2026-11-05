#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>

#include "tree/tree.h"

TestSuite(tree, .description = "Unit tests for the branch tree API");

Test(tree, create_returns_a_branch_for_a_lieu) {
  branch *b = branch_create("root", Folder);

  cr_assert_not_null(b);

  branch_free(b);
}

Test(tree, create_returns_a_branch_for_an_objet) {
  branch *b = branch_create("file.txt", File);

  cr_assert_not_null(b);

  branch_free(b);
}

Test(tree, create_rejects_an_empty_name) {
  branch *folder = branch_create("", Folder);
  branch *file = branch_create("", File);

  cr_expect_null(folder);
  cr_expect_null(file);

  branch_free(folder);
  branch_free(file);
}

Test(tree, create_rejects_a_name_of_64_characters) {
  char name[65];
  branch *b;

  memset(name, 'n', sizeof(name) - 1);
  name[sizeof(name) - 1] = '\0';

  b = branch_create(name, Folder);

  cr_expect_null(b);

  branch_free(b);
}

Test(tree, create_accepts_a_name_of_63_characters) {
  char name[64];
  branch *root = branch_create("root", Folder);
  branch *child;

  memset(name, 'n', sizeof(name) - 1);
  name[sizeof(name) - 1] = '\0';

  child = branch_create(name, Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find(root, name), child);

  branch_free(root);
}

Test(tree, create_preserves_special_characters) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("a b-c_d_e.txt (1)", File);
  branch *with_slash = branch_create("a b/c-d_e.txt (1)", File);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_null(with_slash);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find(root, "a b-c_d_e.txt (1)"), child);
  cr_expect_null(branch_find(root, "a b/c-d_e.txt (1)"));

  branch_free(root);
  branch_free(with_slash);
}

Test(tree, create_rejects_a_null_name) {
  branch *folder = branch_create(NULL, Folder);
  branch *file = branch_create(NULL, File);

  cr_expect_null(folder);
  cr_expect_null(file);

  branch_free(folder);
  branch_free(file);
}

Test(tree, create_accepts_a_one_character_name) {
  branch *b = branch_create("x", Folder);

  cr_assert_not_null(b);
  cr_expect_str_eq(b->nom, "x");

  branch_free(b);
}

Test(tree, create_rejects_a_slash_in_any_position) {
  char *names[] = {"a/b", "/a", "a/", "/"};

  for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
    branch *folder = branch_create(names[i], Folder);
    branch *file = branch_create(names[i], File);

    cr_expect_null(folder);
    cr_expect_null(file);

    branch_free(folder);
    branch_free(file);
  }
}

Test(tree, create_initializes_every_field) {
  branch *folder = branch_create("root", Folder);
  branch *file = branch_create("file.txt", File);

  cr_assert_not_null(folder);
  cr_assert_not_null(file);

  cr_expect_str_eq(folder->nom, "root");
  cr_expect_eq(folder->type, Folder);
  cr_expect_null(folder->parent);
  cr_expect_null(folder->next_sibling);
  cr_expect_null(folder->first_child);

  cr_expect_str_eq(file->nom, "file.txt");
  cr_expect_eq(file->type, File);
  cr_expect_null(file->parent);
  cr_expect_null(file->next_sibling);
  cr_expect_null(file->first_child);

  branch_free(folder);
  branch_free(file);
}

Test(tree, create_copies_the_name) {
  char name[16] = "original";
  branch *b = branch_create(name, Folder);

  cr_assert_not_null(b);

  strcpy(name, "changed");

  cr_expect_str_eq(b->nom, "original");

  branch_free(b);
}

Test(tree, find_child_on_an_empty_parent_returns_null) {
  branch *root = branch_create("root", Folder);

  cr_assert_not_null(root);
  cr_expect_null(branch_find(root, "child"));
  cr_expect_null(branch_find(root, ""));

  branch_free(root);
}

Test(tree, find_child_finds_each_sibling) {
  branch *root = branch_create("root", Folder);
  branch *one = branch_create("one", Folder);
  branch *two = branch_create("two", Folder);
  branch *three = branch_create("three", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(one);
  cr_assert_not_null(two);
  cr_assert_not_null(three);
  cr_expect_eq(branch_add_child(root, one), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, two), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, three), EXIT_SUCCESS);

  cr_expect_eq(branch_find(root, "one"), one);
  cr_expect_eq(branch_find(root, "two"), two);
  cr_expect_eq(branch_find(root, "three"), three);

  branch_free(root);
}

Test(tree, find_child_returns_null_for_an_unknown_name) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("child", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);

  cr_expect_null(branch_find(root, "unknown"));
  cr_expect_null(branch_find(root, "childs"));
  cr_expect_null(branch_find(root, "Child"));

  branch_free(root);
}

Test(tree, find_child_is_case_sensitive) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("Alpha", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);

  cr_expect_eq(branch_find(root, "Alpha"), child);
  cr_expect_null(branch_find(root, "alpha"));
  cr_expect_null(branch_find(root, "ALPHA"));

  branch_free(root);
}

Test(tree, find_child_requires_an_exact_name) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("foo", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);

  cr_expect_eq(branch_find(root, "foo"), child);
  cr_expect_null(branch_find(root, "fo"));
  cr_expect_null(branch_find(root, "foobar"));
  cr_expect_null(branch_find(root, "foo "));
  cr_expect_null(branch_find(root, " foo"));

  branch_free(root);
}

Test(tree, find_child_searches_the_whole_subtree) {
  branch *root = branch_create("root", Folder);
  branch *left = branch_create("left", Folder);
  branch *right = branch_create("right", Folder);
  branch *left_leaf = branch_create("left_leaf", Folder);
  branch *right_leaf = branch_create("right_leaf", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(left);
  cr_assert_not_null(right);
  cr_assert_not_null(left_leaf);
  cr_assert_not_null(right_leaf);
  cr_expect_eq(branch_add_child(root, left), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, right), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(left, left_leaf), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(right, right_leaf), EXIT_SUCCESS);

  cr_expect_eq(branch_find(root, "left"), left);
  cr_expect_eq(branch_find(root, "right"), right);
  cr_expect_eq(branch_find(root, "left_leaf"), left_leaf);
  cr_expect_eq(branch_find(root, "right_leaf"), right_leaf);
  cr_expect_eq(branch_find(left, "left_leaf"), left_leaf);
  cr_expect_null(branch_find(left, "right_leaf"));
  cr_expect_null(branch_find(root, "absent"));

  branch_free(root);
}

Test(tree, find_accepts_a_null_tree) {
  cr_expect_null(branch_find(NULL, "child"));
}

Test(tree, find_with_a_null_name_returns_null) {
  branch *root = branch_create("root", Folder);

  cr_assert_not_null(root);
  cr_expect_null(branch_find(root, NULL));

  branch_free(root);
}

Test(tree, find_returns_the_first_match_when_names_are_duplicated) {
  branch *root = branch_create("root", Folder);
  branch *left = branch_create("left", Folder);
  branch *right = branch_create("right", Folder);
  branch *left_same = branch_create("same", Folder);
  branch *right_same = branch_create("same", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(left);
  cr_assert_not_null(right);
  cr_assert_not_null(left_same);
  cr_assert_not_null(right_same);
  cr_expect_eq(branch_add_child(root, left), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, right), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(left, left_same), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(right, right_same), EXIT_SUCCESS);

  cr_expect_eq(branch_find(root, "same"), left_same);

  branch_free(root);
}

Test(tree, add_child_returns_exit_success_when_it_links_a_child) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("child", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find(root, "child"), child);

  branch_free(root);
}

Test(tree, add_child_accepts_an_objet_child) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("file", File);

  cr_assert_not_null(root);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find(root, "file"), child);

  branch_free(root);
}

Test(tree, add_child_rejects_a_child_of_an_objet_parent) {
  branch *parent = branch_create("file", File);
  branch *child = branch_create("child", Folder);

  cr_assert_not_null(parent);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(parent, child), EXIT_FAILURE);
  cr_expect_null(branch_find(parent, "child"));
  cr_expect_null(child->parent);
  cr_expect_null(child->next_sibling);

  branch_free(child);
  branch_free(parent);
}

Test(tree, add_child_rejects_a_duplicate_name) {
  branch *root = branch_create("root", Folder);
  branch *first = branch_create("dup", Folder);
  branch *second = branch_create("dup", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(first);
  cr_assert_not_null(second);

  cr_expect_eq(branch_add_child(root, first), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, second), EXIT_FAILURE);

  cr_expect_eq(branch_find(root, "dup"), first);
  cr_expect_neq(branch_find(root, "dup"), second);
  cr_expect_eq(root->first_child, first);
  cr_expect_null(first->next_sibling);
  cr_expect_null(second->parent);
  cr_expect_null(second->next_sibling);

  branch_free(root);
  branch_free(second);
}

Test(tree, add_child_duplicate_check_is_case_sensitive) {
  branch *root = branch_create("root", Folder);
  branch *lower = branch_create("name", Folder);
  branch *upper = branch_create("Name", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(lower);
  cr_assert_not_null(upper);

  cr_expect_eq(branch_add_child(root, lower), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, upper), EXIT_SUCCESS);

  cr_expect_eq(branch_find(root, "name"), lower);
  cr_expect_eq(branch_find(root, "Name"), upper);

  branch_free(root);
}

Test(tree, add_child_allows_the_same_name_under_different_parents) {
  branch *root = branch_create("root", Folder);
  branch *left = branch_create("left", Folder);
  branch *right = branch_create("right", Folder);
  branch *left_child = branch_create("same", Folder);
  branch *right_child = branch_create("same", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(left);
  cr_assert_not_null(right);
  cr_assert_not_null(left_child);
  cr_assert_not_null(right_child);

  cr_expect_eq(branch_add_child(root, left), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, right), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(left, left_child), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(right, right_child), EXIT_SUCCESS);

  cr_expect_eq(branch_find(left, "same"), left_child);
  cr_expect_eq(branch_find(right, "same"), right_child);

  branch_free(root);
}

Test(tree, add_child_allows_a_child_with_the_same_name_as_its_parent) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("root", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find(root, "root"), root);
  cr_expect_eq(branch_find(root->first_child, "root"), child);

  branch_free(root);
}

Test(tree, add_child_rejected_elsewhere_can_be_attached_to_another_parent) {
  branch *file = branch_create("file", File);
  branch *child = branch_create("kept", Folder);
  branch *root = branch_create("root", Folder);

  cr_assert_not_null(file);
  cr_assert_not_null(child);
  cr_assert_not_null(root);

  cr_expect_eq(branch_add_child(file, child), EXIT_FAILURE);
  cr_expect_null(branch_find(file, "kept"));

  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);
  cr_expect_eq(branch_find(root, "kept"), child);

  branch_free(root);
  branch_free(file);
}

Test(tree, add_child_builds_a_deep_hierarchy) {
  branch *root = branch_create("root", Folder);
  branch *n1 = branch_create("n1", Folder);
  branch *n2 = branch_create("n2", Folder);
  branch *n3 = branch_create("n3", Folder);
  branch *n4 = branch_create("n4", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(n1);
  cr_assert_not_null(n2);
  cr_assert_not_null(n3);
  cr_assert_not_null(n4);

  cr_expect_eq(branch_add_child(root, n1), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n1, n2), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n2, n3), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n3, n4), EXIT_SUCCESS);

  cr_expect_eq(branch_find(root, "n4"), n4);

  branch_free(root);
}

Test(tree, add_child_sets_parent_on_every_child) {
  branch *root = branch_create("root", Folder);
  branch *first = branch_create("first", Folder);
  branch *second = branch_create("second", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(first);
  cr_assert_not_null(second);

  cr_expect_eq(branch_add_child(root, first), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, second), EXIT_SUCCESS);

  cr_expect_eq(first->parent, root);
  cr_expect_eq(second->parent, root);

  branch_free(root);
}

Test(tree, add_child_keeps_insertion_order) {
  branch *root = branch_create("root", Folder);
  branch *one = branch_create("one", Folder);
  branch *two = branch_create("two", Folder);
  branch *three = branch_create("three", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(one);
  cr_assert_not_null(two);
  cr_assert_not_null(three);

  cr_expect_eq(branch_add_child(root, one), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, two), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, three), EXIT_SUCCESS);

  cr_expect_eq(root->first_child, one);
  cr_expect_eq(one->next_sibling, two);
  cr_expect_eq(two->next_sibling, three);
  cr_expect_null(three->next_sibling);

  branch_free(root);
}

Test(tree, add_child_rejects_a_duplicate_of_the_last_sibling) {
  branch *root = branch_create("root", Folder);
  branch *first = branch_create("first", Folder);
  branch *last = branch_create("last", Folder);
  branch *duplicate = branch_create("last", Folder);
  branch **link;
  int result;

  cr_assert_not_null(root);
  cr_assert_not_null(first);
  cr_assert_not_null(last);
  cr_assert_not_null(duplicate);

  cr_expect_eq(branch_add_child(root, first), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(root, last), EXIT_SUCCESS);

  result = branch_add_child(root, duplicate);

  cr_expect_eq(result, EXIT_FAILURE);
  cr_expect_eq(root->first_child, first);
  cr_expect_eq(first->next_sibling, last);
  cr_expect_null(last->next_sibling);
  cr_expect_null(duplicate->parent);
  cr_expect_null(duplicate->next_sibling);

  link = &root->first_child;
  while (*link != NULL && *link != duplicate) {
    link = &(*link)->next_sibling;
  }
  if (*link == duplicate) {
    *link = duplicate->next_sibling;
  }
  duplicate->parent = NULL;
  duplicate->next_sibling = NULL;

  branch_free(root);
  branch_free(duplicate);
}

Test(tree, add_child_rejects_a_null_parent) {
  branch *child = branch_create("child", Folder);

  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(NULL, child), EXIT_FAILURE);

  branch_free(child);
}

Test(tree, add_child_rejects_a_null_child) {
  branch *root = branch_create("root", Folder);

  cr_assert_not_null(root);

  cr_expect_eq(branch_add_child(root, NULL), EXIT_FAILURE);
  cr_expect_null(root->first_child);

  branch_free(root);
}

Test(tree, add_child_rejects_a_branch_as_its_own_child) {
  branch *root = branch_create("root", Folder);
  int result;

  cr_assert_not_null(root);

  result = branch_add_child(root, root);

  cr_expect_eq(result, EXIT_FAILURE);

  if (result == EXIT_SUCCESS) {
    root->first_child = NULL;
    root->parent = NULL;
  }

  branch_free(root);
}

Test(tree, add_child_rejects_an_ancestor_as_a_child) {
  branch *root = branch_create("root", Folder);
  branch *child = branch_create("child", Folder);
  int result;

  cr_assert_not_null(root);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(root, child), EXIT_SUCCESS);

  result = branch_add_child(child, root);

  cr_expect_eq(result, EXIT_FAILURE);

  if (result == EXIT_SUCCESS) {
    root->first_child = NULL;
    root->parent = NULL;
    child->first_child = NULL;
    branch_free(child);
    branch_free(root);
  } else {
    branch_free(root);
  }
}

Test(tree, add_child_rejects_a_child_already_attached) {
  branch *old_parent = branch_create("old", Folder);
  branch *new_parent = branch_create("new", Folder);
  branch *child = branch_create("child", Folder);
  branch **link;
  int result;

  cr_assert_not_null(old_parent);
  cr_assert_not_null(new_parent);
  cr_assert_not_null(child);
  cr_expect_eq(branch_add_child(old_parent, child), EXIT_SUCCESS);

  result = branch_add_child(new_parent, child);

  cr_expect_eq(result, EXIT_FAILURE);
  cr_expect_eq(old_parent->first_child, child);
  cr_expect_null(new_parent->first_child);

  link = &new_parent->first_child;
  while (*link != NULL && *link != child) {
    link = &(*link)->next_sibling;
  }
  if (*link == child) {
    *link = child->next_sibling;
  }
  child->next_sibling = NULL;
  child->parent = old_parent;

  branch_free(old_parent);
  branch_free(new_parent);
}

Test(tree, add_child_rejects_an_objet_child_of_an_objet_parent) {
  branch *parent = branch_create("file", File);
  branch *child = branch_create("nested", File);

  cr_assert_not_null(parent);
  cr_assert_not_null(child);

  cr_expect_eq(branch_add_child(parent, child), EXIT_FAILURE);
  cr_expect_null(branch_find(parent, "nested"));
  cr_expect_null(child->parent);
  cr_expect_null(child->next_sibling);

  branch_free(child);
  branch_free(parent);
}

Test(tree, free_null_is_a_noop) {
  branch_free(NULL);
  branch_free(NULL);
}

Test(tree, free_a_leaf) {
  branch *b = branch_create("leaf", File);

  cr_assert_not_null(b);

  branch_free(b);
}

Test(tree, free_a_parent_with_several_children) {
  branch *root = branch_create("root", Folder);
  branch *one = branch_create("one", Folder);
  branch *two = branch_create("two", Folder);
  branch *three = branch_create("three", Folder);

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
  branch *root = branch_create("root", Folder);
  branch *n1 = branch_create("n1", Folder);
  branch *n2 = branch_create("n2", Folder);
  branch *n3 = branch_create("n3", Folder);

  cr_assert_not_null(root);
  cr_assert_not_null(n1);
  cr_assert_not_null(n2);
  cr_assert_not_null(n3);

  cr_expect_eq(branch_add_child(root, n1), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n1, n2), EXIT_SUCCESS);
  cr_expect_eq(branch_add_child(n2, n3), EXIT_SUCCESS);

  branch_free(root);
}
