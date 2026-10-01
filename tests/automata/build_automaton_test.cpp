#include <gtest/gtest.h>

extern "C" {
    #include "src/automata/automata.h"
}


static AutomatonTransition* transition_at(Vector* transitions, size_t index) {
    return (AutomatonTransition*) vector_get_element_ptr(transitions, index);
}

static void expect_transition(Vector* transitions, size_t index, bool is_epsilon, char symbol, AutomatonState* to) {
    AutomatonTransition* transition = transition_at(transitions, index);
    EXPECT_EQ(transition->is_epsilon, is_epsilon);
    EXPECT_EQ(transition->symbol, symbol);
    EXPECT_EQ(transition->to, to);
}

TEST(BuildAutomatonTest, SingleLiteral) {
    Automaton result = build_automaton_from_reverse_polish((char*) "a");

    ASSERT_EQ(result.start->transitions.size, 1);
    expect_transition(&result.start->transitions, 0, false, 'a', result.accept);
    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(BuildAutomatonTest, Concat) {
    Automaton result = build_automaton_from_reverse_polish((char*) "ab.");

    ASSERT_EQ(result.start->transitions.size, 1);
    AutomatonTransition* to_s1 = transition_at(&result.start->transitions, 0);
    EXPECT_FALSE(to_s1->is_epsilon);
    EXPECT_EQ(to_s1->symbol, 'a');
    AutomatonState* s1 = to_s1->to;

    ASSERT_EQ(s1->transitions.size, 1);
    AutomatonTransition* to_s2 = transition_at(&s1->transitions, 0);
    EXPECT_TRUE(to_s2->is_epsilon);
    AutomatonState* s2 = to_s2->to;

    ASSERT_EQ(s2->transitions.size, 1);
    expect_transition(&s2->transitions, 0, false, 'b', result.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(BuildAutomatonTest, Union) {
    Automaton result = build_automaton_from_reverse_polish((char*) "ab|");

    ASSERT_EQ(result.start->transitions.size, 2);
    AutomatonTransition* branch_a = transition_at(&result.start->transitions, 0);
    AutomatonTransition* branch_b = transition_at(&result.start->transitions, 1);
    EXPECT_TRUE(branch_a->is_epsilon);
    EXPECT_TRUE(branch_b->is_epsilon);
    AutomatonState* a1 = branch_a->to;
    AutomatonState* b1 = branch_b->to;

    ASSERT_EQ(a1->transitions.size, 1);
    AutomatonTransition* a_edge = transition_at(&a1->transitions, 0);
    EXPECT_FALSE(a_edge->is_epsilon);
    EXPECT_EQ(a_edge->symbol, 'a');
    AutomatonState* a2 = a_edge->to;
    ASSERT_EQ(a2->transitions.size, 1);
    expect_transition(&a2->transitions, 0, true, '\0', result.accept);

    ASSERT_EQ(b1->transitions.size, 1);
    AutomatonTransition* b_edge = transition_at(&b1->transitions, 0);
    EXPECT_FALSE(b_edge->is_epsilon);
    EXPECT_EQ(b_edge->symbol, 'b');
    AutomatonState* b2 = b_edge->to;
    ASSERT_EQ(b2->transitions.size, 1);
    expect_transition(&b2->transitions, 0, true, '\0', result.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(BuildAutomatonTest, Star) {
    Automaton result = build_automaton_from_reverse_polish((char*) "a*");

    ASSERT_EQ(result.start->transitions.size, 2);
    AutomatonTransition* to_a1 = transition_at(&result.start->transitions, 0);
    expect_transition(&result.start->transitions, 1, true, '\0', result.accept);
    EXPECT_TRUE(to_a1->is_epsilon);
    AutomatonState* a1 = to_a1->to;

    ASSERT_EQ(a1->transitions.size, 1);
    AutomatonTransition* a_edge = transition_at(&a1->transitions, 0);
    EXPECT_FALSE(a_edge->is_epsilon);
    EXPECT_EQ(a_edge->symbol, 'a');
    AutomatonState* a2 = a_edge->to;

    ASSERT_EQ(a2->transitions.size, 2);
    expect_transition(&a2->transitions, 0, true, '\0', a1);
    expect_transition(&a2->transitions, 1, true, '\0', result.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(BuildAutomatonTest, ConcatOfUnion) {
    Automaton result = build_automaton_from_reverse_polish((char*) "ab|c.");

    ASSERT_EQ(result.start->transitions.size, 2);
    AutomatonState* a1 = transition_at(&result.start->transitions, 0)->to;
    AutomatonState* b1 = transition_at(&result.start->transitions, 1)->to;

    ASSERT_EQ(a1->transitions.size, 1);
    AutomatonState* a2 = transition_at(&a1->transitions, 0)->to;
    ASSERT_EQ(b1->transitions.size, 1);
    AutomatonState* b2 = transition_at(&b1->transitions, 0)->to;

    ASSERT_EQ(a2->transitions.size, 1);
    AutomatonState* union_accept = transition_at(&a2->transitions, 0)->to;
    expect_transition(&a2->transitions, 0, true, '\0', union_accept);

    ASSERT_EQ(b2->transitions.size, 1);
    expect_transition(&b2->transitions, 0, true, '\0', union_accept);

    ASSERT_EQ(union_accept->transitions.size, 1);
    AutomatonTransition* to_c1 = transition_at(&union_accept->transitions, 0);
    EXPECT_TRUE(to_c1->is_epsilon);
    AutomatonState* c1 = to_c1->to;

    ASSERT_EQ(c1->transitions.size, 1);
    expect_transition(&c1->transitions, 0, false, 'c', result.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(BuildAutomatonTest, StarOfConcat) {
    Automaton result = build_automaton_from_reverse_polish((char*) "ab.*");

    ASSERT_EQ(result.start->transitions.size, 2);
    AutomatonTransition* to_a1 = transition_at(&result.start->transitions, 0);
    expect_transition(&result.start->transitions, 1, true, '\0', result.accept);
    AutomatonState* a1 = to_a1->to;

    ASSERT_EQ(a1->transitions.size, 1);
    AutomatonState* a2 = transition_at(&a1->transitions, 0)->to;

    ASSERT_EQ(a2->transitions.size, 1);
    AutomatonTransition* to_b1 = transition_at(&a2->transitions, 0);
    EXPECT_TRUE(to_b1->is_epsilon);
    AutomatonState* b1 = to_b1->to;

    ASSERT_EQ(b1->transitions.size, 1);
    AutomatonTransition* b_edge = transition_at(&b1->transitions, 0);
    EXPECT_FALSE(b_edge->is_epsilon);
    EXPECT_EQ(b_edge->symbol, 'b');
    AutomatonState* b2 = b_edge->to;

    ASSERT_EQ(b2->transitions.size, 2);
    expect_transition(&b2->transitions, 0, true, '\0', a1);
    expect_transition(&b2->transitions, 1, true, '\0', result.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}
