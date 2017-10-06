#include "libstring.h"

int strcmp(const char *str1, const char *str2)
{
  size_t i = 0;
  for (; str1[i] != '\0' && str2[i] != '\0'; i++)
    if (str1[i] != str2[i])
      return 1;
  if (str1[i] == str2[i])
    return 0;
  return 1;
}
