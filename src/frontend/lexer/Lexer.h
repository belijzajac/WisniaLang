#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <memory>
#include "token/TokenType.h"

class Token;

// TODO: hide the implementation of private parts with the PIMPL idiom
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
        OP_ARROW,          // Arrow: ->

        ESCAPE_SEQ,
        CMT_SINGLE,

        CMT_MAYBE_MULTI_CMT,
        CMT_MULTI,
        CMT_MAYBE_FINISH_MULTI,
    };

    struct TokenState {
        // Info needed to construct a token and to tokenize a letter
        State state_ {START};
        std::string buff_;

        // Accessors to the actual data of the source file
        std::string data_;
        std::string::iterator it_;

        // Vague info about the source file
        std::string fileName_;
        int lineNo {1};

        // String info
        int stringStart {0};

        // Temp info
        TokenType tempType_;
    };

    // Having provided the TokenType, it constructs and returns a token
    std::shared_ptr<Token> finishTok(const TokenType &type_, bool backtrack = false);

    // From an existing token buffer constructs and returns a token of identifier (or keyword) type
    std::shared_ptr<Token> finishIdent();

    // Continues to tokenize the next letter
    std::shared_ptr<Token> tokNext(char ch);

public:
    // Prohibit copy ctor
    Lexer(Lexer const &) = delete;
    void operator=(Lexer const &) = delete;

    // Get an instance of Lexer
    static Lexer &get() {
        static Lexer lexer_;
        return lexer_;
    }

    // Tokenize the given source file
    void tokenize(const std::string &input);

    // Prints out tokens in a pretty table
    void prettyPrint();

private:
    std::vector<std::shared_ptr<Token>> tokens_;
    TokenState tokenState_;
};

#endif // LEXER_H
