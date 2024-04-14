// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_AST_EXPRESSIONS_HPP
#define WISNIALANG_AST_EXPRESSIONS_HPP

#include <cassert>
#include <utility>
#include "fmt/format.h"
// Wisnia
#include "Root.hpp"
#include "TType.hpp"
#include "Types.hpp"
#include "Variable.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class BaseExpr : public Root {
  using ExpressionPtr = std::unique_ptr<BaseExpr>;

 public:
  BaseExpr() = default;

  void print(std::ostream &output, size_t level) const override {
    Root::print(output, level);
  }

  void accept(Visitor &) override = 0;

  void addChild(ExpressionPtr child) {
    m_children.push_back(std::move(child));
  }

  const ExpressionPtr &lhs() const { return m_children[0]; }
  const ExpressionPtr &rhs() const { return m_children[1]; }

 protected:
  explicit BaseExpr(TokenPtr token)
      : Root(std::move(token)) {}

 protected:
  std::vector<ExpressionPtr> m_children; // lhs and rhs
};

class VarExpr : public BaseExpr {
  using TypePtr = std::unique_ptr<BaseType>;

 public:
  explicit VarExpr(TokenPtr token)
      : BaseExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return fmt::format("Var (name={}, type={})", m_token->getASTValueStr(),
                       m_type ? m_type->getStringType() : "null");
  }

  void print(std::ostream &output, size_t level) const override {
    BaseExpr::print(output, level);
  }

  void addType(TypePtr type) {
    m_type = std::move(type);
    Basic::TType tokenType;

    switch (m_type->getType()) {
      case Basic::TType::KW_VOID:
        tokenType = Basic::TType::IDENT_VOID;
        break;
      case Basic::TType::KW_INT:
        tokenType = Basic::TType::IDENT_INT;
        break;
      case Basic::TType::KW_BOOL:
        tokenType = Basic::TType::IDENT_BOOL;
        break;
      case Basic::TType::KW_FLOAT:
        tokenType = Basic::TType::IDENT_FLOAT;
        break;
      case Basic::TType::KW_STRING:
        tokenType = Basic::TType::IDENT_STRING;
        break;
      default:
        tokenType = m_type->getType();
    }
    m_token->setType(tokenType);
  }

  const TypePtr &getType() const {
    return m_type;
  }

 private:
  TypePtr m_type;
};

class BinaryExpr : public BaseExpr {
 public:
  explicit BinaryExpr(TokenPtr token)
      : BaseExpr(std::move(token)) { convertToStringOperand(); }

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  void print(std::ostream &output, size_t level) const override {
    BaseExpr::print(output, level++);
    lhs()->print(output, level);
    rhs()->print(output, level);
  }

  Basic::TType getOperand() const {
    return m_token->getType();
  }

  const std::string &getStringOperand() const {
    return m_operand;
  }

 protected:
  std::string m_operand;

 private:
  void convertToStringOperand() {
    auto stringOperand = [&]() -> std::string {
      switch (getOperand()) {
        // Arithmetic
        case Basic::TType::OP_ADD:
          return "+";
        case Basic::TType::OP_SUB:
          return "-";
        case Basic::TType::OP_MUL:
          return "*";
        case Basic::TType::OP_DIV:
          return "/";
        // Unary
        case Basic::TType::OP_UADD:
          return "++";
        case Basic::TType::OP_USUB:
          return "--";
        case Basic::TType::OP_UNEG:
          return "!";
        // Logical
        case Basic::TType::OP_AND:
          return "&&";
        case Basic::TType::OP_OR:
          return "||";
        case Basic::TType::OP_EQ:
          return "==";
        case Basic::TType::OP_L:
          return "<";
        case Basic::TType::OP_G:
          return ">";
        case Basic::TType::OP_LE:
          return "<=";
        case Basic::TType::OP_GE:
          return ">=";
        case Basic::TType::OP_NE:
          return "!=";
        default:
          throw ParserError{fmt::format("Invalid operand type in {}:{}",
                                        m_token->getPosition().getFileName(),
                                        m_token->getPosition().getLineNo())};
      }
    };

    m_operand = stringOperand();
  }
};

class BooleanExpr : public BinaryExpr {
 public:
  explicit BooleanExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "BooleanExpr" << " (" << m_operand << ")";
    return ss.str();
  }
};

class EqExpr : public BinaryExpr {
 public:
  explicit EqExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "EqExpr" << " (" << m_operand << ")";
    return ss.str();
  }
};

class CompExpr : public BinaryExpr {
 public:
  explicit CompExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "CompExpr" << " (" << m_operand << ")";
    return ss.str();
  }
};

class AddExpr : public BinaryExpr {
 public:
  explicit AddExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "AddExpr" << " (" << m_operand << ")";
    return ss.str();
  }
};

class SubExpr : public BinaryExpr {
 public:
  explicit SubExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "SubExpr" << " (" << m_operand << ")";
    return ss.str();
  }
};

class MultExpr : public BinaryExpr {
 public:
  explicit MultExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "MultExpr" << " (" << m_operand << ")";
    return ss.str();
  }
};

class DivExpr : public BinaryExpr {
 public:
  explicit DivExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "DivExpr" << " (" << m_operand << ")";
    return ss.str();
  }
};

class UnaryExpr : public BinaryExpr {
 public:
  explicit UnaryExpr(TokenPtr token)
      : BinaryExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "UnaryExpr" << " (" << m_operand << ")";
    return ss.str();
  }

  void print(std::ostream &output, size_t level) const override {
    BinaryExpr::print(output, level++);
    lhs()->print(output, level); // holds only the lhs value, that is m_children[0]
  }
};

class FnCallExpr : public BaseExpr, public VariableMixin {
  using ArgumentPtr = std::unique_ptr<BaseExpr>;

 public:
  explicit FnCallExpr(TokenPtr token)
      : BaseExpr(std::move(token)) {}

  FnCallExpr() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "FnCallExpr";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseExpr::print(output, level++);
    getVariable()->print(output, level);
    for (const auto &arg : m_arguments) {
      arg->print(output, level);
    }
  }

  void addClassName(TokenPtr className) {
    m_className = std::move(className);
  }

  void addArguments(std::vector<ArgumentPtr> arguments) {
    m_arguments = std::move(arguments);
  }

  const TokenPtr &getFunctionName() const {
    return m_token;
  }

  const TokenPtr &getClassName() const {
    return m_className;
  }

  const std::vector<ArgumentPtr> &getArguments() const {
    return m_arguments;
  }

 private:
  TokenPtr m_className;
  std::vector<ArgumentPtr> m_arguments;
};

class ClassInitExpr : public BaseExpr, public VariableMixin {
  using ArgumentPtr = std::unique_ptr<BaseExpr>;

 public:
  explicit ClassInitExpr(TokenPtr token)
      : BaseExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    return "ClassInitExpr";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseExpr::print(output, level++);
    getVariable()->print(output, level);
    for (const auto &arg : m_arguments) {
      arg->print(output, level);
    }
  }

  void addArguments(std::vector<ArgumentPtr> arguments) {
    m_arguments = std::move(arguments);
  }

  const std::vector<ArgumentPtr> &getArguments() const {
    return m_arguments;
  }

 private:
  std::vector<ArgumentPtr> m_arguments;
};

class ConstExpr : public BaseExpr {
 public:
  void accept(Visitor &v) override {
    v.visit(*this);
  }

 protected:
  explicit ConstExpr(TokenPtr token)
      : BaseExpr(std::move(token)) {}
};

class IntExpr : public ConstExpr {
 public:
  explicit IntExpr(TokenPtr token)
      : ConstExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    const auto getType = [&]() {
      switch (m_token->getType()) {
        case Basic::TType::LIT_INT:
          return "int";
        default:
          assert(0 && "Unknown integer type");
      }
    };
    return fmt::format("IntExpr (value={}, type={})", m_token->getASTValueStr(), getType());
  }
};

class FloatExpr : public ConstExpr {
 public:
  explicit FloatExpr(TokenPtr token)
      : ConstExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "FloatExpr" << " (" << m_token->getASTValueStr() << ")";
    return ss.str();
  }
};

class BoolExpr : public ConstExpr {
 public:
  explicit BoolExpr(TokenPtr token)
      : ConstExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "BoolExpr" << " (" << m_token->getASTValueStr() << ")";
    return ss.str();
  }
};

class StringExpr : public ConstExpr {
 public:
  explicit StringExpr(TokenPtr token)
      : ConstExpr(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "StringExpr" << " (" << m_token->getASTValueStr() << ")";
    return ss.str();
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_EXPRESSIONS_HPP
