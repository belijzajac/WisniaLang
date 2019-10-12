#ifndef DFA_H
#define DFA_H

#include <array>

// Transition states for the Deterministic Finite Automaton
enum Transitions {
    MAIN = 0,
    INTEGER,
    FLOAT,
    OPERATOR,
    IDENT,
    UNKNOWN,
    SPACE,
};

// The Deterministic Finite Automaton
static constexpr std::array<std::array<int, 7>, 7> transitionTbl = {
/* Placeholder */ {{MAIN,    INTEGER, FLOAT,   OPERATOR, IDENT,   UNKNOWN, SPACE},

/* Trans. 1 */    {INTEGER,  INTEGER, FLOAT,   MAIN,     MAIN,    MAIN,    MAIN},
/* Trans. 2 */    {FLOAT,    FLOAT,   UNKNOWN, MAIN,     MAIN,    MAIN,    MAIN},
/* Trans. 3 */    {OPERATOR, MAIN,    MAIN,    MAIN,     IDENT,   MAIN,    MAIN},
/* Trans. 4 */    {IDENT,    IDENT,   MAIN,    IDENT,    IDENT,   MAIN,    MAIN},
/* Trans. 5 */    {UNKNOWN,  UNKNOWN, UNKNOWN, UNKNOWN,  UNKNOWN, UNKNOWN, MAIN},
/* Trans. 6 */    {SPACE,    MAIN,    MAIN,    MAIN,     MAIN,    MAIN,    MAIN}}
};

#endif // DFA_H
