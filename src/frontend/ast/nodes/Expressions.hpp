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
  std::vector<std::unique_ptr<BaseExpr>> m_children; // lhs and rhs
};

class VarExpr : public BaseExpr {
 public:
  explicit VarExpr(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return fmt::format("Var (name={0}, type={1})", m_token->getValueStr(),
                       m_type ? m_type->m_strType : "null");
  }

  void print(size_t level) const override {
    BaseExpr::print(level);
  }

  void addType(std::unique_ptr<BaseType> type) {
    m_type = std::move(type);
  }

 public:
  std::unique_ptr<BaseType> m_type;
};

class BinaryExpr : public BaseExpr {
 public:
  explicit BinaryExpr(Basic::TType opType) {
    m_operand = opType;
    convertOpToStr();
  }

 private:
  void convertOpToStr() {
    // Returns a string equivalent of an enum
    auto opTypeStr = [&]() -> std::string {
      switch (m_operand) {
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

 public:
  void accept(Visitor *v) override {
    v->visit(this);
  }

  void print(size_t level) const override {
    BaseExpr::print(level++);
    lhs()->print(level);
    rhs()->print(level);
  }

 public:
  Basic::TType m_operand;   // operand for expression (+, *, &&, ...)
  std::string m_strOperand; // a string representation of an operand
};

class BooleanExpr : public BinaryExpr {
 public:
  explicit BooleanExpr(Basic::TType opType) : BinaryExpr(opType) {}

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
  explicit EqExpr(Basic::TType opType) : BinaryExpr(opType) {}

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
  explicit CompExpr(Basic::TType opType) : BinaryExpr(opType) {}

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
  explicit AddExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "AddExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class MultExpr : public BinaryExpr {
 public:
  explicit MultExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "MultExpr" << " (" << m_strOperand << ")";
    return ss.str();
  }
};

class UnaryExpr : public BinaryExpr {
 public:
  explicit UnaryExpr(Basic::TType opType) : BinaryExpr(opType) {}

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
  explicit FnCallExpr(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }
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

  void addClassName(std::shared_ptr<Basic::Token> className) {
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

  std::shared_ptr<Basic::Token> getClassName() const { return m_className; }
  std::shared_ptr<Basic::Token> getFnName() const { return m_token; }

 public:
  std::shared_ptr<Basic::Token> m_className;
  std::vector<std::unique_ptr<BaseExpr>> m_args;
  std::unique_ptr<BaseExpr> m_var;
};

class ClassInitExpr : public BaseExpr {
 public:
  explicit ClassInitExpr(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

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

 public:
  std::vector<std::unique_ptr<BaseExpr>> m_args;
  std::unique_ptr<BaseExpr> m_var;
};

class ConstExpr : public BaseExpr {
 public:
  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "ConstExpr" << " (" << m_token->getValueStr() << ")";
    return ss.str();
  }
};

class IntExpr : public ConstExpr {
 public:
  explicit IntExpr(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

class FloatExpr : public ConstExpr {
 public:
  explicit FloatExpr(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

class BoolExpr : public ConstExpr {
 public:
  explicit BoolExpr(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

class StringExpr : public ConstExpr {
 public:
  explicit StringExpr(const std::shared_ptr<Basic::Token> &tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_EXPRESSIONS_HPP
