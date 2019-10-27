#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <memory>
#include "token/TokenType.h"

class Token;

class Lexer {
    Lexer() = default;

    enum State {
        START,            // Start state
        IDENT,            // Identifier
        OP_COMPARE,       // Either a single operand (!, <, >, =) or (!=, <=, >=, ==)
        STRING,           // String literal
        INTEGER,          // Integer literal
        FLOAT,            // Float literal
        ERRONEOUS_NUMBER, // Invalid number
        LOGIC_AND,        // Logical AND: &&
        LOGIC_OR,         // Logical OR: ||
        OP_PP,            // Unary prefix: ++
        OP_ARROW          // Arrow: ->
    };

    struct TokenState {
        State state_ {START};
        std::string buff_;

        // Accessors to file data
        std::string data_;
        std::string::iterator it_;

        // Vague info about data file
        std::string fileName_;
        int lineNo {1};
    };

public:
    // Prohibit copy ctor
    Lexer(Lexer const &) = delete;
    void operator=(Lexer const &) = delete;

    // Get an instance of Lexer
    static Lexer &get() {
        static Lexer lexer_;
        return lexer_;
    }

    // Tokenize source file
    void tokenize(const std::string &input);

    std::shared_ptr<Token> finish_lexem(const TokenType &type_, bool backtrack = false);

    std::shared_ptr<Token> finish_ident();

    std::shared_ptr<Token> lex_next(char ch);

    // Prints out tokens in a pretty table
    void prettyPrint();

private:
    std::vector<std::shared_ptr<Token>> tokens_;
    TokenState tokenState_;
};

#endif // LEXER_H
