#ifndef LIBSTRING_H
# define LIBSTRING_H
# include <stdlib.h>

size_t my_strlen(const char *string);
char *my_strcat(char *dest, const char *src);
int strcmp(const char *str1, const char *str2);
char *my_strcpy(const char *str, size_t n);
#endif /* !LIBSTRING_H */
