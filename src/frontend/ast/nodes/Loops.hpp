// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0
  
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
  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level);
  }

  void addBody(std::unique_ptr<BaseStmt> body) {
    m_body = std::move(body);
  }

  const std::unique_ptr<BaseStmt> &getBody() const {
    return m_body;
  }

 protected:
  explicit BaseLoop(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

 protected:
  std::unique_ptr<BaseStmt> m_body;
};

class WhileLoop : public BaseLoop {
 public:
  explicit WhileLoop(const std::shared_ptr<Basic::Token> &tok)
      : BaseLoop(tok) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "WhileLoop";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseLoop::print(output, level++);
    m_cond->print(output, level);
    m_body->print(output, level);
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
  explicit ForLoop(const std::shared_ptr<Basic::Token> &tok)
      : BaseLoop(tok) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ForLoop";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseLoop::print(output, level++);
    m_initial->print(output, level);
    m_condition->print(output, level);
    m_increment->print(output, level);
    m_body->print(output, level);
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
  explicit ForEachLoop(const std::shared_ptr<Basic::Token> &tok)
      : BaseLoop(tok) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ForEachLoop";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseLoop::print(output, level++);
    m_element->print(output, level);
    m_collection->print(output, level);
    m_body->print(output, level);
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
