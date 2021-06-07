#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include <unordered_map>

namespace Wisnia::Basic {
enum class TType {
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
  OP_USUB,
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

// A string representations for TType`s
static inline std::unordered_map<TType, std::string> TokenTypeToStr = {
  // Main types  
  {TType::LIT_INT, "LIT_INT"},
  {TType::LIT_FLT, "LIT_FLT"},
  {TType::LIT_STR, "LIT_STR"},
  {TType::IDENT, "IDENT"},
  // Keywords
  {TType::KW_FN, "KW_FN"},
  {TType::KW_CLASS, "KW_CLASS"},
  {TType::KW_CLASS_INIT, "KW_CLASS_INIT"},
  {TType::KW_CLASS_DEF, "KW_CLASS_DEF"},
  {TType::KW_CLASS_REM, "KW_CLASS_REM"},
  {TType::KW_RETURN, "KW_RETURN"},
  {TType::KW_IF, "KW_IF"},
  {TType::KW_ELIF, "KW_ELIF"},
  {TType::KW_ELSE, "KW_ELSE"},
  {TType::KW_FOR, "KW_FOR"},
  {TType::KW_WHILE, "KW_WHILE"},
  {TType::KW_FOREACH, "KW_FOREACH"},
  {TType::KW_FOREACH_IN, "KW_FOREACH_IN"},
  {TType::KW_CONTINUE, "KW_CONTINUE"},
  {TType::KW_BREAK, "KW_BREAK"},
  {TType::KW_TRUE, "KW_TRUE"},
  {TType::KW_FALSE, "KW_FALSE"},
  {TType::KW_READ, "KW_READ"},
  {TType::KW_PRINT, "KW_PRINT"},
  {TType::KW_VOID, "KW_VOID"},
  {TType::KW_INT, "KW_INT"},
  {TType::KW_BOOL, "KW_BOOL"},
  {TType::KW_FLOAT, "KW_FLOAT"},
  {TType::KW_STRING, "KW_STRING"},
  // Operators
  {TType::OP_ASSN, "OP_ASSN"},
  {TType::OP_FN_ARROW, "OP_FN_ARROW"},
  {TType::OP_METHOD_CALL, "OP_METHOD_CALL"},
  // Arithmetic operations
  {TType::OP_ADD, "OP_ADD"},
  {TType::OP_SUB, "OP_SUB"},
  {TType::OP_MUL, "OP_MUL"},
  {TType::OP_DIV, "OP_DIV"},
  // Unary operations
  {TType::OP_UADD, "OP_UADD"},
  {TType::OP_USUB, "OP_USUB"},
  {TType::OP_UNEG, "OP_UNEG"},
  // Logic operations
  {TType::OP_AND, "OP_AND"},
  {TType::OP_OR, "OP_OR"},
  {TType::OP_EQ, "OP_EQ"},
  {TType::OP_L, "OP_L"},
  {TType::OP_G, "OP_G"},
  {TType::OP_LE, "OP_LE"},
  {TType::OP_GE, "OP_GE"},
  {TType::OP_NE, "OP_NE"},
  // Separators
  {TType::OP_COMMA, "OP_COMMA"},
  {TType::OP_COL, "OP_COL"},
  {TType::OP_SEMICOLON, "OP_SEMICOLON"},
  // Parenthesis, Braces, Brackets
  {TType::OP_PAREN_O, "OP_PAREN_O"},
  {TType::OP_PAREN_C, "OP_PAREN_C"},
  {TType::OP_BRACE_O, "OP_BRACE_O"},
  {TType::OP_BRACE_C, "OP_BRACE_C"},
  // Other
  {TType::TOK_INVALID, "TOK_INVALID"},
  {TType::TOK_EOF, "TOK_EOF"}};

// String to TType of Keyword type
static inline std::unordered_map<std::string, TType> StrToTokenKw = {
    {"fn", TType::KW_FN},          {"class", TType::KW_CLASS},
    {"new", TType::KW_CLASS_INIT}, {"def", TType::KW_CLASS_DEF},
    {"rem", TType::KW_CLASS_REM},  {"return", TType::KW_RETURN},
    {"if", TType::KW_IF},          {"elif", TType::KW_ELIF},
    {"else", TType::KW_ELSE},      {"for", TType::KW_FOR},
    {"while", TType::KW_WHILE},    {"for_each", TType::KW_FOREACH},
    {"in", TType::KW_FOREACH_IN},  {"continue", TType::KW_CONTINUE},
    {"break", TType::KW_BREAK},    {"true", TType::KW_TRUE},
    {"false", TType::KW_FALSE},    {"read", TType::KW_READ},
    {"print", TType::KW_PRINT},    {"void", TType::KW_VOID},
    {"int", TType::KW_INT},        {"bool", TType::KW_BOOL},
    {"float", TType::KW_FLOAT},    {"string", TType::KW_STRING}};

// String to TType of Operator type
static inline std::unordered_map<std::string, TType> StrToTokenOp = {
    {"=", TType::OP_ASSN},        {"->", TType::OP_FN_ARROW},
    {".", TType::OP_METHOD_CALL}, {"+", TType::OP_ADD},
    {"-", TType::OP_SUB},         {"*", TType::OP_MUL},
    {"/", TType::OP_DIV},         {"++", TType::OP_UADD},
    {"!", TType::OP_UNEG},        {"&&", TType::OP_AND},
    {"||", TType::OP_OR},         {"==", TType::OP_EQ},
    {"<", TType::OP_L},           {">", TType::OP_G},
    {"<=", TType::OP_LE},         {">=", TType::OP_GE},
    {"!=", TType::OP_NE},         {"(", TType::OP_PAREN_O},
    {")", TType::OP_PAREN_C},     {"{", TType::OP_BRACE_O},
    {"}", TType::OP_BRACE_C},     {",", TType::OP_COMMA},
    {":", TType::OP_COL},         {";", TType::OP_SEMICOLON},
    {"--", TType::OP_USUB},
};

}  // namespace Wisnia::Basic

#endif  // TOKENTYPE_H
