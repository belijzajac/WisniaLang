// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_AST_DEFINITIONS_HPP
#define WISNIALANG_AST_DEFINITIONS_HPP

#include <utility>

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class Param : public Root {
  using TypePtr     = std::unique_ptr<BaseType>;
  using VariablePtr = std::unique_ptr<BaseExpr>;

 public:
  explicit Param(TokenPtr token)
      : Root(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "Param";
  }

  void print(std::ostream &output, size_t level) const override {
    Root::print(output, level++);
    m_variable->print(output, level);
  }

  void addType(TypePtr type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_variable.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVariable(VariablePtr variable) {
    m_variable = std::move(variable);
  }

  const VariablePtr &getVariable() const {
    return m_variable;
  }

 private:
  VariablePtr m_variable;
};

class BaseDef : public Root {
  using VariablePtr = std::unique_ptr<BaseExpr>;

 public:
  explicit BaseDef(TokenPtr token)
      : Root(std::move(token)) {}

  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    Root::print(output, level++);
    m_variable->print(output, level);
  }

  void addType(std::unique_ptr<BaseType> type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_variable.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVariable(VariablePtr variable) {
    m_variable = std::move(variable);
  }

  const VariablePtr &getVariable() const {
    return m_variable;
  }

 protected:
  VariablePtr m_variable;
};

class MethodDef : public BaseDef {
  using TypePtr      = std::unique_ptr<BaseType>;
  using ParameterPtr = std::unique_ptr<Param>;
  using BodyPtr      = std::unique_ptr<BaseStmt>;

 public:
  explicit MethodDef(TokenPtr token)
      : BaseDef(std::move(token)) {}

  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    BaseDef::print(output, level++);
    for (const auto &param : m_parameters) {
      param->print(output, level);
    }
  }

  void addReturnType(TypePtr type) {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_variable.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addParameters(std::vector<ParameterPtr> params) {
    m_parameters = std::move(params);
  }

  void addBody(BodyPtr body) {
    m_body = std::move(body);
  }

  const std::vector<ParameterPtr> &getParameters() const {
    return m_parameters;
  }

  const BodyPtr &getBody() const {
    return m_body;
  }

 protected:
  BodyPtr m_body;

 private:
  std::vector<ParameterPtr> m_parameters;
};

class FnDef : public MethodDef {
 public:
  explicit FnDef(TokenPtr token)
      : MethodDef(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "FnDef";
  }

  void print(std::ostream &output, size_t level) const override {
    MethodDef::print(output, level++);
    m_body->print(output, level);
  }
};

class CtorDef : public MethodDef {
 public:
  explicit CtorDef(TokenPtr token)
      : MethodDef(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "CtorDef";
  }

  void print(std::ostream &output, size_t level) const override {
    MethodDef::print(output, level++);
    m_body->print(output, level);
  }
};

class DtorDef : public MethodDef {
 public:
  explicit DtorDef(TokenPtr token)
      : MethodDef(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "DtorDef";
  }

  void print(std::ostream &output, size_t level) const override {
    MethodDef::print(output, level++);
    m_body->print(output, level);
  }
};

class Field : public Root {
  using TypePtr     = std::unique_ptr<BaseType>;
  using VariablePtr = std::unique_ptr<BaseExpr>;
  using ValuePtr    = std::unique_ptr<BaseExpr>;

 public:
  explicit Field(TokenPtr token)
      : Root(std::move(token)) {}

  Field() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "Field";
  }

  void print(std::ostream &output, size_t level) const override {
    Root::print(output, level++);
    m_variable->print(output, level);
  }

  void addType(TypePtr type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_variable.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVariable(VariablePtr variable) {
    m_variable = std::move(variable);
  }

  void addValue(ValuePtr varValue) {
    m_value = std::move(varValue);
  }

  const VariablePtr &getVariable() const {
    return m_variable;
  }

  const ValuePtr &getValue() const {
    return m_value;
  }

 private:
  VariablePtr m_variable;
  ValuePtr m_value;
};

class ClassDef : public BaseDef {
  using ConstructorPtr = std::unique_ptr<BaseDef>;
  using DestructorPtr  = std::unique_ptr<BaseDef>;
  using MethodPtr      = std::unique_ptr<BaseDef>;
  using FieldPtr       = std::unique_ptr<Field>;

 public:
  explicit ClassDef(TokenPtr token)
      : BaseDef(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ClassDef";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseDef::print(output, level++);
    if (m_constructor) m_constructor->print(output, level);
    if (m_destructor) m_destructor->print(output, level);
    for (const auto &method : m_methods) {
      method->print(output, level);
    }
    for (const auto &field : m_fields) {
      field->print(output, level);
    }
  }

  void addConstructor(ConstructorPtr constructor) {
    m_constructor = std::move(constructor);
  }

  void addDestructor(DestructorPtr destructor) {
    m_destructor = std::move(destructor);
  }

  void addMethod(MethodPtr method) {
    m_methods.push_back(std::move(method));
  }

  void addField(FieldPtr field) {
    m_fields.push_back(std::move(field));
  }

  const ConstructorPtr &getConstructor() const {
    return m_constructor;
  }

  const DestructorPtr &getDestructor() const {
    return m_destructor;
  }

  const std::vector<MethodPtr> &getMethods() const {
    return m_methods;
  }

  const std::vector<FieldPtr> &getFields() const {
    return m_fields;
  }

 private:
  ConstructorPtr m_constructor;
  DestructorPtr m_destructor;
  std::vector<MethodPtr> m_methods;
  std::vector<FieldPtr> m_fields;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_DEFINITIONS_HPP
