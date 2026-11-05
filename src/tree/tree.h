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
int branch_add_child(branch *parent, branch *child);
void branch_print(branch *b, int depth);
void branch_free(branch *p);

#endif /* TREE_H */
