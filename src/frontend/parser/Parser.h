#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>

namespace Wisnia {
class Lexer;

namespace Basic {
class Token;
enum class TType;
}  // namespace Basic

namespace AST {
class Root;
class Def;
class Param;
class Expr;
class Type;
class Stmt;
class Loop;
class BaseIf;
class Var;
class Field;
}  // namespace AST

class Parser {
 public:
  explicit Parser(const Lexer &lexer);
  ~Parser() = default;

  // Starts parsing and returns the root node when done doing so
  std::unique_ptr<AST::Root> parse();

 private:
  // Checks if the current token is of type `token`
  bool has(const Basic::TType &token) const;
  bool has2(const Basic::TType &token) const;

  // Checks if the sequence of tokens is of type `token`
  template <typename... TokenTypes>
  bool hasAnyOf(TokenTypes... type) {
    return (has(type) || ...);
  }

  // Position of current token
  int pos_{-1};

  // Expects the following token to be of type `token`
  void expect(const Basic::TType &token);

  // Returns an instance of the current token
  const std::shared_ptr<Basic::Token> &curr() const { return tokens_.at(pos_); }

  // Consumes and returns current token
  // Used for obtaining tokens that represent names
  const std::shared_ptr<Basic::Token> &getNextToken() {
    consume();
    return tokens_.at(pos_);
  }

  // Returns an instance of the following token (peeks)
  const std::shared_ptr<Basic::Token> &peek() const { return tokens_.at(pos_ + 1); }

  // Consumes token (skips current token position by 1)
  void consume() { pos_++; }

  // Checks if we haven't reached the end of token stream
  bool hasNext() const { return pos_ + 1 < tokens_.size(); }

  // Tokens stream copied from Lexer through constructor call
  std::vector<std::shared_ptr<Basic::Token>> tokens_;

 private:
  // Parses identifier
  std::unique_ptr<AST::Var> parseVar();

  // Parses function definitions
  std::unique_ptr<AST::Def> parseFnDef();

  // Parses a single parameter
  std::unique_ptr<AST::Param> parseParam();

  // Parses parameters list, found in function and ctor/dtor definitions
  std::vector<std::unique_ptr<AST::Param>> parseParamsList();

  // Parses function return type
  std::unique_ptr<AST::Type> parsePrimitiveType();

  // Parses statement block
  std::unique_ptr<AST::Stmt> parseStmtBlock();

  // Parses statement of a specific type
  std::unique_ptr<AST::Stmt> parseStmt();

  // Parses return statement
  std::unique_ptr<AST::Stmt> parseReturnStmt();

  // Parses loop break (break, continue) statement
  std::unique_ptr<AST::Stmt> parseLoopBrkStmt();

  // Parses variable declaration statement
  std::unique_ptr<AST::Stmt> parseVarDeclStmt();

  // Parses variable assignment statement
  std::unique_ptr<AST::Stmt> parseVarAssignStmt(bool expect_semicolon = true);

  // Parses expression statement
  std::unique_ptr<AST::Stmt> parseExprStmt();

  // Parses IO statement
  std::unique_ptr<AST::Stmt> parseIOStmt();

  // Parses Read IO statement
  std::unique_ptr<AST::Stmt> parseReadIOStmt();

  // Parses Write IO statement
  std::unique_ptr<AST::Stmt> parseWriteIOStmt();

  // Parses Loop statements
  std::unique_ptr<AST::Loop> parseLoops();

  // Parses While loop statement
  std::unique_ptr<AST::Loop> parseWhileLoop();

  // Parses For loop statement
  std::unique_ptr<AST::Loop> parseForLoop();

  // Parses ForEach loop statement
  std::unique_ptr<AST::Loop> parseForEachLoop();

  // Parses if condition block statement
  std::unique_ptr<AST::BaseIf> parseIfBlock();

  // Parses multiple else block statements
  std::vector<std::unique_ptr<AST::BaseIf>> parseMultipleElseBlock();

  // Parses expression -- starts the whole parsing from this function
  std::unique_ptr<AST::Expr> parseExpr();

  // Parses logical AND (&&) expression
  std::unique_ptr<AST::Expr> parseAndExpr();

  // Parses logical Equality (==, !=) expression
  std::unique_ptr<AST::Expr> parseEqExpr();

  // Parses comparison (>, >=, <, <=) expression
  std::unique_ptr<AST::Expr> parseCompExpr();

  // Parses addition (+) expression
  std::unique_ptr<AST::Expr> parseAddExpr();

  // Parses multiplication (*, /) expression
  std::unique_ptr<AST::Expr> parseMultExpr();

  // Parses unary (!, ++) expression
  std::unique_ptr<AST::Expr> parseUnaryExpr();

  // Parses other expressions
  std::unique_ptr<AST::Expr> parseSomeExpr();

  // Parses variangle expression (variable, class method call, function call)
  std::unique_ptr<AST::Expr> parseVarExp();

  // Parses function call
  std::unique_ptr<AST::Expr> parseFnCall();

  // Parses class method call
  // e.g. classPtr->getObj()->...->calculateFib(5)
  std::unique_ptr<AST::Expr> parseMethodCall();

  // Parses argument list for function, class initialization, and method call
  std::vector<std::unique_ptr<AST::Expr>> parseArgsList();

  // Parses class initialization expression
  std::unique_ptr<AST::Expr> parseClassInit();

  // Parses constant expression
  std::unique_ptr<AST::Expr> parseConstExpr();

  // Parses class definitions
  std::unique_ptr<AST::Def> parseClassDef();

  // Parses class c-tor definition
  std::unique_ptr<AST::Def> parseClassCtorDef();

  // Parses class d-tor definition
  std::unique_ptr<AST::Def> parseClassDtorDef();

  // Parses class' fields
  std::unique_ptr<AST::Field> parseClassField();
};

}  // namespace Wisnia

#endif  // PARSER_H
