#ifndef TREE_H
#define TREE_H

typedef struct branch branch;
typedef enum { Lieu, Objet } branch_type;

branch *branch_create(char *nom, branch_type type);
branch *branch_find_child(branch *parent, char *nom);
int branch_add_child(branch *parent, branch *child);
void branch_print(branch *b, int depth);
void branch_free(branch *p);

#endif /* TREE_H */
