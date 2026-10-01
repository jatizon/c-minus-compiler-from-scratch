extern "C" {
#include "src/regex/reverse_polish.h"
#include "src/helpers/string.h"
}
#include <cstdlib>
#include <gtest/gtest.h>


TEST(PolishNotationTest, IsParenthesisTrueForOpenAndClose) {
    EXPECT_TRUE(is_parenthesis('('));
    EXPECT_TRUE(is_parenthesis(')'));
}

TEST(PolishNotationTest, IsParenthesisFalseForOther) {
    EXPECT_FALSE(is_parenthesis('a'));
    EXPECT_FALSE(is_parenthesis('*'));
    EXPECT_FALSE(is_parenthesis('.'));
}

TEST(PolishNotationTest, IsLiteralTrueForLettersBothCases) {
    EXPECT_TRUE(is_literal('a'));
    EXPECT_TRUE(is_literal('z'));
    EXPECT_TRUE(is_literal('A'));
    EXPECT_TRUE(is_literal('Z'));
}

TEST(PolishNotationTest, IsLiteralFalseForDigitsAndSymbols) {
    EXPECT_FALSE(is_literal('0'));
    EXPECT_FALSE(is_literal('*'));
    EXPECT_FALSE(is_literal('|'));
    EXPECT_FALSE(is_literal('('));
}

TEST(PolishNotationTest, CanStartExpressionTrueForLiteralAndOpenParen) {
    EXPECT_TRUE(can_start_expression('a'));
    EXPECT_TRUE(can_start_expression('('));
}

TEST(PolishNotationTest, CanStartExpressionFalseForOperatorsAndCloseParen) {
    EXPECT_FALSE(can_start_expression('*'));
    EXPECT_FALSE(can_start_expression('|'));
    EXPECT_FALSE(can_start_expression('.'));
    EXPECT_FALSE(can_start_expression(')'));
}

TEST(PolishNotationTest, CanEndExpressionTrueForLiteralCloseParenAndStar) {
    EXPECT_TRUE(can_end_expression('a'));
    EXPECT_TRUE(can_end_expression(')'));
    EXPECT_TRUE(can_end_expression('*'));
}

TEST(PolishNotationTest, CanEndExpressionFalseForOpenParenAndUnion) {
    EXPECT_FALSE(can_end_expression('('));
    EXPECT_FALSE(can_end_expression('|'));
}

TEST(PolishNotationTest, ImplicitConcatBetweenTwoLiterals) {
    EXPECT_TRUE(is_implicit_concat_between('a', 'b'));
}

TEST(PolishNotationTest, ImplicitConcatBetweenStarAndLiteral) {
    EXPECT_TRUE(is_implicit_concat_between('*', 'a'));
}

TEST(PolishNotationTest, ImplicitConcatBetweenUnionAndLiteral) {
    EXPECT_FALSE(is_implicit_concat_between('|', 'a'));
}

TEST(PolishNotationTest, GetOperatorArityForEachOperator) {
    struct { RegexOperator op; int expected_arity; } cases[] = {
        { STAR, 1 },
        { UNION, 2 },
        { CONCAT, 2 },
        { LEFT_PARENTHESIS, 0 },
        { RIGHT_PARENTHESIS, 0 },
    };

    for (auto& c : cases) {
        EXPECT_EQ(get_operator_arity(c.op), c.expected_arity);
    }
}

TEST(PolishNotationTest, GetOperatorPrecedenceForEachOperator) {
    struct { RegexOperator op; int expected_precedence; } cases[] = {
        { STAR, 2 },
        { CONCAT, 1 },
        { UNION, 0 },
        { LEFT_PARENTHESIS, -1 },
        { RIGHT_PARENTHESIS, -1 },
    };

    for (auto& c : cases) {
        EXPECT_EQ(get_operator_precedence(c.op), c.expected_precedence);
    }
}

TEST(PolishNotationTest, StarHasGreaterPrecedenceThanConcat) {
    EXPECT_TRUE(has_greater_precedence_than((RegexOperator) '*', (RegexOperator) '.'));
}

TEST(PolishNotationTest, ConcatHasGreaterPrecedenceThanUnion) {
    EXPECT_TRUE(has_greater_precedence_than((RegexOperator) '.', (RegexOperator) '|'));
}

TEST(PolishNotationTest, UnionDoesNotHaveGreaterPrecedenceThanStar) {
    EXPECT_FALSE(has_greater_precedence_than((RegexOperator) '|', (RegexOperator) '*'));
}

TEST(PolishNotationTest, RemovesAllSpaces) {
    char* result = remove_spaces((char*) "a b c");
    EXPECT_STREQ(result, "abc");
    free(result);
}

TEST(PolishNotationTest, NoSpacesStaysUnchanged) {
    char* result = remove_spaces((char*) "abc");
    EXPECT_STREQ(result, "abc");
    free(result);
}

TEST(PolishNotationTest, InsertsConcatBetweenAdjacentLiterals) {
    char* result = resolve_implicit_concats((char*) "ab");
    EXPECT_STREQ(result, "a.b");
    free(result);
}

TEST(PolishNotationTest, NoConcatBeforeUnion) {
    char* result = resolve_implicit_concats((char*) "a|b");
    EXPECT_STREQ(result, "a|b");
    free(result);
}

TEST(PolishNotationTest, InsertsConcatAfterCloseParen) {
    char* result = resolve_implicit_concats((char*) "(a)b");
    EXPECT_STREQ(result, "(a).b");
    free(result);
}

TEST(PolishNotationTest, RemovesSpacesAndInsertsConcats) {
    char* result = pre_process_regex((char*) "a b");
    EXPECT_STREQ(result, "a.b");
    free(result);
}

TEST(PolishNotationTest, BuildReversePolishForMultipleExpressions) {
    struct { const char* input; const char* expected; } cases[] = {
        {"a.b", "ab."},
        {"a|b", "ab|"},
        {"a.b|c", "ab.c|"},
        {"a.(b|c)", "abc|."},
        {"a.b*", "ab*."},
        {"a*.b", "a*b."},
    };

    for (auto& c : cases) {
        char* result = build_reverse_polish_from_regex((char*) c.input);
        EXPECT_STREQ(result, c.expected) << "input: " << c.input;
        free(result);
    }
}

TEST(PolishNotationTest, BuildReversePolishForTrivialComplexExpressions) {
    struct { const char* input; const char* expected; } cases[] = {
        {"(a.b).c", "ab.c."},
        {"a.b.c", "ab.c."},
        {"a*.b*.c", "a*b*.c."},
        {"(a|b).(c|d)", "ab|cd|."},
        {"((a|b)*.c)|(a.b)", "ab|*c.ab.|"},
        {"a*.b|c", "a*b.c|"},
        {"(a.b)*|c", "ab.*c|"},
        {"a|b.c*", "abc*.|"},
        {"(a|b)*.c", "ab|*c."},
        {"a.b*|c.d", "ab*.cd.|"},
    };

    for (auto& c : cases) {
        char* result = build_reverse_polish_from_regex((char*) c.input);
        EXPECT_STREQ(result, c.expected) << "input: " << c.input;
        free(result);
    }
}

TEST(PolishNotationTest, BuildReversePolishForComplexExpressions) {
    struct { const char* input; const char* expected; } cases[] = {
        {"((a|b)*.c|a.b)*.(a|b.c)", "ab|*c.ab.|*abc.|."},
        {"(a.b|c.d)*.e", "ab.cd.|*e."},
        {"a.(b.c|d)*", "abc.d|*."},
        {"(a|b.c)*.(d|e)", "abc.|*de|."},
        {"((a.b)*|c).d", "ab.*c|d."},
        {"(a|b)*.(c|d)*", "ab|*cd|*."},
        {"a.(b|c.(d|e))", "abcde|.|."},
        {"(a.(b|c))*.d", "abc|.*d."},
        {"((a|b).c)*|d.e", "ab|c.*de.|"},
        {"(a.b.c)*|(d|e).f", "ab.c.*de|f.|"},
    };

    for (auto& c : cases) {
        char* result = build_reverse_polish_from_regex((char*) c.input);
        EXPECT_STREQ(result, c.expected) << "input: " << c.input;
        free(result);
    }
}
