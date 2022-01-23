#ifndef WISNIALANG_AST_DEFINITIONS_HPP
#define WISNIALANG_AST_DEFINITIONS_HPP

// Wisnia
#include "Root.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class Param : public Root {
 public:
  explicit Param(const std::shared_ptr<Basic::Token> &tok)
      : Root(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "Param";
  }

  void print(size_t level) const override {
    Root::print(level++);
    m_var->print(level);
  }

  void addType(std::unique_ptr<BaseType> type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_var = std::move(var);
  }

  const std::unique_ptr<BaseExpr> &getVar() const {
    return m_var;
  }

 private:
  std::unique_ptr<BaseExpr> m_var;
};

class BaseDef : public Root {
 public:
  explicit BaseDef(const std::shared_ptr<Basic::Token> &tok)
      : Root(tok) {}

  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    Root::print(level++);
    m_var->print(level);
  }

  void addType(std::unique_ptr<BaseType> type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_var = std::move(var);
  }

  const std::unique_ptr<BaseExpr> &getVar() const {
    return m_var;
  }

 protected:
  std::unique_ptr<BaseExpr> m_var;
};

class MethodDef : public BaseDef {
 public:
  explicit MethodDef(const std::shared_ptr<Basic::Token> &tok)
      : BaseDef(tok) {}

  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    BaseDef::print(level++);
    for (const auto &param : m_params)
      param->print(level);
  }

  void addRetType(std::unique_ptr<BaseType> type) {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addParams(std::vector<std::unique_ptr<Param>> params) {
    m_params = std::move(params);
  }

  void addBody(std::unique_ptr<BaseStmt> body) {
    m_body = std::move(body);
  }

  const std::vector<std::unique_ptr<Param>> &getParams() const {
    return m_params;
  }

  const std::unique_ptr<BaseStmt> &getBody() const {
    return m_body;
  }

 protected:
  std::unique_ptr<BaseStmt> m_body;

 private:
  std::vector<std::unique_ptr<Param>> m_params;
};

class FnDef : public MethodDef {
 public:
  explicit FnDef(const std::shared_ptr<Basic::Token> &tok)
      : MethodDef(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "FnDef";
  }

  void print(size_t level) const override {
    MethodDef::print(level++);
    m_body->print(level);
  }
};

class CtorDef : public MethodDef {
 public:
  explicit CtorDef(const std::shared_ptr<Basic::Token> &tok)
      : MethodDef(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "CtorDef";
  }

  void print(size_t level) const override {
    MethodDef::print(level++);
    m_body->print(level);
  }
};

class DtorDef : public MethodDef {
 public:
  explicit DtorDef(const std::shared_ptr<Basic::Token> &tok)
      : MethodDef(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "DtorDef";
  }

  void print(size_t level) const override {
    MethodDef::print(level++);
    m_body->print(level);
  }
};

class Field : public Root {
 public:
  explicit Field(const std::shared_ptr<Basic::Token> &tok)
      : Root(tok) {}

  Field() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "Field";
  }

  void print(size_t level) const override {
    Root::print(level++);
    m_var->print(level);
  }

  void addType(std::unique_ptr<BaseType> varType) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(varType));
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_var = std::move(var);
  }

  void addValue(std::unique_ptr<BaseExpr> varValue) {
    m_value = std::move(varValue);
  }

  const std::unique_ptr<BaseExpr> &getVar() const {
    return m_var;
  }

  const std::unique_ptr<BaseExpr> &getValue() const {
    return m_value;
  }

 private:
  std::unique_ptr<BaseExpr> m_var;
  std::unique_ptr<BaseExpr> m_value;
};

class ClassDef : public BaseDef {
 public:
  explicit ClassDef(const std::shared_ptr<Basic::Token> &tok)
      : BaseDef(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ClassDef";
  }

  void print(size_t level) const override {
    BaseDef::print(level++);
    if (m_ctor) m_ctor->print(level);
    if (m_dtor) m_dtor->print(level);
    for (const auto &method : m_methods)
      method->print(level);
    for (const auto &field : m_fields)
      field->print(level);
  }

  void addCtor(std::unique_ptr<BaseDef> ctor) {
    m_ctor = std::move(ctor);
  }

  void addDtor(std::unique_ptr<BaseDef> dtor) {
    m_dtor = std::move(dtor);
  }

  void addMethod(std::unique_ptr<BaseDef> method) {
    m_methods.push_back(std::move(method));
  }

  void addField(std::unique_ptr<Field> field) {
    m_fields.push_back(std::move(field));
  }

  const std::unique_ptr<BaseDef> &getCtor() const {
    return m_ctor;
  }

  const std::unique_ptr<BaseDef> &getDtor() const {
    return m_dtor;
  }

  const std::vector<std::unique_ptr<BaseDef>> &getMethods() const {
    return m_methods;
  }

  const std::vector<std::unique_ptr<Field>> &getFields() const {
    return m_fields;
  }

 private:
  std::unique_ptr<BaseDef> m_ctor;
  std::unique_ptr<BaseDef> m_dtor;
  std::vector<std::unique_ptr<BaseDef>> m_methods;
  std::vector<std::unique_ptr<Field>> m_fields;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_DEFINITIONS_HPP
