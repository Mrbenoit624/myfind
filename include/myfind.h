#ifndef MYFIND_H
# define MYFIND_H
# include "tree.h"

enum FLAG {
  FLAG_D,
  FLAG_H,
  FLAG_L,
  FLAG_P,
  FLAG_ERROR
};
int my_find(char *path, const char *file, enum FLAG flag, struct tree *expr);

#endif /* !MYFIND_H */
