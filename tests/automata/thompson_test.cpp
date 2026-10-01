#include <gtest/gtest.h>

extern "C" {
    #include "src/automata/automata.h"
    #include "src/automata/thompson.h"
    #include "src/regex/regex.h"
}


static Automaton build_literal_automaton(char symbol) {
    AutomatonState* start = state_new_alloc();
    AutomatonState* accept = state_new_alloc();
    AutomatonTransition transition = transition_new(false, symbol, accept);
    vector_push(&start->transitions, &transition);

    return (Automaton) { .start = start, .accept = accept };
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

TEST(ThompsonTest, StarCreatesNewStartAndAcceptStates) {
    Automaton aut1 = build_literal_automaton('a');

    Automaton result = thompson_apply_operator(&aut1, NULL, STAR);

    EXPECT_NE(result.start, aut1.start);
    EXPECT_NE(result.accept, aut1.accept);
}

TEST(ThompsonTest, StarNewStartHasTwoEpsilonTransitions) {
    Automaton aut1 = build_literal_automaton('a');

    Automaton result = thompson_apply_operator(&aut1, NULL, STAR);

    ASSERT_EQ(result.start->transitions.size, 2);
    expect_transition(&result.start->transitions, 0, true, '\0', aut1.start);
    expect_transition(&result.start->transitions, 1, true, '\0', result.accept);
}

TEST(ThompsonTest, StarOriginalAcceptGetsSameTwoEpsilonTransitions) {
    Automaton aut1 = build_literal_automaton('a');

    Automaton result = thompson_apply_operator(&aut1, NULL, STAR);

    ASSERT_EQ(aut1.accept->transitions.size, 2);
    expect_transition(&aut1.accept->transitions, 0, true, '\0', aut1.start);
    expect_transition(&aut1.accept->transitions, 1, true, '\0', result.accept);
}

TEST(ThompsonTest, StarNewAcceptHasNoOutgoingTransitions) {
    Automaton aut1 = build_literal_automaton('a');

    Automaton result = thompson_apply_operator(&aut1, NULL, STAR);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(ThompsonTest, StarLeavesOriginalStartUntouched) {
    Automaton aut1 = build_literal_automaton('a');

    thompson_apply_operator(&aut1, NULL, STAR);

    ASSERT_EQ(aut1.start->transitions.size, 1);
    expect_transition(&aut1.start->transitions, 0, false, 'a', aut1.accept);
}

TEST(ThompsonTest, ConcatKeepsOriginalStartAndAccept) {
    Automaton aut1 = build_literal_automaton('a');
    Automaton aut2 = build_literal_automaton('b');

    Automaton result = thompson_apply_operator(&aut1, &aut2, CONCAT);

    EXPECT_EQ(result.start, aut1.start);
    EXPECT_EQ(result.accept, aut2.accept);
}

TEST(ThompsonTest, ConcatAddsEpsilonFromAut1AcceptToAut2Start) {
    Automaton aut1 = build_literal_automaton('a');
    Automaton aut2 = build_literal_automaton('b');

    thompson_apply_operator(&aut1, &aut2, CONCAT);

    ASSERT_EQ(aut1.accept->transitions.size, 1);
    expect_transition(&aut1.accept->transitions, 0, true, '\0', aut2.start);
}

TEST(ThompsonTest, ConcatLeavesAut2StartUntouched) {
    Automaton aut1 = build_literal_automaton('a');
    Automaton aut2 = build_literal_automaton('b');

    thompson_apply_operator(&aut1, &aut2, CONCAT);

    ASSERT_EQ(aut2.start->transitions.size, 1);
    expect_transition(&aut2.start->transitions, 0, false, 'b', aut2.accept);
}

TEST(ThompsonTest, UnionCreatesNewStartAndAcceptStates) {
    Automaton aut1 = build_literal_automaton('a');
    Automaton aut2 = build_literal_automaton('b');

    Automaton result = thompson_apply_operator(&aut1, &aut2, UNION);

    EXPECT_NE(result.start, aut1.start);
    EXPECT_NE(result.start, aut2.start);
    EXPECT_NE(result.accept, aut1.accept);
    EXPECT_NE(result.accept, aut2.accept);
}

TEST(ThompsonTest, UnionNewStartHasEpsilonsToBothBranches) {
    Automaton aut1 = build_literal_automaton('a');
    Automaton aut2 = build_literal_automaton('b');

    Automaton result = thompson_apply_operator(&aut1, &aut2, UNION);

    ASSERT_EQ(result.start->transitions.size, 2);
    expect_transition(&result.start->transitions, 0, true, '\0', aut1.start);
    expect_transition(&result.start->transitions, 1, true, '\0', aut2.start);
}

TEST(ThompsonTest, UnionBothBranchAcceptsPointToNewAccept) {
    Automaton aut1 = build_literal_automaton('a');
    Automaton aut2 = build_literal_automaton('b');

    Automaton result = thompson_apply_operator(&aut1, &aut2, UNION);

    ASSERT_EQ(aut1.accept->transitions.size, 1);
    expect_transition(&aut1.accept->transitions, 0, true, '\0', result.accept);

    ASSERT_EQ(aut2.accept->transitions.size, 1);
    expect_transition(&aut2.accept->transitions, 0, true, '\0', result.accept);
}

TEST(ThompsonTest, UnionNewAcceptHasNoOutgoingTransitions) {
    Automaton aut1 = build_literal_automaton('a');
    Automaton aut2 = build_literal_automaton('b');

    Automaton result = thompson_apply_operator(&aut1, &aut2, UNION);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(ThompsonTest, ConcatWithComposedLeftOperandPreservesInternalStructure) {
    Automaton lit_a = build_literal_automaton('a');
    Automaton lit_b = build_literal_automaton('b');
    Automaton union_ab = thompson_apply_operator(&lit_a, &lit_b, UNION);
    Automaton lit_c = build_literal_automaton('c');
    Automaton aut1 = thompson_apply_operator(&union_ab, &lit_c, CONCAT);   // (a|b)c
    Automaton aut2 = build_literal_automaton('d');

    Automaton result = thompson_apply_operator(&aut1, &aut2, CONCAT);     // (a|b)cd

    EXPECT_EQ(result.start, aut1.start);
    EXPECT_EQ(result.accept, aut2.accept);

    ASSERT_EQ(aut1.accept->transitions.size, 1);
    expect_transition(&aut1.accept->transitions, 0, true, '\0', aut2.start);

    ASSERT_EQ(aut1.start->transitions.size, 2);
    expect_transition(&aut1.start->transitions, 0, true, '\0', lit_a.start);
    expect_transition(&aut1.start->transitions, 1, true, '\0', lit_b.start);

    ASSERT_EQ(lit_a.start->transitions.size, 1);
    AutomatonTransition* a_edge = transition_at(&lit_a.start->transitions, 0);
    EXPECT_FALSE(a_edge->is_epsilon);
    EXPECT_EQ(a_edge->symbol, 'a');
    EXPECT_EQ(a_edge->to, lit_a.accept);

    ASSERT_EQ(lit_a.accept->transitions.size, 1);
    expect_transition(&lit_a.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(lit_b.start->transitions.size, 1);
    AutomatonTransition* b_edge = transition_at(&lit_b.start->transitions, 0);
    EXPECT_FALSE(b_edge->is_epsilon);
    EXPECT_EQ(b_edge->symbol, 'b');
    EXPECT_EQ(b_edge->to, lit_b.accept);

    ASSERT_EQ(lit_b.accept->transitions.size, 1);
    expect_transition(&lit_b.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(union_ab.accept->transitions.size, 1);
    expect_transition(&union_ab.accept->transitions, 0, true, '\0', lit_c.start);

    ASSERT_EQ(lit_c.start->transitions.size, 1);
    AutomatonTransition* c_edge = transition_at(&lit_c.start->transitions, 0);
    EXPECT_FALSE(c_edge->is_epsilon);
    EXPECT_EQ(c_edge->symbol, 'c');
    EXPECT_EQ(c_edge->to, lit_c.accept);

    ASSERT_EQ(aut2.start->transitions.size, 1);
    AutomatonTransition* d_edge = transition_at(&aut2.start->transitions, 0);
    EXPECT_FALSE(d_edge->is_epsilon);
    EXPECT_EQ(d_edge->symbol, 'd');
    EXPECT_EQ(d_edge->to, aut2.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(ThompsonTest, ConcatWithBothOperandsComposedPreservesInternalStructure) {
    Automaton lit_a = build_literal_automaton('a');
    Automaton lit_b = build_literal_automaton('b');
    Automaton union_ab = thompson_apply_operator(&lit_a, &lit_b, UNION);

    Automaton lit_c = build_literal_automaton('c');
    Automaton lit_d = build_literal_automaton('d');
    Automaton union_cd = thompson_apply_operator(&lit_c, &lit_d, UNION);

    Automaton result = thompson_apply_operator(&union_ab, &union_cd, CONCAT);   // (a|b)(c|d)

    EXPECT_EQ(result.start, union_ab.start);
    EXPECT_EQ(result.accept, union_cd.accept);

    ASSERT_EQ(union_ab.accept->transitions.size, 1);
    expect_transition(&union_ab.accept->transitions, 0, true, '\0', union_cd.start);

    ASSERT_EQ(union_ab.start->transitions.size, 2);
    expect_transition(&union_ab.start->transitions, 0, true, '\0', lit_a.start);
    expect_transition(&union_ab.start->transitions, 1, true, '\0', lit_b.start);

    ASSERT_EQ(lit_a.start->transitions.size, 1);
    expect_transition(&lit_a.start->transitions, 0, false, 'a', lit_a.accept);
    ASSERT_EQ(lit_a.accept->transitions.size, 1);
    expect_transition(&lit_a.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(lit_b.start->transitions.size, 1);
    expect_transition(&lit_b.start->transitions, 0, false, 'b', lit_b.accept);
    ASSERT_EQ(lit_b.accept->transitions.size, 1);
    expect_transition(&lit_b.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(union_cd.start->transitions.size, 2);
    expect_transition(&union_cd.start->transitions, 0, true, '\0', lit_c.start);
    expect_transition(&union_cd.start->transitions, 1, true, '\0', lit_d.start);

    ASSERT_EQ(lit_c.start->transitions.size, 1);
    expect_transition(&lit_c.start->transitions, 0, false, 'c', lit_c.accept);
    ASSERT_EQ(lit_c.accept->transitions.size, 1);
    expect_transition(&lit_c.accept->transitions, 0, true, '\0', union_cd.accept);

    ASSERT_EQ(lit_d.start->transitions.size, 1);
    expect_transition(&lit_d.start->transitions, 0, false, 'd', lit_d.accept);
    ASSERT_EQ(lit_d.accept->transitions.size, 1);
    expect_transition(&lit_d.accept->transitions, 0, true, '\0', union_cd.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}

TEST(ThompsonTest, StarOfComposedOperandPreservesInternalStructure) {
    Automaton lit_a = build_literal_automaton('a');
    Automaton lit_b = build_literal_automaton('b');
    Automaton union_ab = thompson_apply_operator(&lit_a, &lit_b, UNION);
    Automaton lit_c = build_literal_automaton('c');
    Automaton aut1 = thompson_apply_operator(&union_ab, &lit_c, CONCAT);   // (a|b)c

    Automaton result = thompson_apply_operator(&aut1, NULL, STAR);        // ((a|b)c)*

    EXPECT_NE(result.start, aut1.start);
    EXPECT_NE(result.accept, aut1.accept);

    ASSERT_EQ(result.start->transitions.size, 2);
    expect_transition(&result.start->transitions, 0, true, '\0', aut1.start);
    expect_transition(&result.start->transitions, 1, true, '\0', result.accept);

    ASSERT_EQ(aut1.accept->transitions.size, 2);
    expect_transition(&aut1.accept->transitions, 0, true, '\0', aut1.start);
    expect_transition(&aut1.accept->transitions, 1, true, '\0', result.accept);

    ASSERT_EQ(aut1.start->transitions.size, 2);
    expect_transition(&aut1.start->transitions, 0, true, '\0', lit_a.start);
    expect_transition(&aut1.start->transitions, 1, true, '\0', lit_b.start);

    ASSERT_EQ(lit_a.start->transitions.size, 1);
    expect_transition(&lit_a.start->transitions, 0, false, 'a', lit_a.accept);
    ASSERT_EQ(lit_a.accept->transitions.size, 1);
    expect_transition(&lit_a.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(lit_b.start->transitions.size, 1);
    expect_transition(&lit_b.start->transitions, 0, false, 'b', lit_b.accept);
    ASSERT_EQ(lit_b.accept->transitions.size, 1);
    expect_transition(&lit_b.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(union_ab.accept->transitions.size, 1);
    expect_transition(&union_ab.accept->transitions, 0, true, '\0', lit_c.start);

    ASSERT_EQ(lit_c.start->transitions.size, 1);
    expect_transition(&lit_c.start->transitions, 0, false, 'c', lit_c.accept);
}

TEST(ThompsonTest, UnionWithBothOperandsComposedOfConcatAndUnionPreservesInternalStructure) {
    Automaton lit_a = build_literal_automaton('a');
    Automaton lit_b = build_literal_automaton('b');
    Automaton union_ab = thompson_apply_operator(&lit_a, &lit_b, UNION);
    Automaton lit_c = build_literal_automaton('c');
    Automaton concat_abc = thompson_apply_operator(&union_ab, &lit_c, CONCAT);   // (a|b)c

    Automaton lit_d = build_literal_automaton('d');
    Automaton lit_e = build_literal_automaton('e');
    Automaton union_de = thompson_apply_operator(&lit_d, &lit_e, UNION);
    Automaton lit_f = build_literal_automaton('f');
    Automaton concat_def = thompson_apply_operator(&union_de, &lit_f, CONCAT);   // (d|e)f

    Automaton result = thompson_apply_operator(&concat_abc, &concat_def, UNION); // (a|b)c | (d|e)f

    EXPECT_NE(result.start, concat_abc.start);
    EXPECT_NE(result.start, concat_def.start);
    EXPECT_NE(result.accept, concat_abc.accept);
    EXPECT_NE(result.accept, concat_def.accept);

    ASSERT_EQ(result.start->transitions.size, 2);
    expect_transition(&result.start->transitions, 0, true, '\0', concat_abc.start);
    expect_transition(&result.start->transitions, 1, true, '\0', concat_def.start);

    ASSERT_EQ(concat_abc.accept->transitions.size, 1);
    expect_transition(&concat_abc.accept->transitions, 0, true, '\0', result.accept);

    ASSERT_EQ(concat_def.accept->transitions.size, 1);
    expect_transition(&concat_def.accept->transitions, 0, true, '\0', result.accept);

    // ramo (a|b)c, intocado pela UNION externa
    ASSERT_EQ(concat_abc.start->transitions.size, 2);
    expect_transition(&concat_abc.start->transitions, 0, true, '\0', lit_a.start);
    expect_transition(&concat_abc.start->transitions, 1, true, '\0', lit_b.start);

    ASSERT_EQ(lit_a.start->transitions.size, 1);
    expect_transition(&lit_a.start->transitions, 0, false, 'a', lit_a.accept);
    ASSERT_EQ(lit_a.accept->transitions.size, 1);
    expect_transition(&lit_a.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(lit_b.start->transitions.size, 1);
    expect_transition(&lit_b.start->transitions, 0, false, 'b', lit_b.accept);
    ASSERT_EQ(lit_b.accept->transitions.size, 1);
    expect_transition(&lit_b.accept->transitions, 0, true, '\0', union_ab.accept);

    ASSERT_EQ(union_ab.accept->transitions.size, 1);
    expect_transition(&union_ab.accept->transitions, 0, true, '\0', lit_c.start);

    ASSERT_EQ(lit_c.start->transitions.size, 1);
    expect_transition(&lit_c.start->transitions, 0, false, 'c', lit_c.accept);

    // ramo (d|e)f, intocado pela UNION externa
    ASSERT_EQ(concat_def.start->transitions.size, 2);
    expect_transition(&concat_def.start->transitions, 0, true, '\0', lit_d.start);
    expect_transition(&concat_def.start->transitions, 1, true, '\0', lit_e.start);

    ASSERT_EQ(lit_d.start->transitions.size, 1);
    expect_transition(&lit_d.start->transitions, 0, false, 'd', lit_d.accept);
    ASSERT_EQ(lit_d.accept->transitions.size, 1);
    expect_transition(&lit_d.accept->transitions, 0, true, '\0', union_de.accept);

    ASSERT_EQ(lit_e.start->transitions.size, 1);
    expect_transition(&lit_e.start->transitions, 0, false, 'e', lit_e.accept);
    ASSERT_EQ(lit_e.accept->transitions.size, 1);
    expect_transition(&lit_e.accept->transitions, 0, true, '\0', union_de.accept);

    ASSERT_EQ(union_de.accept->transitions.size, 1);
    expect_transition(&union_de.accept->transitions, 0, true, '\0', lit_f.start);

    ASSERT_EQ(lit_f.start->transitions.size, 1);
    expect_transition(&lit_f.start->transitions, 0, false, 'f', lit_f.accept);

    EXPECT_EQ(result.accept->transitions.size, 0);
}
