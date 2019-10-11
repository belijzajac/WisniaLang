#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <memory>

class Lexer {
    class Token;
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

private:
    std::vector<std::unique_ptr<Token>> tokens_;
};

#endif // LEXER_H
