#include "src/automata/automata.h"
#include "src/automata/thompson.h"
#include "src/regex/regex.h"
#include "src/data_structures/stack.h"
#include "src/helpers/string.h"


Automaton thompson_apply_operator(Automaton* aut1, Automaton* aut2, RegexOperator op) {
    switch (op) {
        case STAR: {
            AutomatonState* start_state = state_new_alloc();
            AutomatonState* accept_state = state_new_alloc();

            AutomatonTransition to_aut1 = transition_new(true, '\0', aut1->start);
            AutomatonTransition to_accept = transition_new(true, '\0', accept_state);
            vector_push(&start_state->transitions, &to_aut1);
            vector_push(&aut1->accept->transitions, &to_aut1);
            vector_push(&aut1->accept->transitions, &to_accept);
            vector_push(&start_state->transitions, &to_accept);

            return (Automaton) {
                .start = start_state,
                .accept = accept_state,
            };
        }

        case CONCAT: {
            AutomatonTransition to_aut2 = transition_new(true, '\0', aut2->start);
            vector_push(&aut1->accept->transitions, &to_aut2);

            return (Automaton) {
                .start = aut1->start,
                .accept = aut2->accept,
            };
        }

        case UNION: {
            AutomatonState* start_state = state_new_alloc();
            AutomatonState* accept_state = state_new_alloc();

            AutomatonTransition to_aut1 = transition_new(true, '\0', aut1->start);
            AutomatonTransition to_aut2 = transition_new(true, '\0', aut2->start);
            vector_push(&start_state->transitions, &to_aut1);
            vector_push(&start_state->transitions, &to_aut2);

            AutomatonTransition to_accept1 = transition_new(true, '\0', accept_state);
            AutomatonTransition to_accept2 = transition_new(true, '\0', accept_state);
            vector_push(&aut1->accept->transitions, &to_accept1);
            vector_push(&aut2->accept->transitions, &to_accept2);


            return (Automaton) {
                .start = start_state,
                .accept = accept_state,
            };
        }
    }
}

Automaton build_automaton_from_reverse_polish(char* regex) {
    char* treated_regex = remove_spaces(regex);

    Stack automata = stack_new(sizeof(Automaton));
    for(char* c = treated_regex; *c; ++ c) {
        if (is_literal(*c)) {
            Automaton literal = automaton_new_literal(*c);
            stack_push(&automata, &literal);
            continue;
        }

        int num_operands = get_operator_arity(*c);

        Automaton *left, *right;
        if (num_operands == 1) {
            left = stack_pop(&automata);
            right = NULL;
        }
        else {
            right = stack_pop(&automata);
            left = stack_pop(&automata);
        }

        Automaton new_automaton = thompson_apply_operator(left, right, *c);
        stack_push(&automata, &new_automaton);
    }

    Automaton* result = stack_pop(&automata);
    return *result;
}