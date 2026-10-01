#ifndef THOMPSON_H
#define THOMPSON_H

#include "src/automata/automata.h"
#include "src/regex/regex.h"

Automaton thompson_apply_operator(Automaton* aut1, Automaton* aut2, RegexOperator op);

#endif
