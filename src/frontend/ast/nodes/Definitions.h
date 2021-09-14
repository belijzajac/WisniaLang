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
  explicit Param(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  Param() = default;

  std::string kind() const override {
    return "Param";
  }

  void print(size_t level) const override {
    Root::print(level); level++;
    type_->print(level);
    value_->print(level);
  }

  void addType(std::unique_ptr<Type> type) {
    type_ = std::move(type);
  }

  void addValue(std::unique_ptr<Expr> value) {
    value_ = std::move(value);
  }

 public:
  std::unique_ptr<Type> type_;
  std::unique_ptr<Expr> value_;
};

// An abstract definition for Def node
class Def : public Root {
 public:
  explicit Def(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }

  std::string getName() const {
    return token_->getValueStr();
  }

  void print(size_t level) const override {
    Root::print(level);
  }
};

// An abstract definition for Def node
class MethodDef : public Def {
 public:
  explicit MethodDef(const std::shared_ptr<Basic::Token> &tok) : Def(tok) { token_ = tok; }

  void print(size_t level) const override {
    Def::print(level); level++;
    for (const auto &param : params_)
      param->print(level);

    if (retType_)
      retType_->print(level);
  }

  void addRetType(std::unique_ptr<Type> type) {
    retType_ = std::move(type);
  }

  void addParams(std::vector<std::unique_ptr<Param>> params) {
    params_ = std::move(params);
  }

  void addBody(std::unique_ptr<Stmt> body) {
    body_ = std::move(body);
  }

 public:
  std::unique_ptr<Type> retType_;               // return type
  std::vector<std::unique_ptr<Param>> params_;  // parameters
  std::unique_ptr<Stmt> body_;                  // body, surrounded by "{" and "}"
};

// Function Definition node
class FnDef : public MethodDef {
 public:
  explicit FnDef(const std::shared_ptr<Basic::Token> &tok) : MethodDef(tok) { token_ = tok; }

  std::string kind() const override {
    std::stringstream ss;
    ss << "FnDef" << " (" << Def::getName() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    MethodDef::print(level); level++;
    body_->print(level);
  }
};

// Constructor Definition node
class CtorDef : public MethodDef {
 public:
  explicit CtorDef(const std::shared_ptr<Basic::Token> &tok)
      : MethodDef(tok) { token_ = tok; }

  std::string kind() const override {
    std::stringstream ss;
    ss << "CtorDef" << " (" << Def::getName() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    MethodDef::print(level); level++;
    body_->print(level);
  }
};

// Destructor Definition node
class DtorDef : public MethodDef {
 public:
  explicit DtorDef(const std::shared_ptr<Basic::Token> &tok)
      : MethodDef(tok) { token_ = tok; }

  std::string kind() const override {
    std::stringstream ss;
    ss << "DtorDef" << " (" << Def::getName() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Def::print(level); level++;
    body_->print(level);
  }
};

// Field Definition node
class Field : public Root {
 public:
  explicit Field(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  Field() = default;

  std::string kind() const override {
    std::stringstream ss;
    ss << "Field" << " (" << name_->getValueStr() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Root::print(level); level++;

    if (type_) type_->print(level);
    if (value_) value_->print(level);
  }

  void addType(std::unique_ptr<Type> varType) {
    type_ = std::move(varType);
  }

  void addName(std::shared_ptr<Basic::Token> varName) {
    name_ = varName;
  }

  void addValue(std::unique_ptr<Expr> varValue) {
    value_ = std::move(varValue);
  }

 public:
  std::unique_ptr<Type> type_;          // variable type
  std::shared_ptr<Basic::Token> name_;  // variable name
  std::unique_ptr<Expr> value_;         // variable value
};

// Class Definition node
class ClassDef : public Def {
 public:
  explicit ClassDef(const std::shared_ptr<Basic::Token> &tok)
      : Def(tok) { token_ = tok; }

  std::string kind() const override {
    std::stringstream ss;
    ss << "ClassDef" << " (" << Def::getName() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Def::print(level); level++;

    if (ctor_) ctor_->print(level);
    if (dtor_) dtor_->print(level);

    for (const auto &method : methods_)
      method->print(level);
    for (const auto &field : fields_)
      field->print(level);
  }

  void addConstructor(std::unique_ptr<Def> ctor) {
    ctor_ = std::move(ctor);
  }

  void addDestructor(std::unique_ptr<Def> dtor) {
    dtor_ = std::move(dtor);
  }

  void addMethod(std::unique_ptr<Def> method) {
    methods_.push_back(std::move(method));
  }

  void addField(std::unique_ptr<Field> field) {
    fields_.push_back(std::move(field));
  }

 public:
  std::unique_ptr<Def> ctor_;                   // constructor
  std::unique_ptr<Def> dtor_;                   // destructor
  std::vector<std::unique_ptr<Def>> methods_;   // methods
  std::vector<std::unique_ptr<Field>> fields_;  // fields
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_DEFINITIONS_H
