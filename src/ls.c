#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE = 200112L

#include "myfind.h"
#include "tree.h"
#include "libstring.h"
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

static
int my_find_rec(char *path, enum FLAG flag, struct tree *expr)
{
  DIR* dir = NULL;
  struct dirent* file_read = NULL;
  dir = opendir(path);
  if (!dir)
  {
    fprintf(stderr, "myfind: cannot do ‘%s’: Permission denied\n", path);
    return 0;
  }
  int res = 1;
  while ((file_read = readdir(dir)) != NULL)
  {
    if (strcmp(file_read->d_name, ".") != 0
        && strcmp(file_read->d_name, "..") !=0)
      res &= my_find(path, file_read->d_name, flag, expr);
  }
  closedir(dir);
  return res;
}
static
struct stat *check_lstat(const char *path)
{
  struct stat *buf = malloc(sizeof (struct stat));
  if (lstat(path, buf) != 0)
  {
    fprintf(stderr, "myfind: cannot do '%s': No such file or directory\n"
        , path);
    free(buf);
    return NULL;
  }
  return buf;
}
static
int check_FLAG_L_H(enum FLAG flag, struct stat *buf, char *tmp)
{
  if ((flag == FLAG_L || flag == FLAG_H) && S_ISLNK(buf->st_mode))
  {
    free(buf);
    tmp = my_strcat(tmp, "/");
    if (!(buf = check_lstat(tmp)))
      return 1;
  }
  return 0;
}
int my_find(char *path, const char *file, enum FLAG flag, struct tree *expr)
{
  int res = 0;
  struct stat *buf;
  char *tmp = my_strcpy(path, 512);
  my_strcat(tmp, file);
  if (!(buf = check_lstat(tmp)))
    return 1;
  if (flag != FLAG_D)
    print_tree(tmp, file, expr, buf);
  if (check_FLAG_L_H(flag, buf, tmp) == 1)
    return 1;
  if (flag == FLAG_H)
    flag = FLAG_P;
  size_t len = my_strlen(tmp) - 1;
  tmp[len] = tmp[len] == '/' ? '\0' : tmp[len];
  if (!S_ISDIR(buf->st_mode))
    res = 0;
  else if (!S_ISLNK(buf->st_mode) || (flag == FLAG_L && S_ISLNK(buf->st_mode)))
    res = my_find_rec(my_strcat(tmp,"/"), flag, expr);
  if (flag == FLAG_D)
    print_tree(tmp, file, expr, buf);
  free(buf);
  free(tmp);
  return res;
}
