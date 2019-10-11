#ifndef TOKENTYPE_H
#define TOKENTYPE_H

enum class TokenType {
    IDENT = 0,
    FN_DECL,
    L_BRACKET,
    R_BRACKER,
    COLON,
    KW_BOOL,
    ARROW_SYM,
    L_CURLY_BRACKET,
    R_CURLY_BRACKET,
};

#endif // TOKENTYPE_H
