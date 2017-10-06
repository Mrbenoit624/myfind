#include "libstring.h"

size_t my_strlen(const char *string)
{
  size_t len = 0;
  for (; string[len] != '\0'; len++)
    continue;
  return len;
}
