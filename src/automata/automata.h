#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "src/data_structures/vector.h"
#include <stdbool.h>


typedef struct AutomatonState {
    Vector transitions;
} AutomatonState;

typedef struct AutomatonTransition {
    bool is_epsilon;
    char symbol;
    AutomatonState* to;
} AutomatonTransition;

typedef struct Automaton {
    AutomatonState* start;
    AutomatonState* accept;
} Automaton;

AutomatonState state_new();

AutomatonState* state_new_alloc();

AutomatonTransition transition_new(bool is_epsilon, char symbol, AutomatonState* to);

Automaton automaton_new(AutomatonState* start, AutomatonState* accept);

Automaton automaton_new_literal(char symbol);

void automaton_state_add_transition(AutomatonState* state, AutomatonTransition* transition);

Automaton build_automaton_from_reverse_polish(char* regex);

#endif
