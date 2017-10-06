#include "libstring.h"

char *my_strcat(char *dest, const char *src)
{
  size_t size_src = my_strlen(src);
  size_t size_dest = my_strlen(dest);
  for (size_t i = 0; i < size_src; i++)
    dest[size_dest + i] = src[i];
  dest[size_src + size_dest] = '\0';
  return dest;
}
