#include <stdlib.h>
#include <string.h>
#include "src/helpers/string.h"
#include "src/data_structures/vector.h"


char* string_from_char(char c) {
    char* string = (char*) malloc(2 * sizeof(char));

    string[0] = c;
    string[1] = '\0';

    return string;
}

bool is_parenthesis(char c) {
    return (
        c == '(' ||
        c == ')'
    );
}

bool is_literal(char c) {
    return (
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z')
    );
}

char* remove_spaces(char* input) {
    Vector output = vector_new(sizeof(char));

    while (*input) {
        if (*input != ' ')
            vector_push(&output, input);
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
