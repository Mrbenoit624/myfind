#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCES = 200809L

#include "myfind.h"
#include "tree.h"
#include "libstring.h"
#include <fnmatch.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static
enum TYPE switch_type(char *flag)
{
  switch (flag[0])
  {
    case 'b':
      return B;
    case 'c':
      return C;
    case 'd':
      return D;
    case 'l':
      return L;
    case 'f':
      return F;
    case 'p':
      return P;
    case 's':
      return S;
    default:
      fprintf(stderr, "cannot do arguments : wrong flag\n");
      return ERROR_TYPE;
      break;
  }
}

  static
enum TYPE check_type(char *flag)
{
  if (!flag)
  {
    fprintf(stderr, "cannot do arguments : wrong flag\n");
    return ERROR_TYPE;
  }
  else
    return switch_type(flag);
}
  static
struct expr *parse(char **flag, int *i, int argc)
{
  struct expr *expr= malloc(sizeof (struct expr));
  expr->expr = NONE;
  expr->arg = NULL;
  expr->type = ERROR_TYPE;
  if (flag[*i][0] == '-')
  {
    if (strcmp(flag[*i], "-name") == 0)
    {
      if (argc <= (*i ) + 1)
      {
        expr->expr = ERROR;
        return expr;
      }
      expr->expr = NAME;
      expr->arg = malloc(sizeof (char *));
      expr->arg[0] = flag[++(*i)];
    }
    else if (strcmp(flag[*i], "-type") == 0)
    {
      expr->expr = TYPE;
      expr->type = check_type(flag[++(*i)]);
    }
    else if (strcmp(flag[*i], "-o") == 0)
      expr->expr = OR;
    else if (strcmp(flag[*i], "-a") == 0)
      expr->expr = AND;
    else if (strcmp(flag[*i], "-print") == 0)
      expr->expr = PRINT;
    else if (strcmp(flag[*i], "-exec") == 0
        || strcmp(flag[*i], "-execdir") == 0)
    {
      if (strcmp(flag[*i], "-exec") == 0)
        expr->expr = EXEC;
      else
        expr->expr = EXEC_DIR;
      int j = 0;
      (*i)++;
      for (; *i + j < argc && strcmp(flag[*i + j], ";") != 0; j++)
        continue;
      expr->arg = malloc(sizeof (char *) * (++j));
      expr->func = flag[*i];
      for (int k = 0; k < j - 1; k++)
        expr->arg[k] = flag[*i + k];
      *i += j;
      expr->arg[j - 1] = NULL;
    }
    else
    {
      fprintf(stderr, "cannot do arguments : wrong flag\n");
      exit(1);
    }
  }
  else if (flag[*i][0] == '(')
    expr->expr = PO;
  else if (flag[*i][0] == ')')
    expr->expr = PC;
  else if (flag[*i][0] == '!')
    expr->expr = NOT;
  return expr;
}
#define ADD_NODE_PO                                            \
      (*i)++;                                                  \
      if (*ope == 0)                                           \
      {                                                        \
        struct tree *tmp = malloc(sizeof (struct tree));       \
        struct expr *tmp_expr = malloc(sizeof (struct expr));  \
        tmp_expr->expr = AND;                                  \
        tmp_expr->arg = NULL;                                  \
        tmp_expr->type = ERROR_TYPE;                           \
        tmp->data = tmp_expr;                                  \
        tmp->left = tree;                                      \
        tmp->right = NULL;                                     \
        tree = tmp;                                            \
      }                                                        \
      if (tree)                                                \
        tree->right = create_tree(argv, argc, i);              \
      else                                                     \
        tree = create_tree(argv, argc, i);                     \
      break;
#define ADD_NODE_DEFAULT                                       \
      t = malloc(sizeof (struct tree));                        \
      t->data = expr;                                          \
      t->left = NULL;                                          \
      t->right = NULL;                                         \
      if (!tree)                                               \
        tree = t;                                              \
      else                                                     \
      {                                                        \
        if (*ope == 0)                                         \
        {                                                      \
          struct tree *tmp = malloc(sizeof (struct tree));     \
          struct expr *tmp_expr = malloc(sizeof (struct expr));\
          tmp_expr->expr = AND;                                \
          tmp_expr->arg = NULL;                                \
          tmp_expr->type = ERROR_TYPE;                         \
          tmp->data = tmp_expr;                                \
          tmp->left = tree;                                    \
          tmp->right = NULL;                                   \
          tree = tmp;                                          \
        }                                                      \
        tree->right = t;                                       \
      }                                                        \
      *ope = 0;                                                \
      break;
#define ADD_NODE_AND                                           \
      if (!tree)                                               \
      {                                                        \
        fprintf(stderr, "cannot do begin by : a flag\n");      \
        exit(1);                                               \
      }                                                        \
      t = malloc(sizeof (struct tree));                        \
      t->data = expr;                                          \
      if (tree->right)                                         \
      {                                                        \
        t->left = tree->right;                                 \
        (*i)++;                                                \
        t->right =  create_tree(argv, argc, i);                \
        tree->right = t;                                       \
      }                                                        \
      else                                                     \
      {                                                        \
        t->left = tree;                                        \
        tree = t;                                              \
      }                                                        \
      *ope = 1;                                                \
      break;
#define ADD_NODE_OR                                            \
      if (!tree)                                               \
      {                                                        \
        fprintf(stderr, "cannot do begin by : a flag\n");      \
        exit(1);                                               \
      }                                                        \
      t = malloc(sizeof (struct tree));                        \
      t->data = expr;                                          \
      t->left = tree;                                          \
      t->right = NULL;                                         \
      tree = t;                                                \
      *ope = 1;                                                \
      break;
static
struct tree *add_node(char **argv, int argc, struct tree *tree, int *i
    , int *ope)
{
  struct expr *expr = parse(argv, i, argc);
  struct tree *t = NULL;
  switch (expr->expr)
  {
    case NOT:
      t = malloc(sizeof (struct tree));
      t->data = expr;
      (*i)++;
      t->left = add_node(argv, argc, t->left, i, ope);
      if (tree)
        tree->right = t;
      else
        tree = t;
      break;
    case PO:
      ADD_NODE_PO
    case PC:
      return tree;
    case OR:
      ADD_NODE_OR
    case AND:
      ADD_NODE_AND
    case ERROR:
      fprintf(stderr, "cannot do arguments : wrong flag\n");
      exit(1);
    default:
      ADD_NODE_DEFAULT
  }
  return tree;
}

struct tree *create_tree(char **argv, int argc, int *i)
{
  struct tree *tree = NULL;
  int ope = 0;
  for (; *i < argc; (*i)++)
  {
    tree = add_node(argv, argc, tree, i, &ope);
  }
  if (tree)
  {
    enum EXPR test = tree->data->expr;
    if ((test == OR || test == AND) && tree->right == NULL)
    {
      fprintf(stderr, "cannot do finish by : an operator\n");
      free(tree);
      exit(1);
    }
  }
  return tree;
}
#define CHECK_FILE_B             \
      if (S_ISBLK(buf->st_mode)) \
        return 1;                \
      return 0;
#define CHECK_FILE_C             \
      if (S_ISCHR(buf->st_mode)) \
        return 1;                \
      return 0;
#define CHECK_FILE_D             \
      if (S_ISDIR(buf->st_mode)) \
        return 1;                \
      return 0;
#define CHECK_FILE_F             \
      if (S_ISREG(buf->st_mode)) \
        return 1;                \
      return 0;
#define CHECK_FILE_L             \
      if (S_ISLNK(buf->st_mode)) \
        return 1;                \
      return 0;
#define CHECK_FILE_P             \
      if (S_ISFIFO(buf->st_mode))\
        return 1;                \
      return 0;
#define CHECK_FILE_S             \
      if (S_ISSOCK(buf->st_mode))\
        return 1;                \
      return 0;
  static
int check_file_type(enum TYPE type, struct stat *buf)
{
  switch (type)
  {
    case B:
      CHECK_FILE_B
    case C:
      CHECK_FILE_C
    case D:
      CHECK_FILE_D
    case F:
      CHECK_FILE_F
    case L:
      CHECK_FILE_L
    case P:
      CHECK_FILE_P
    case S:
      CHECK_FILE_S
    default:
      return 0;
  }
  return 0;
}
#define INIT_CHECK_TREE  \
  int _chdir = 0;        \
  char *tmp_path = NULL; \
  pid_t pid;
#define CHECK_NAME                                              \
  if (my_strlen(file) == 0 && fnmatch(expr->data->arg[0], path, \
        FNM_PATHNAME) == 0)                                     \
  return 1;                                                     \
else if (fnmatch(expr->data->arg[0], file, FNM_PATHNAME) == 0)  \
  return 1;                                                     \
  return 0;
#define CHECK_EXEC                                  \
pid = fork();                                       \
int len = 0;                                        \
       for (; expr->data->arg[len]; len++)          \
         continue;                                  \
       char **tmp = malloc(len * sizeof (char *));  \
       for (int j = 0; expr->data->arg[j]; j++)     \
         tmp[j] = expr->data->arg[j];               \
       tmp[len] = NULL;                             \
       for (int j = 0; tmp[j]; j++)                 \
         if (strcmp(tmp[j], "{}") == 0)             \
           tmp[j] =  _chdir ? tmp_path : path;      \
       if (pid == 0)                                \
       {                                            \
         if (_chdir)                                \
           chdir(path);                             \
         execvp(expr->data->func, tmp);             \
         exit(1);                                   \
       }                                            \
       else                                         \
       {                                            \
         int status = 0;                            \
         waitpid(pid, &status, 0);                  \
         *print = 1;                                \
         free(tmp);                                 \
         return WEXITED;                            \
       }
#define CHECK_EXEC_DIR                              \
      _chdir = 1;                                   \
      if (strcmp(file, "") == 0)                    \
        file = path;                                \
      tmp_path = my_strcpy("./", 255);              \
      tmp_path = my_strcat(tmp_path, file);
#define CHECK_PRINT                                 \
      printf("%s\n", path);                         \
      *print = 1;                                   \
      return 1;

  static
int check_tree(char *path, const char *file, struct tree *expr,
    struct stat *buf, int *print)
{
  INIT_CHECK_TREE
  if (!expr)
    return 1;
  switch (expr->data->expr)
  {
  case NOT:
      return ! check_tree(path, file, expr->left, buf, print);
  case OR:
      return (check_tree(path, file, expr->left, buf, print)
          || check_tree(path, file, expr->right, buf, print));
  case AND:
      return (check_tree(path, file, expr->left, buf, print)
          && check_tree(path, file, expr->right, buf, print));
  case NAME:
      CHECK_NAME
  case PRINT:
        CHECK_PRINT
  case TYPE:
      return check_file_type(expr->data->type, buf);
  case EXEC_DIR:
      CHECK_EXEC_DIR
      __attribute__ ((fallthrough));
  case EXEC:
      CHECK_EXEC
  default:
       return 0;
  }
  return 0;
}
void print_tree(char *path, const char *file, struct tree *expr,
    struct stat *buf)
{
  int print = 0;
  if (check_tree(path, file, expr, buf, &print) == 1 && print == 0)
    printf("%s\n", path);
}
void destroy_tree(struct tree *tree)
{
  if (!tree)
    return;
  destroy_tree(tree->left);
  destroy_tree(tree->right);
  if (tree->data)
  {
    if (tree->data->arg)
      free(tree->data->arg);
    free(tree->data);
  }
  free(tree);
}
