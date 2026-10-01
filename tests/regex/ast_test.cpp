extern "C" {
#include "src/regex/ast.h"
}
#include <gtest/gtest.h>


TEST(AstTest, BuildAstFromSingleLiteral) {
    RegexNode root = build_ast_from_reverse_polish((char*) "a");

    EXPECT_EQ(root.type, LITERAL);
    EXPECT_EQ(root.value, 'a');
    EXPECT_EQ(root.left, nullptr);
    EXPECT_EQ(root.right, nullptr);
}

TEST(AstTest, BuildAstFromConcat) {
    RegexNode root = build_ast_from_reverse_polish((char*) "ab.");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '.');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, LITERAL);
    EXPECT_EQ(root.left->value, 'a');

    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->type, LITERAL);
    EXPECT_EQ(root.right->value, 'b');
}

TEST(AstTest, BuildAstFromUnion) {
    RegexNode root = build_ast_from_reverse_polish((char*) "ab|");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '|');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->value, 'a');

    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->value, 'b');
}

TEST(AstTest, BuildAstFromStar) {
    RegexNode root = build_ast_from_reverse_polish((char*) "a*");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '*');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, LITERAL);
    EXPECT_EQ(root.left->value, 'a');

    EXPECT_EQ(root.right, nullptr);
}

TEST(AstTest, BuildAstFromNestedExpression) {
    RegexNode root = build_ast_from_reverse_polish((char*) "ab.c|");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '|');

    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->type, LITERAL);
    EXPECT_EQ(root.right->value, 'c');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, OPERATION);
    EXPECT_EQ(root.left->value, '.');

    ASSERT_NE(root.left->left, nullptr);
    EXPECT_EQ(root.left->left->value, 'a');

    ASSERT_NE(root.left->right, nullptr);
    EXPECT_EQ(root.left->right->value, 'b');
}

TEST(AstTest, BuildAstFromDeeplyNestedExpression) {
    RegexNode root = build_ast_from_reverse_polish((char*) "ab*.c|");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '|');

    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->value, 'c');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, OPERATION);
    EXPECT_EQ(root.left->value, '.');

    ASSERT_NE(root.left->left, nullptr);
    EXPECT_EQ(root.left->left->value, 'a');

    ASSERT_NE(root.left->right, nullptr);
    EXPECT_EQ(root.left->right->type, OPERATION);
    EXPECT_EQ(root.left->right->value, '*');

    ASSERT_NE(root.left->right->left, nullptr);
    EXPECT_EQ(root.left->right->left->value, 'b');
    EXPECT_EQ(root.left->right->right, nullptr);
}

TEST(AstTest, BuildAstFromTripleConcatChain) {
    RegexNode root = build_ast_from_reverse_polish((char*) "ab.c.");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '.');

    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->value, 'c');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, OPERATION);
    EXPECT_EQ(root.left->value, '.');

    ASSERT_NE(root.left->left, nullptr);
    EXPECT_EQ(root.left->left->value, 'a');

    ASSERT_NE(root.left->right, nullptr);
    EXPECT_EQ(root.left->right->value, 'b');
}

TEST(AstTest, BuildAstFromUnionOfTwoConcats) {
    RegexNode root = build_ast_from_reverse_polish((char*) "ab.cd.|");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '|');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, OPERATION);
    EXPECT_EQ(root.left->value, '.');
    ASSERT_NE(root.left->left, nullptr);
    EXPECT_EQ(root.left->left->value, 'a');
    ASSERT_NE(root.left->right, nullptr);
    EXPECT_EQ(root.left->right->value, 'b');

    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->type, OPERATION);
    EXPECT_EQ(root.right->value, '.');
    ASSERT_NE(root.right->left, nullptr);
    EXPECT_EQ(root.right->left->value, 'c');
    ASSERT_NE(root.right->right, nullptr);
    EXPECT_EQ(root.right->right->value, 'd');
}

TEST(AstTest, BuildAstFromMixedOperators) {
    RegexNode root = build_ast_from_reverse_polish((char*) "a*bc|.");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '.');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, OPERATION);
    EXPECT_EQ(root.left->value, '*');
    ASSERT_NE(root.left->left, nullptr);
    EXPECT_EQ(root.left->left->value, 'a');
    EXPECT_EQ(root.left->right, nullptr);

    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->type, OPERATION);
    EXPECT_EQ(root.right->value, '|');
    ASSERT_NE(root.right->left, nullptr);
    EXPECT_EQ(root.right->left->value, 'b');
    ASSERT_NE(root.right->right, nullptr);
    EXPECT_EQ(root.right->right->value, 'c');
}

TEST(AstTest, BuildAstFromFourLevelChain) {
    RegexNode root = build_ast_from_reverse_polish((char*) "ab.c.d.");

    EXPECT_EQ(root.type, OPERATION);
    EXPECT_EQ(root.value, '.');
    ASSERT_NE(root.right, nullptr);
    EXPECT_EQ(root.right->value, 'd');

    ASSERT_NE(root.left, nullptr);
    EXPECT_EQ(root.left->type, OPERATION);
    EXPECT_EQ(root.left->value, '.');
    ASSERT_NE(root.left->right, nullptr);
    EXPECT_EQ(root.left->right->value, 'c');

    ASSERT_NE(root.left->left, nullptr);
    EXPECT_EQ(root.left->left->type, OPERATION);
    EXPECT_EQ(root.left->left->value, '.');
    ASSERT_NE(root.left->left->left, nullptr);
    EXPECT_EQ(root.left->left->left->value, 'a');
    ASSERT_NE(root.left->left->right, nullptr);
    EXPECT_EQ(root.left->left->right->value, 'b');
}
