#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include <unordered_map>

enum class TokenType {
    // Main types
    LIT_INT,
    LIT_FLT,
    LIT_STR,
    IDENT,

    // Keywords
    KW_FN,
    KW_CLASS,
    KW_CLASS_INIT,
    KW_CLASS_DEF,
    KW_CLASS_REM,
    KW_RETURN,
    KW_IF,
    KW_ELIF,
    KW_ELSE,
    KW_FOR,
    KW_WHILE,
    KW_FOREACH,
    KW_FOREACH_IN,
    KW_CONTINUE,
    KW_BREAK,
    KW_TRUE,
    KW_FALSE,
    KW_READ,
    KW_PRINT,
    KW_VOID,
    KW_INT,
    KW_BOOL,
    KW_FLOAT,
    KW_STRING,

    // Operators
    OP_ASSN,
    OP_FN_ARROW,
    OP_METHOD_CALL,

    // Arithmetic operations
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,

    // Unary operations
    OP_UADD,
    OP_UNEG,

    // Logic operations
    OP_AND,
    OP_OR,
    OP_EQ,
    OP_L,
    OP_G,
    OP_LE,
    OP_GE,
    OP_NE,

    // Separators
    OP_COMMA,
    OP_COL,
    OP_SEMICOLON,

    // Parenthesis, Braces, Brackets
    OP_PAREN_O,
    OP_PAREN_C,
    OP_BRACE_O,
    OP_BRACE_C,

    // Other
    TOK_INVALID,
    TOK_EOF
};

// A string representations for TokenType`s
static inline std::unordered_map<TokenType, std::string> TokenTypeToStr = {
    {TokenType::LIT_INT, "LIT_INT"},
    {TokenType::LIT_FLT, "LIT_FLT"},
    {TokenType::LIT_STR, "LIT_STR"},
    {TokenType::IDENT, "IDENT"},

    {TokenType::KW_FN, "KW_FN"},
    {TokenType::KW_CLASS, "KW_CLASS"},
    {TokenType::KW_CLASS_INIT, "KW_CLASS_INIT"},
    {TokenType::KW_CLASS_DEF, "KW_CLASS_DEF"},
    {TokenType::KW_CLASS_REM, "KW_CLASS_REM"},
    {TokenType::KW_RETURN, "KW_RETURN"},
    {TokenType::KW_IF, "KW_IF"},
    {TokenType::KW_ELIF, "KW_ELIF"},
    {TokenType::KW_ELSE, "KW_ELSE"},
    {TokenType::KW_FOR, "KW_FOR"},
    {TokenType::KW_WHILE, "KW_WHILE"},
    {TokenType::KW_FOREACH, "KW_FOREACH"},
    {TokenType::KW_FOREACH_IN, "KW_FOREACH_IN"},
    {TokenType::KW_CONTINUE, "KW_CONTINUE"},
    {TokenType::KW_BREAK, "KW_BREAK"},
    {TokenType::KW_TRUE, "KW_TRUE"},
    {TokenType::KW_FALSE, "KW_FALSE"},
    {TokenType::KW_READ, "KW_READ"},
    {TokenType::KW_PRINT, "KW_PRINT"},
    {TokenType::KW_VOID, "KW_VOID"},
    {TokenType::KW_INT, "KW_INT"},
    {TokenType::KW_BOOL, "KW_BOOL"},
    {TokenType::KW_FLOAT, "KW_FLOAT"},
    {TokenType::KW_STRING, "KW_STRING"},

    {TokenType::OP_ASSN, "OP_ASSN"},
    {TokenType::OP_FN_ARROW, "OP_FN_ARROW"},
    {TokenType::OP_METHOD_CALL, "OP_METHOD_CALL"},

    {TokenType::OP_ADD, "OP_ADD"},
    {TokenType::OP_SUB, "OP_SUB"},
    {TokenType::OP_MUL, "OP_MUL"},
    {TokenType::OP_DIV, "OP_DIV"},

    {TokenType::OP_UADD, "OP_UADD"},
    {TokenType::OP_UNEG, "OP_UNEG"},

    {TokenType::OP_AND, "OP_AND"},
    {TokenType::OP_OR, "OP_OR"},
    {TokenType::OP_EQ, "OP_EQ"},
    {TokenType::OP_L, "OP_L"},
    {TokenType::OP_G, "OP_G"},
    {TokenType::OP_LE, "OP_LE"},
    {TokenType::OP_GE, "OP_GE"},
    {TokenType::OP_NE, "OP_NE"},

    {TokenType::OP_COMMA, "OP_COMMA"},
    {TokenType::OP_COL, "OP_COL"},
    {TokenType::OP_SEMICOLON, "OP_SEMICOLON"},

    {TokenType::OP_PAREN_O, "OP_PAREN_O"},
    {TokenType::OP_PAREN_C, "OP_PAREN_C"},
    {TokenType::OP_BRACE_O, "OP_BRACE_O"},
    {TokenType::OP_BRACE_C, "OP_BRACE_C"},

    {TokenType::TOK_INVALID, "TOK_INVALID"},
    {TokenType::TOK_EOF, "TOK_EOF"}
};

// String to TokenType of Keyword type
static inline std::unordered_map<std::string, TokenType> StrToTokenKw = {
    {"fn", TokenType::KW_FN},
    {"class", TokenType::KW_CLASS},
    {"new", TokenType::KW_CLASS_INIT},
    {"def", TokenType::KW_CLASS_DEF},
    {"rem", TokenType::KW_CLASS_REM},
    {"return", TokenType::KW_RETURN},
    {"if", TokenType::KW_IF},
    {"elif", TokenType::KW_ELIF},
    {"else", TokenType::KW_ELSE},
    {"for", TokenType::KW_FOR},
    {"while", TokenType::KW_WHILE},
    {"for_each", TokenType::KW_FOREACH},
    {"in", TokenType::KW_FOREACH_IN},
    {"continue", TokenType::KW_CONTINUE},
    {"break", TokenType::KW_BREAK},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE},
    {"read", TokenType::KW_READ},
    {"print", TokenType::KW_PRINT},
    {"void", TokenType::KW_VOID},
    {"int", TokenType::KW_INT},
    {"bool", TokenType::KW_BOOL},
    {"float", TokenType::KW_FLOAT},
    {"string", TokenType::KW_STRING}
};

// String to TokenType of Operator type
static inline std::unordered_map<std::string, TokenType> StrToTokenOp = {
    {"=", TokenType::OP_ASSN},
    {"->", TokenType::OP_FN_ARROW},
    {".", TokenType::OP_METHOD_CALL},

    {"+", TokenType::OP_ADD},
    {"-", TokenType::OP_SUB},
    {"*", TokenType::OP_MUL},
    {"/", TokenType::OP_DIV},

    {"++", TokenType::OP_UADD},
    {"!", TokenType::OP_UNEG},

    {"&&", TokenType::OP_AND},
    {"||", TokenType::OP_OR},
    {"==", TokenType::OP_EQ},
    {"<", TokenType::OP_L},
    {">", TokenType::OP_G},
    {"<=", TokenType::OP_LE},
    {">=", TokenType::OP_GE},
    {"!=", TokenType::OP_NE},

    {"(", TokenType::OP_PAREN_O},
    {")", TokenType::OP_PAREN_C},
    {"{", TokenType::OP_BRACE_O},
    {"}", TokenType::OP_BRACE_C},

    {",", TokenType::OP_COMMA},
    {":", TokenType::OP_COL},
    {";", TokenType::OP_SEMICOLON},
};

#endif // TOKENTYPE_H
