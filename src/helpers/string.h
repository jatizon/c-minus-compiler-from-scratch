#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include <stdbool.h>

char* string_from_char(char c);

bool is_parenthesis(char c);

bool is_literal(char c);

char* remove_spaces(char* input);

#endif
