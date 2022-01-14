#ifndef WISNIALANG_AST_CONDITIONALS_HPP
#define WISNIALANG_AST_CONDITIONALS_HPP

// Wisnia
#include "Root.hpp"
#include "Statements.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class BaseIf : public BaseStmt {
 public:
  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    BaseStmt::print(level);
  }

  void addBody(std::unique_ptr<BaseStmt> body) {
    m_body = std::move(body);
  }

  const std::unique_ptr<BaseStmt> &getBody() const {
    return m_body;
  }

 protected:
  std::unique_ptr<BaseStmt> m_body;
};

class IfStmt : public BaseIf {
 public:
  explicit IfStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "IfStmt";
  }

  void print(size_t level) const override {
    BaseIf::print(level++);
    m_condition->print(level);
    m_body->print(level);
    level--; // reset for else statements
    for (const auto &stmt : m_elseStmts)
      stmt->print(level);
  }

  void addCondition(std::unique_ptr<BaseExpr> expr) {
    m_condition = std::move(expr);
  }

  void addElseBlocks(std::vector<std::unique_ptr<BaseIf>> expr) {
    m_elseStmts = std::move(expr);
  }

  const std::unique_ptr<BaseExpr> &getCondition() const {
    return m_condition;
  }

  const std::vector<std::unique_ptr<BaseIf>> &getElseStatements() const {
    return m_elseStmts;
  }

 private:
  std::unique_ptr<BaseExpr> m_condition;
  std::vector<std::unique_ptr<BaseIf>> m_elseStmts;
};

class ElseStmt : public BaseIf {
 public:
  explicit ElseStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ElseStmt";
  }

  void print(size_t level) const override {
    BaseIf::print(level++);
    m_body->print(level);
  }
};

class ElseIfStmt : public BaseIf {
 public:
  explicit ElseIfStmt(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ElseIfStmt";
  }

  void print(size_t level) const override {
    BaseIf::print(level++);
    m_condition->print(level);
    m_body->print(level);
  }

  void addCondition(std::unique_ptr<BaseExpr> expr) {
    m_condition = std::move(expr);
  }

  const std::unique_ptr<BaseExpr> &getCondition() const {
    return m_condition;
  }

 private:
  std::unique_ptr<BaseExpr> m_condition;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_CONDITIONALS_HPP
