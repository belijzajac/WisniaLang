#ifndef WISNIALANG_AST_LOOPS_HPP
#define WISNIALANG_AST_LOOPS_HPP

// Wisnia
#include "Root.hpp"
#include "Statements.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

class BaseLoop : public BaseStmt {
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

class WhileLoop : public BaseLoop {
 public:
  explicit WhileLoop(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "WhileLoop";
  }

  void print(size_t level) const override {
    BaseLoop::print(level++);
    m_cond->print(level);
    m_body->print(level);
  }

  void addCond(std::unique_ptr<BaseExpr> expr) {
    m_cond = std::move(expr);
  }

  const std::unique_ptr<BaseExpr> &getCondition() const {
    return m_cond;
  }

 private:
  std::unique_ptr<BaseExpr> m_cond;
};

class ForLoop : public BaseLoop {
 public:
  explicit ForLoop(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ForLoop";
  }

  void print(size_t level) const override {
    BaseLoop::print(level++);
    m_initial->print(level);
    m_condition->print(level);
    m_increment->print(level);
    m_body->print(level);
  }

  void addInitial(std::unique_ptr<BaseStmt> expr) {
    m_initial = std::move(expr);
  }

  void addCondition(std::unique_ptr<BaseExpr> expr) {
    m_condition = std::move(expr);
  }

  void addIncrement(std::unique_ptr<BaseStmt> stmt) {
    m_increment = std::move(stmt);
  }

  const std::unique_ptr<BaseStmt> &getInitial() const {
    return m_initial;
  }

  const std::unique_ptr<BaseExpr> &getCondition() const {
    return m_condition;
  }

  const std::unique_ptr<BaseStmt> &getIncrement() const {
    return m_increment;
  }

 private:
  std::unique_ptr<BaseStmt> m_initial;
  std::unique_ptr<BaseExpr> m_condition;
  std::unique_ptr<BaseStmt> m_increment;
};

class ForEachLoop : public BaseLoop {
 public:
  explicit ForEachLoop(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ForEachLoop";
  }

  void print(size_t level) const override {
    BaseLoop::print(level++);
    m_element->print(level);
    m_collection->print(level);
    m_body->print(level);
  }

  void addElement(std::unique_ptr<BaseExpr> expr) {
    m_element = std::move(expr);
  }

  void addCollection(std::unique_ptr<BaseExpr> expr) {
    m_collection = std::move(expr);
  }

  const std::unique_ptr<BaseExpr> &getElement() const {
    return m_element;
  }

  const std::unique_ptr<BaseExpr> &getCollection() const {
    return m_collection;
  }

 private:
  std::unique_ptr<BaseExpr> m_element;
  std::unique_ptr<BaseExpr> m_collection;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_LOOPS_HPP
