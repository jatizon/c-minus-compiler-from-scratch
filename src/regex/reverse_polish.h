#ifndef REVERSE_POLISH_H
#define REVERSE_POLISH_H

#include <stdbool.h>
#include "src/data_structures/vector.h"
#include "src/regex/regex.h"


bool can_start_expression(char c);

bool can_end_expression(char c);

bool is_implicit_concat_between(char c1, char c2);

char* resolve_implicit_concats(char* input);

char* pre_process_regex(char* regex);

char* build_reverse_polish_from_regex(char* regex);

#endif
