#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>

class Lexer;
class Token;
enum class TokenType;

// TODO: put these under a namespace to not litter the code?
// TODO: put everything under the PIMPL
class AST;
class Def;
class Param;
class Expr;
class Type;
class Stmt;

class Parser {
    // Checks if the current token is of type `token`
    bool has(const TokenType &token);

    // Position of current token
    int pos_ {-1};

    // Expects the following token to be of type `token`
    void expect(const TokenType &token);

    // Parses identifier
    std::unique_ptr<Expr> parseIdent();

    // Parses function definitions
    std::unique_ptr<Def> parseFnDef();

    // Parses a single parameter
    std::unique_ptr<Param> parseParam();

    // Parses parameters list, found in function and ctor/dtor definitions
    std::unique_ptr<Param> parseParamsList();

    // Parses function return type
    std::unique_ptr<Type> parseFnType();

    // Parses statement block
    std::unique_ptr<Stmt> parseStmtBlock();

    // Parses statement of a specific type
    std::unique_ptr<Stmt> parseStmt();

    // Parses return statement
    std::unique_ptr<Stmt> parseReturnStmt();

public:
    explicit Parser(const Lexer &lexer);
    ~Parser() = default;

    // Returns an instance of token at position pos
    std::shared_ptr<Token> at(size_t pos) const {
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

    // Starts parsing
    std::unique_ptr<AST> parse();

private:
    std::vector<std::shared_ptr<Token>> tokens_;
};

#endif // PARSER_H
