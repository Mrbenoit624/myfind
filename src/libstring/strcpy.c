#include "libstring.h"

char *my_strcpy(const char *str, size_t n)
{
  char *res = malloc(n * sizeof (char));
  size_t i = 0;
  for (; str[i] != '\0'; i++)
    res[i] = str[i];
  res[i] = '\0';
  return res;
}
