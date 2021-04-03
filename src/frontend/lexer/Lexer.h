#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <memory>
// Wisnia
#include "../basic/TType.h"

namespace Wisnia {
    namespace Basic {
        class Token;
        enum class TType;
    }

    // TODO: hide the implementation of private parts with the PIMPL idiom
    class Lexer {
        enum class State {
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
            OP_ARROW,         // Arrow: ->

            ESCAPE_SEQ,       // Escapes \t, \n, etc.
            CMT_SINGLE,       // Single line comment: #

            // The following states are just to escape multi-line comments
            CMT_MAYBE_MULTI_CMT,
            CMT_MULTI,
            CMT_MAYBE_FINISH_MULTI,
        };

        struct TokenState {
            // Info needed to construct a token and to tokenize a letter
            State state_ {State::START};
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
            std::string erroneousType_;
        };

        // Having provided the TType, it constructs and returns a token
        std::shared_ptr<Basic::Token> finishTok(const Basic::TType &type_, bool backtrack = false);

        // From an existing token buffer constructs and returns a token of identifier (or keyword) type
        std::shared_ptr<Basic::Token> finishIdent();

        // Continues to tokenize the next letter
        std::shared_ptr<Basic::Token> tokNext(char ch);

    public:
        Lexer() = default;
        ~Lexer() = default;

        // Tokenize the given source file
        void tokenize(const std::string &input);

        // Returns tokens
        std::vector<std::shared_ptr<Basic::Token>> getTokens() const {
            return tokens_;
        }

        // Prints out tokens in a pretty table
        void prettyPrint();

    private:
        std::vector<std::shared_ptr<Basic::Token>> tokens_;
        TokenState tokenState_;
    };
} // Wisnia

#endif // LEXER_H
