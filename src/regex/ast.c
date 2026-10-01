#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "src/regex/ast.h"
#include "src/data_structures/vector.h"
#include "src/data_structures/stack.h"
#include "src/helpers/string.h"
#include "src/regex/regex.h"


RegexNode node_new_value(RegexNodeType type, char value, RegexNode* left, RegexNode* right) {
    return (RegexNode) {
        .type = type,
        .value = value,
        .left = left,
        .right = right,
    };
}

RegexNode* node_new_alloc(RegexNodeType type, char value, RegexNode* left, RegexNode* right) {
    RegexNode* node = malloc(sizeof(RegexNode));
    *node = node_new_value(type, value, left, right);
    return node;
}

RegexNode build_ast_from_reverse_polish(char* regex) {
    char* treated_regex = remove_spaces(regex);

    Stack nodes = stack_new(sizeof(RegexNode*));

    for(char* c = treated_regex; *c; ++ c) {
        if (is_literal(*c)) {
            RegexNode* node = node_new_alloc(LITERAL, *c, NULL, NULL);
            stack_push(&nodes, &node);
            continue;
        }

        int num_operands = get_operator_arity(*c);

        RegexNode *left, *right;
        if (num_operands == 1) {
            left = *(RegexNode**) stack_pop(&nodes);
            right = NULL;
        }
        else {
            right = *(RegexNode**) stack_pop(&nodes);
            left = *(RegexNode**) stack_pop(&nodes);
        }

        RegexNode* node = node_new_alloc(OPERATION, *c, left, right);
        stack_push(&nodes, &node);
    }

    RegexNode* root = *(RegexNode**) stack_pop(&nodes);
    return *root;
}
