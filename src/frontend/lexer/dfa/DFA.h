#ifndef DFA_H
#define DFA_H

#include <array>

// Transition states for Deterministic Finite Automaton
enum Transitions {
    REJECT = 0,
    INTEGER,
    REAL,
    OPERATOR,
    IDENT,
    UNKNOWN,
    SPACE,
};

// The Deterministic Finite Automaton
static constexpr std::array<std::array<int, 7>, 7> transitionTbl = {
                {{0, INTEGER,  REAL,  OPERATOR,  IDENT,  UNKNOWN,  SPACE},  // is just a placeholder
/* Trans. 1 */   {INTEGER,  INTEGER,  REAL,   REJECT,   REJECT,  REJECT,  REJECT},
/* Trans. 2 */   {REAL,      REAL,  UNKNOWN, REJECT,   REJECT,  REJECT,  REJECT},
/* Trans. 3 */   {OPERATOR,  REJECT, REJECT,  REJECT,   IDENT,  REJECT,  REJECT},
/* Trans. 4 */   {IDENT,    IDENT,   REJECT,  IDENT,   IDENT,  REJECT,  REJECT},
/* Trans. 5 */   {UNKNOWN,  UNKNOWN, UNKNOWN, UNKNOWN,  UNKNOWN, UNKNOWN, REJECT},
/* Trans. 6 */   {SPACE,     REJECT, REJECT,  REJECT,   REJECT,  REJECT,  REJECT}}
};

#endif // DFA_H
