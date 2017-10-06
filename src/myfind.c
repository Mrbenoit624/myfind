#include "myfind.h"
#include "tree.h"
#include "libstring.h"
#include <stdio.h>
#include <stdlib.h>

static
enum FLAG check_flag(char **flag, int *i, int argc)
{
  enum FLAG res = FLAG_P;
  for (; *i < argc; (*i)++)
  {
    if (flag[*i][0] == '-')
      switch (flag[*i][1])
      {
      case 'd':
        res = FLAG_D;
        break;
      case 'H':
        res = FLAG_H;
        break;
      case 'L':
        res = FLAG_L;
        break;
      case 'P':
        res = FLAG_P;
        break;
      default:
        return FLAG_ERROR;
        break;
      }
    else
      break;
  }
    return res;
}
static
char **check_files(char *path, char **flag, int *i, int argc, size_t *len)
{
  *len = 0;
  char **files = malloc((argc - *i) * sizeof (char *));
  for (; *i < argc; (*i)++, (*len)++)
  {
    if (flag[*i][0] != '-' && flag[*i][0] != '!' && flag[*i][0] != '(')
      files[*len] = flag[*i];
    else
      break;
  }
  if (*len != 0)
    return files;
  files[(*len)++] = path;
  return files;

}
int main(int argc, char **argv)
{
  char *path;
  char tmp[2] = ".";
  path = tmp;
  char **paths;
  size_t len = 0;
  int index = 1;
  enum FLAG flag = check_flag(argv, &index, argc);
  paths = check_files(path, argv, &index, argc, &len);
  struct tree *expr = create_tree(argv, argc, &index);
  int res = 0;
  for (size_t i = 0; i < len && res == 0; i++)
    res |= my_find(paths[i], "", flag, expr);
  destroy_tree(expr);
  free(paths);
  return res;
}
