#include <stdlib.h>
#include <stdbool.h>
#include "src/automata/automata.h"
#include "src/data_structures/vector.h"


AutomatonState state_new() {
    return (AutomatonState) {
        .transitions = vector_new(sizeof(AutomatonTransition)),
    };
}

AutomatonState* state_new_alloc() {
    AutomatonState* state = malloc(sizeof(AutomatonState));
    *state = state_new();
    return state;
}

AutomatonTransition transition_new(bool is_epsilon, char symbol, AutomatonState* to) {
    return (AutomatonTransition) {
        .is_epsilon = is_epsilon,
        .symbol = symbol,
        .to = to,
    };
}

Automaton automaton_new(AutomatonState* start, AutomatonState* accept) {
    return (Automaton) {
        .start = start,
        .accept = accept,
    };
}

Automaton automaton_new_literal(char symbol) {
    AutomatonState* start = state_new_alloc();
    AutomatonState* accept = state_new_alloc();
    AutomatonTransition transition = transition_new(false, symbol, accept);
    vector_push(&start->transitions, &transition);

    return automaton_new(start, accept);
}


