#ifndef REGEX_H
#define REGEX_H

#include <stdbool.h>

typedef enum RegexOperator {
    UNION = '|',
    CONCAT = '.',
    STAR = '*',
    LEFT_PARENTHESIS = '(',
    RIGHT_PARENTHESIS = ')',
} RegexOperator;

int get_operator_arity(RegexOperator op);

int get_operator_precedence(RegexOperator op);

bool has_greater_precedence_than(RegexOperator op1, RegexOperator op2);

#endif
