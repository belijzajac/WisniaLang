#ifndef AST_DEFINITIONS_H
#define AST_DEFINITIONS_H

// Wisnia
#include "Root.h"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class Param : public Root {
 public:
  explicit Param(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

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

 public:
  std::unique_ptr<BaseExpr> m_var;
};

class BaseDef : public Root {
 public:
  explicit BaseDef(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

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

 public:
  std::unique_ptr<BaseExpr> m_var;
};

class MethodDef : public BaseDef {
 public:
  explicit MethodDef(const std::shared_ptr<Basic::Token> &tok)
      : BaseDef(tok) { m_token = tok; }

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

 public:
  std::vector<std::unique_ptr<Param>> m_params;  // parameters
  std::unique_ptr<BaseStmt> m_body;              // body, surrounded by "{" and "}"
};

class FnDef : public MethodDef {
 public:
  explicit FnDef(const std::shared_ptr<Basic::Token> &tok)
      : MethodDef(tok) { m_token = tok; }

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
      : MethodDef(tok) { m_token = tok; }

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
      : MethodDef(tok) { m_token = tok; }

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
  explicit Field(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
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

 public:
  std::unique_ptr<BaseExpr> m_var;   // variable name
  std::unique_ptr<BaseExpr> m_value; // variable value
};

class ClassDef : public BaseDef {
 public:
  explicit ClassDef(const std::shared_ptr<Basic::Token> &tok)
      : BaseDef(tok) { m_token = tok; }

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

 public:
  std::unique_ptr<BaseDef> m_ctor;                 // constructor
  std::unique_ptr<BaseDef> m_dtor;                 // destructor
  std::vector<std::unique_ptr<BaseDef>> m_methods; // methods
  std::vector<std::unique_ptr<Field>> m_fields;    // fields
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_DEFINITIONS_H
