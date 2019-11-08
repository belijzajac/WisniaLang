#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>

class Lexer;
class Token;
enum class TokenType;

class Parser {
    // Checks if the current token is of type `token`
    bool has(const TokenType &token);

    // Position of current token
    size_t pos_ {0};

    // Expects the following token to be of type `token`
    void expect(const TokenType &token);

public:
    explicit Parser(const Lexer &lexer);
    ~Parser() = default;

    // Returns an instance of token at position pos
    const std::shared_ptr<Token> &at(size_t pos) const {
        return (pos >= tokens_.size() || pos < 0)? nullptr : tokens_.at(pos);
    }

    // Returns an instance of the current token
    const std::shared_ptr<Token> &curr() const { return tokens_.at(pos_); }

    // Returns an instance of the following token (peeks)
    const std::shared_ptr<Token> &peek() const { return tokens_.at(pos_ + 1); }

    // Consumes token (skips current token position by 1)
    void consume() { pos_++; }

    // Checks if we haven't reached the end of token stream
    bool hasNext() const { return pos_ + 1 < tokens_.size(); }

private:
    std::vector<std::shared_ptr<Token>> tokens_;
};

#endif // PARSER_H
