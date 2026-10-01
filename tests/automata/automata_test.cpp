#include <gtest/gtest.h>

extern "C" {
    #include "src/automata/automata.h"
}


TEST(AutomataTest, StateNewHasEmptyTransitions) {
    AutomatonState state = state_new();

    EXPECT_EQ(state.transitions.size, 0);
}

TEST(AutomataTest, StateNewAllocReturnsEmptyTransitions) {
    AutomatonState* state = state_new_alloc();

    EXPECT_EQ(state->transitions.size, 0);
}

TEST(AutomataTest, TransitionNewSetsEpsilonFields) {
    AutomatonState* to = state_new_alloc();

    AutomatonTransition transition = transition_new(true, '\0', to);

    EXPECT_TRUE(transition.is_epsilon);
    EXPECT_EQ(transition.symbol, '\0');
    EXPECT_EQ(transition.to, to);
}

TEST(AutomataTest, TransitionNewSetsSymbolFields) {
    AutomatonState* to = state_new_alloc();

    AutomatonTransition transition = transition_new(false, 'a', to);

    EXPECT_FALSE(transition.is_epsilon);
    EXPECT_EQ(transition.symbol, 'a');
    EXPECT_EQ(transition.to, to);
}
