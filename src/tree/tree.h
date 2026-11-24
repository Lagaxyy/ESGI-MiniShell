#ifndef TREE_H
#define TREE_H

typedef enum { Folder, File } branch_type;

typedef struct branch branch;
struct branch {
  char nom[64];
  branch_type type;
  branch *parent;
  branch *next_sibling;
  branch *first_child;
};

branch *branch_create(char *nom, branch_type type);
branch *branch_find(branch *b, char *nom);
branch *branch_find_child(branch *parent, char *nom);
int branch_add_child(branch *parent, branch *child);
void branch_print(branch *b, int depth);
void branch_free(branch *p);
int branch_compare(branch *b1, branch *b2);
int branch_save(branch *b, const char *path);
branch *branch_load(const char *path);

#endif /* TREE_H */
