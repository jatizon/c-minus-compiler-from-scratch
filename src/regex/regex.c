#include "src/regex/regex.h"


int get_operator_arity(RegexOperator op) {
    switch (op) {
        case STAR:
            return 1;
        case UNION:
            return 2;
        case CONCAT:
            return 2;
        default:
            return 0;
    }
}

int get_operator_precedence(RegexOperator op) {
    switch (op) {
        case STAR:
            return 2;
        case CONCAT:
            return 1;
        case UNION:
            return 0;
        default:
            return -1;
    }
}

bool has_greater_precedence_than(RegexOperator op1, RegexOperator op2) {
    return get_operator_precedence(op1) > get_operator_precedence(op2);
}
