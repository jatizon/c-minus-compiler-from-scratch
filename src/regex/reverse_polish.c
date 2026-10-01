#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "src/regex/reverse_polish.h"
#include "src/data_structures/vector.h"
#include "src/data_structures/stack.h"
#include "src/helpers/string.h"


bool can_start_expression(char c) {
    return (
        is_literal(c) ||
        (c == LEFT_PARENTHESIS)
    );
}

bool can_end_expression(char c) {
    return (
        is_literal(c) ||
        (c == RIGHT_PARENTHESIS) ||
        (c == STAR)
    );
}

bool is_implicit_concat_between(char c1, char c2) {
    return can_end_expression(c1) && can_start_expression(c2);
}

char* resolve_implicit_concats(char* input) {
    Vector output = vector_new(sizeof(char));

    while (*input) {
        vector_push(&output, input);
        if (
            *(input + 1) &&
            is_implicit_concat_between(*input, *(input + 1))
        ) {
            char concat = CONCAT;
            vector_push(&output, &concat);
        }
        ++ input;
    }

    char null_terminator = '\0';
    vector_push(&output, &null_terminator);

    size_t output_size = output.size;
    char* result = malloc(output_size);
    memcpy(result, vector_get_data(&output), output_size);

    vector_free(&output);

    return result;
}

char* pre_process_regex(char* regex) {
    char* without_spaces = remove_spaces(regex);
    char* treated_regex = resolve_implicit_concats(without_spaces);

    free(without_spaces);

    return treated_regex;
}

char* build_reverse_polish_from_regex(char* regex) {
    char* treated_regex = pre_process_regex(regex);

    Vector result = vector_new(sizeof(char));
    Stack operators = stack_new(sizeof(char));

    for (char* c = treated_regex; *c; ++c) {
        if (is_literal(*c)) {
            vector_push(&result, c);
            continue;
        }

        if (*c == LEFT_PARENTHESIS) {
            stack_push(&operators, c);
            continue;
        }

        if (*c == RIGHT_PARENTHESIS) {
            while (*(char*) stack_top(&operators) != LEFT_PARENTHESIS) {
                char* top = stack_pop(&operators);
                vector_push(&result, top);
            }
            
            stack_pop(&operators);
            continue;
        }

        while (
            !stack_empty(&operators) && 
            *(char*) stack_top(&operators) != LEFT_PARENTHESIS &&
            !has_greater_precedence_than(*c, *(char*) stack_top(&operators)
        )) {
            char* top = stack_pop(&operators);
            vector_push(&result, top);
        }

        stack_push(&operators, c);
    }

    while (!stack_empty(&operators)) {
        char* top = stack_pop(&operators);
        vector_push(&result, top);
    }

    char* output_str = (char*) malloc((vector_get_size(&result) + 1) * sizeof(char));

    char* c = output_str;
    for (size_t i = 0; i < vector_get_size(&result); ++ i) {
        char* top = vector_get_element_ptr(&result, i);
        *c = *top;
        ++ c;
    }
    output_str[vector_get_size(&result)] = '\0';

        

    return output_str;
}