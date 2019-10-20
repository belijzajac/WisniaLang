#ifndef DFA_H
#define DFA_H

#include <array>

// Transition states for the Deterministic Finite Automaton
enum Transitions {
    MAIN = 0,     // Start state
    INT,          // Integer number
    FLT,          // Float number

    OP_SINGLE,    // e.g. (, }, >, =
    OP_L_AND,     // e.g. &&
    OP_L_AND_ST,  // e.g. &
    OP_L_OR,      // e.g. ||
    OP_L_OR_ST,   // e.g. |

    OP_CMP_ST,    // e.g. !, <, >, =
    OP_CMP_EN,    // e.g. !=, <=, >=, ==

    OP_ARROW,     // e.g. ->
    OP_PP,        // e.g. ++

    IDENT,
    STR_ST,       // "\""
    STR_EN,       // Complete the string
    ERR,          // Erroneous
    SPACE,        // Whitespace

    __LAST        // Just to mark the index of `Transitions`
};

// The Deterministic Finite Automaton
static constexpr std::array<std::array<int, __LAST>, __LAST> transitionTbl = {
/* Tr */    {{MAIN,       INT,    FLT,    OP_SINGLE, OP_L_AND, OP_L_AND_ST, OP_L_OR, OP_L_OR_ST, OP_CMP_ST, OP_CMP_EN, OP_ARROW, OP_PP,  IDENT,  STR_ST, STR_EN, ERR,  SPACE},

/* 1) */    {INT,         INT,    FLT,    MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},
/* 2) */    {FLT,         FLT,    ERR,    MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},

/* 3) */    {OP_SINGLE,   MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},
/* 4) */    {OP_L_AND,    MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},
/* 5) */    {OP_L_AND_ST, MAIN,   MAIN,   MAIN,      OP_L_AND, OP_L_AND,    MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},
/* 6) */    {OP_L_OR,     MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},
/* 7) */    {OP_L_OR_ST,  MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        OP_L_OR, OP_L_OR,    MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},

/* 8) */    {OP_CMP_ST,   MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       OP_CMP_EN, OP_CMP_EN, MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},
/* 9) */    {OP_CMP_EN,   MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},

/* 10) */   {OP_ARROW,    MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},
/* 11) */   {OP_PP,       MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},

/* 12) */   {IDENT,       IDENT,  MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   IDENT,  MAIN,   MAIN,   MAIN, MAIN},
/* 13) */   {STR_ST,      STR_ST, STR_ST, STR_ST,    STR_ST,   STR_ST,      STR_ST,  STR_ST,     STR_ST,    STR_ST,    STR_ST,   STR_ST, STR_ST, STR_ST, STR_EN, MAIN, STR_ST},
/* 13) */   {STR_EN,      MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN},

/* 14) */   {ERR,         ERR,    ERR,    ERR,       ERR,      ERR,         ERR,     ERR,        ERR,       ERR,       ERR,      ERR,    ERR,    ERR,    ERR,    ERR,  MAIN},
/* 15) */   {SPACE,       MAIN,   MAIN,   MAIN,      MAIN,     MAIN,        MAIN,    MAIN,       MAIN,      MAIN,      MAIN,     MAIN,   MAIN,   MAIN,   MAIN,   MAIN, MAIN}}
};

#endif // DFA_H
