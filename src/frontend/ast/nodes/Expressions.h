#ifndef AST_EXPRESSIONS_H
#define AST_EXPRESSIONS_H

// Wisnia
#include "Root.h"
#include "Types.h"
#include "TType.h"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

// An abstract definition for Expr node
class Expr : public Root {
 public:
  void print(size_t level) const override {
    Root::print(level);
  }

  void accept(Visitor *v) override = 0;
};

class VarExpr : public Expr {
 public:
  explicit VarExpr(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  VarExpr() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return fmt::format("Var (name={0}, type={1})",
                       token_->getValueStr(),
                       type_ ? type_->typeStr_ : "null");
  }

  void print(size_t level) const override {
    Root::print(level);
  }

  void addType(std::unique_ptr<Type> type) {
    type_ = std::move(type);
  }

 public:
  std::unique_ptr<Type> type_;  // int, bool, float, str, later fn ...
};

// Binary Expression node
class BinaryExpr : public Expr {
 public:
  explicit BinaryExpr(Basic::TType opType) {
    op_ = opType;
    convertOpToStr();
  }

 private:
  void convertOpToStr() {
    // Returns a string equivalent of an enum
    auto opTypeStr = [&]() -> std::string {
      switch (op_) {
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

    opStr_ = opTypeStr();
  }

 public:
  void accept(Visitor *v) override {
    v->visit(this);
  }

  void print(size_t level) const override {
    Root::print(level); level++;
    lhs()->print(level);
    rhs()->print(level);
  }

  Expr *lhs() const { return static_cast<Expr *>(first()); }
  Expr *rhs() const { return static_cast<Expr *>(second()); }

 public:
  Basic::TType op_;    // operand for expression (+, *, &&, ...)
  std::string opStr_;  // a string representation of an operand
};

// Boolean Expression node
class BooleanExpr : public BinaryExpr {
 public:
  explicit BooleanExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "BooleanExpr" << " (" << opStr_ << ")";
    return ss.str();
  }
};

// Equality Expression node
class EqExpr : public BinaryExpr {
 public:
  explicit EqExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "EqExpr" << " (" << opStr_ << ")";
    return ss.str();
  }
};

// Comparison Expression node
class CompExpr : public BinaryExpr {
 public:
  explicit CompExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "CompExpr" << " (" << opStr_ << ")";
    return ss.str();
  }
};

// Addition (and subtraction) Expression node
class AddExpr : public BinaryExpr {
 public:
  explicit AddExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "AddExpr" << " (" << opStr_ << ")";
    return ss.str();
  }
};

// Multiplication (and division) Expression node
class MultExpr : public BinaryExpr {
 public:
  explicit MultExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "MultExpr" << " (" << opStr_ << ")";
    return ss.str();
  }
};

// Unary Expression node
class UnaryExpr : public BinaryExpr {
 public:
  explicit UnaryExpr(Basic::TType opType) : BinaryExpr(opType) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "UnaryExpr" << " (" << opStr_ << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Root::print(level); level++;
    // UnaryExpr especially holds only the lhs value, e.g. children_[0]
    lhs()->print(level);
  }
};

// Function Call Expression node
class FnCallExpr : public Expr {
 public:
  explicit FnCallExpr(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  FnCallExpr() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return className_
        ? fmt::format("FnCallExpr ({}::{})", className_->getValueStr(), token_->getValueStr())
        : fmt::format("FnCallExpr ({})", token_->getValueStr());
  }

  void print(size_t level) const override {
    Expr::print(level); level++;
    for (const auto &arg : args_)
      arg->print(level);
  }

  void addClassName(std::shared_ptr<Basic::Token> className) {
    className_ = className;
  }

  void addArgs(std::vector<std::unique_ptr<Expr>> args) {
    args_ = std::move(args);
  }

  std::shared_ptr<Basic::Token> getClassName() const { return className_; }
  std::shared_ptr<Basic::Token> getFnName() const { return token_; }

 public:
  std::shared_ptr<Basic::Token> className_;
  std::vector<std::unique_ptr<Expr>> args_;
};

// Function Expression node
class ClassInitExpr : public Expr {
 public:
  explicit ClassInitExpr(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "ClassInitExpr" << " (" << token_->getValueStr() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Expr::print(level); level++;
    for (const auto &arg : args_)
      arg->print(level);
  }

  void addArgs(std::vector<std::unique_ptr<Expr>> args) {
    args_ = std::move(args);
  }

 public:
  std::vector<std::unique_ptr<Expr>> args_;
};

// An abstract definition for constant expression node
class ConstExpr : public Expr {
 public:
  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "ConstExpr" << " (" << token_->getValueStr() << ")";
    return ss.str();
  }
};

// A node for Integer expression
class IntExpr : public ConstExpr {
 public:
  explicit IntExpr(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  IntExpr() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

// A node for Float expression
class FloatExpr : public ConstExpr {
 public:
  explicit FloatExpr(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  FloatExpr() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

// A node for Bool expression
class BoolExpr : public ConstExpr {
 public:
  explicit BoolExpr(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  BoolExpr() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

// A node for String expression
class StringExpr : public ConstExpr {
 public:
  explicit StringExpr(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  StringExpr() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_EXPRESSIONS_H
