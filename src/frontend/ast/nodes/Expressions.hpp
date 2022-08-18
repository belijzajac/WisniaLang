/***

  WisniaLang - A Compiler for an Experimental Programming Language
  Copyright (C) 2022 Tautvydas Povilaitis (belijzajac) and contributors

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

***/

#ifndef WISNIALANG_AST_EXPRESSIONS_HPP
#define WISNIALANG_AST_EXPRESSIONS_HPP

// Wisnia
#include "Root.hpp"
#include "Types.hpp"
#include "TType.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class BaseExpr : public Root {
 public:
  BaseExpr() = default;

  void print(size_t level) const override {
    Root::print(level);
  }

  void accept(Visitor *v) override = 0;

  void addChild(std::unique_ptr<BaseExpr> child) {
    m_children.push_back(std::move(child));
  }

  BaseExpr *lhs() const { return m_children[0].get(); }
  BaseExpr *rhs() const { return m_children[1].get(); }

 protected:
  explicit BaseExpr(const std::shared_ptr<Basic::Token> &tok)
      : Root(tok) {}

 protected:
  std::vector<std::unique_ptr<BaseExpr>> m_children; // lhs and rhs
};

class VarExpr : public BaseExpr {
 public:
  explicit VarExpr(const std::shared_ptr<Basic::Token> &tok)
      : BaseExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return fmt::format("Var (name={}, type={})", m_token->getASTValueStr(),
                       m_type ? m_type->getStrType() : "null");
  }

  void print(size_t level) const override {
    BaseExpr::print(level);
  }

  void addType(std::unique_ptr<BaseType> type) {
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

  const std::unique_ptr<BaseType> &getType() const {
    return m_type;
  }

 private:
  std::unique_ptr<BaseType> m_type;
};

class BinaryExpr : public BaseExpr {
 public:
  explicit BinaryExpr(const std::shared_ptr<Basic::Token> &tok)
      : BaseExpr(tok) { convertOpToStr(); }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  void print(size_t level) const override {
    BaseExpr::print(level++);
    lhs()->print(level);
    rhs()->print(level);
  }

  Basic::TType getOperand() const {
    return m_token->getType();
  }

  const std::string &getStrOperand() const {
    return m_strOperand;
  }

 protected:
  std::string m_strOperand;

 private:
  void convertOpToStr() {
    // Returns a string equivalent of an enum
    auto opTypeStr = [&]() -> std::string {
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
          throw Utils::ParserError{"Invalid operand type"};
      }
    };

    m_strOperand = opTypeStr();
  }
};

class BooleanExpr : public BinaryExpr {
 public:
  explicit BooleanExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "BooleanExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class EqExpr : public BinaryExpr {
 public:
  explicit EqExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "EqExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class CompExpr : public BinaryExpr {
 public:
  explicit CompExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "CompExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class AddExpr : public BinaryExpr {
 public:
  explicit AddExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "AddExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class SubExpr : public BinaryExpr {
 public:
  explicit SubExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "SubExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class MultExpr : public BinaryExpr {
 public:
  explicit MultExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "MultExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class DivExpr : public BinaryExpr {
 public:
  explicit DivExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "DivExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class UnaryExpr : public BinaryExpr {
 public:
  explicit UnaryExpr(const std::shared_ptr<Basic::Token> &tok)
      : BinaryExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "UnaryExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Root::print(level++);
    lhs()->print(level); // holds only the lhs value, that is m_children[0]
  }
};

class FnCallExpr : public BaseExpr {
 public:
  explicit FnCallExpr(const std::shared_ptr<Basic::Token> &tok)
      : BaseExpr(tok) {}

  FnCallExpr() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "FnCallExpr";
  }

  void print(size_t level) const override {
    BaseExpr::print(level++);
    m_var->print(level);
    for (const auto &arg : m_args)
      arg->print(level);
  }

  void addClassName(const std::shared_ptr<Basic::Token> &className) {
    m_className = className;
  }

  void addArgs(std::vector<std::unique_ptr<BaseExpr>> args) {
    m_args = std::move(args);
  }

  void addType(std::unique_ptr<BaseType> type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_var = std::move(var);
  }

  const std::shared_ptr<Basic::Token> &getFnName() const {
    return m_token;
  }

  const std::shared_ptr<Basic::Token> &getClassName() const {
    return m_className;
  }

  const std::vector<std::unique_ptr<BaseExpr>> &getArgs() const {
    return m_args;
  }

  const std::unique_ptr<BaseExpr> &getVar() const {
    return m_var;
  }

 private:
  std::shared_ptr<Basic::Token> m_className;
  std::vector<std::unique_ptr<BaseExpr>> m_args;
  std::unique_ptr<BaseExpr> m_var;
};

class ClassInitExpr : public BaseExpr {
 public:
  explicit ClassInitExpr(const std::shared_ptr<Basic::Token> &tok)
      : BaseExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ClassInitExpr";
  }

  void print(size_t level) const override {
    BaseExpr::print(level++);
    m_var->print(level);
    for (const auto &arg : m_args)
      arg->print(level);
  }

  void addArgs(std::vector<std::unique_ptr<BaseExpr>> args) {
    m_args = std::move(args);
  }

  void addType(std::unique_ptr<BaseType> type) const {
    if (auto varPtr = dynamic_cast<AST::VarExpr*>(m_var.get())) {
      varPtr->addType(std::move(type));
    }
  }

  void addVar(std::unique_ptr<BaseExpr> var) {
    m_var = std::move(var);
  }

  const std::vector<std::unique_ptr<BaseExpr>> &getArgs() const {
    return m_args;
  }

  const std::unique_ptr<BaseExpr> &getVar() const {
    return m_var;
  }

 private:
  std::vector<std::unique_ptr<BaseExpr>> m_args;
  std::unique_ptr<BaseExpr> m_var;
};

class ConstExpr : public BaseExpr {
 public:
  void accept(Visitor *v) override {
    v->visit(this);
  }

 protected:
  explicit ConstExpr(const std::shared_ptr<Basic::Token> &tok)
      : BaseExpr(tok) {}
};

class IntExpr : public ConstExpr {
 public:
  explicit IntExpr(const std::shared_ptr<Basic::Token> &tok)
      : ConstExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "IntExpr" << " (" << m_token->getASTValueStr() << ")";
    return ss.str();
  }
};

class FloatExpr : public ConstExpr {
 public:
  explicit FloatExpr(const std::shared_ptr<Basic::Token> &tok)
      : ConstExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "FloatExpr" << " (" << m_token->getASTValueStr() << ")";
    return ss.str();
  }
};

class BoolExpr : public ConstExpr {
 public:
  explicit BoolExpr(const std::shared_ptr<Basic::Token> &tok)
      : ConstExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "BoolExpr" << " (" << m_token->getASTValueStr() << ")";
    return ss.str();
  }
};

class StringExpr : public ConstExpr {
 public:
  explicit StringExpr(const std::shared_ptr<Basic::Token> &tok)
      : ConstExpr(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
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
