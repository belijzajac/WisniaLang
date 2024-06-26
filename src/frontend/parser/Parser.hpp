// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_PARSER_HPP
#define WISNIALANG_PARSER_HPP

#include <memory>
#include <vector>
#include <cassert>

namespace Wisnia {
class Lexer;

namespace Basic {
class Token;
enum class TType;
}  // namespace Basic

namespace AST {
class Root;
class BaseDef;
class Param;
class BaseExpr;
class BaseType;
class BaseStmt;
class BaseLoop;
class BaseIf;
class Field;
}  // namespace AST

class Parser {
  using TokenPtr = std::shared_ptr<Basic::Token>;

 public:
  explicit Parser(const Lexer &lexer);

  // Starts parsing and returns the root node
  std::unique_ptr<AST::Root> parse();

 private:
  // Checks if the current token holds a correct type
  bool has(const Basic::TType &token) const;
  bool has2(const Basic::TType &token) const;

  // Checks if the sequence of tokens holds a correct type
  template <typename... TokenTypes>
  bool hasAnyOf(TokenTypes... type) {
    return (has(type) || ...);
  }

  // Position of current token
  int m_pos{-1};

  // Expects the following token to be of type `token`
  void expect(const Basic::TType &token);

  // Returns an instance of the current token
  const TokenPtr &curr() const {
    assert(m_pos < m_tokens.size());
    return m_tokens[m_pos];
  }

  // Consumes and returns current token
  // Used for obtaining tokens that represent names
  const TokenPtr &getNextToken() {
    assert(hasNext());
    consume();
    return m_tokens[m_pos];
  }

  // Returns an instance of the following token (peeks)
  const TokenPtr &peek() const {
    assert(hasNext());
    return m_tokens[m_pos + 1];
  }

  // Consumes token (skips current token position by 1)
  void consume() { m_pos++; }

  // Checks if we haven't reached the end of token stream
  bool hasNext() const { return m_pos + 1 < m_tokens.size(); }

  // Tokens stream copied from Lexer through constructor call
  std::vector<TokenPtr> m_tokens;

 private:
  // Parses identifier
  std::unique_ptr<AST::BaseExpr> parseVar();

  // Parses function definitions
  std::unique_ptr<AST::BaseDef> parseFnDef();

  // Parses a single parameter
  std::unique_ptr<AST::Param> parseParam();

  // Parses parameters list, found in function and constructor/destructor definitions
  std::vector<std::unique_ptr<AST::Param>> parseParamsList();

  // Parses primitive types
  std::unique_ptr<AST::BaseType> parsePrimitiveType();

  // Parses statement block
  std::unique_ptr<AST::BaseStmt> parseStmtBlock();

  // Parses statement of a specific type
  std::unique_ptr<AST::BaseStmt> parseStmt();

  // Parses return statement
  std::unique_ptr<AST::BaseStmt> parseReturnStmt();

  // Parses loop break (break, continue) statement
  std::unique_ptr<AST::BaseStmt> parseLoopBrkStmt();

  // Parses variable declaration statement
  std::unique_ptr<AST::BaseStmt> parseVarDeclStmt();

  // Parses variable assignment statement
  std::unique_ptr<AST::BaseStmt> parseVarAssignStmt(bool expectSemicolon = true);

  // Parses expression statement
  std::unique_ptr<AST::BaseStmt> parseExprStmt();

  // Parses IO statement
  std::unique_ptr<AST::BaseStmt> parseIOStmt();

  // Parses Read IO statement
  std::unique_ptr<AST::BaseStmt> parseReadIOStmt();

  // Parses Write IO statement
  std::unique_ptr<AST::BaseStmt> parseWriteIOStmt();

  // Parses Loop statements
  std::unique_ptr<AST::BaseLoop> parseLoops();

  // Parses While loop statement
  std::unique_ptr<AST::BaseLoop> parseWhileLoop();

  // Parses For loop statement
  std::unique_ptr<AST::BaseLoop> parseForLoop();

  // Parses ForEach loop statement
  std::unique_ptr<AST::BaseLoop> parseForEachLoop();

  // Parses if condition block statement
  std::unique_ptr<AST::BaseIf> parseIfBlock();

  // Parses multiple else block statements
  std::vector<std::unique_ptr<AST::BaseIf>> parseMultipleElseBlock();

  // Parses expression -- start of the expression parsing
  std::unique_ptr<AST::BaseExpr> parseExpr();

  // Parses logical AND (&&) expression
  std::unique_ptr<AST::BaseExpr> parseAndExpr();

  // Parses logical Equality (==, !=) expression
  std::unique_ptr<AST::BaseExpr> parseEqExpr();

  // Parses comparison (>, >=, <, <=) expression
  std::unique_ptr<AST::BaseExpr> parseCompExpr();

  // Parses addition and subtraction (+, -) expression
  std::unique_ptr<AST::BaseExpr> parseAddExpr();

  // Parses multiplication and division (*, /) expression
  std::unique_ptr<AST::BaseExpr> parseMultExpr();

  // Parses unary (!, ++) expression
  std::unique_ptr<AST::BaseExpr> parseUnaryExpr();

  // Parses other expressions
  std::unique_ptr<AST::BaseExpr> parseSomeExpr();

  // Parses variable expression (variable, class method call, function call)
  std::unique_ptr<AST::BaseExpr> parseVarExp();

  // Parses function call
  std::unique_ptr<AST::BaseExpr> parseFnCall();

  // Parses class method call
  // e.g. classPtr->calculateFib(5)
  std::unique_ptr<AST::BaseExpr> parseMethodCall();

  // Parses argument list for function, class initialization, and method call
  std::vector<std::unique_ptr<AST::BaseExpr>> parseArgsList();

  // Parses class initialization expression
  std::unique_ptr<AST::BaseExpr> parseClassInit();

  // Parses constant expression
  std::unique_ptr<AST::BaseExpr> parseConstExpr();

  // Parses class definitions
  std::unique_ptr<AST::BaseDef> parseClassDef();

  // Parses class constructor definition
  std::unique_ptr<AST::BaseDef> parseClassCtorDef();

  // Parses class destructor definition
  std::unique_ptr<AST::BaseDef> parseClassDtorDef();

  // Parses class fields
  std::unique_ptr<AST::Field> parseClassField();
};

}  // namespace Wisnia

#endif  // WISNIALANG_PARSER_HPP
