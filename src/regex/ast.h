#ifndef AST_H
#define AST_H

typedef enum RegexNodeType {
    LITERAL,
    OPERATION,
} RegexNodeType;

typedef struct RegexNode {
    RegexNodeType type;
    char value;
    struct RegexNode* left;
    struct RegexNode* right;
} RegexNode;

RegexNode node_new_value(RegexNodeType type, char value, RegexNode* left, RegexNode* right);

RegexNode* node_new_alloc(RegexNodeType type, char value, RegexNode* left, RegexNode* right);

RegexNode build_ast_from_reverse_polish(char* regex);

#endif
