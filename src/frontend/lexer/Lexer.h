#ifndef LEXER_H
#define LEXER_H

#include <string>

class Lexer {
private:
    Lexer() = default;

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
};

#endif // LEXER_H
