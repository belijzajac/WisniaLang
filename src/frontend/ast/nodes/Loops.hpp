// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0
  
#ifndef WISNIALANG_AST_LOOPS_HPP
#define WISNIALANG_AST_LOOPS_HPP

#include <utility>
// Wisnia
#include "Root.hpp"
#include "Statements.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

class BaseLoop : public BaseStmt {
  using BodyPtr = std::unique_ptr<BaseStmt>;

 public:
  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level);
  }

  void addBody(BodyPtr body) {
    m_body = std::move(body);
  }

  const BodyPtr &getBody() const {
    return m_body;
  }

 protected:
  explicit BaseLoop(TokenPtr token)
      : BaseStmt(std::move(token)) {}

 protected:
  BodyPtr m_body;
};

class WhileLoop final : public BaseLoop {
  using ConditionPtr = std::unique_ptr<BaseExpr>;

 public:
  explicit WhileLoop(TokenPtr token)
      : BaseLoop(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "WhileLoop";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseLoop::print(output, level++);
    m_condition->print(output, level);
    m_body->print(output, level);
  }

  void addCondition(ConditionPtr condition) {
    m_condition = std::move(condition);
  }

  const ConditionPtr &getCondition() const {
    return m_condition;
  }

 private:
  ConditionPtr m_condition;
};

class ForLoop final : public BaseLoop {
  using InitialPtr   = std::unique_ptr<BaseStmt>; // e.g. int i = 0
  using ConditionPtr = std::unique_ptr<BaseExpr>; // e.g. i < 10
  using IncrementPtr = std::unique_ptr<BaseStmt>; // e.g. i++

 public:
  explicit ForLoop(TokenPtr token)
      : BaseLoop(std::move(token)) {}

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

  void addInitial(InitialPtr initial) {
    m_initial = std::move(initial);
  }

  void addCondition(ConditionPtr condition) {
    m_condition = std::move(condition);
  }

  void addIncrement(IncrementPtr increment) {
    m_increment = std::move(increment);
  }

  const InitialPtr &getInitial() const {
    return m_initial;
  }

  const ConditionPtr &getCondition() const {
    return m_condition;
  }

  const IncrementPtr &getIncrement() const {
    return m_increment;
  }

 private:
  InitialPtr m_initial;
  ConditionPtr m_condition;
  IncrementPtr m_increment;
};

class ForEachLoop final : public BaseLoop {
  using ElementPtr    = std::unique_ptr<BaseExpr>;
  using CollectionPtr = std::unique_ptr<BaseExpr>;

 public:
  explicit ForEachLoop(TokenPtr token)
      : BaseLoop(std::move(token)) {}

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

  void addElement(ElementPtr element) {
    m_element = std::move(element);
  }

  void addCollection(CollectionPtr collection) {
    m_collection = std::move(collection);
  }

  const ElementPtr &getElement() const {
    return m_element;
  }

  const CollectionPtr &getCollection() const {
    return m_collection;
  }

 private:
  ElementPtr m_element;
  CollectionPtr m_collection;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_LOOPS_HPP
