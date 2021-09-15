#ifndef AST_TYPES_H
#define AST_TYPES_H

#include <sstream>
// Wisnia
#include "Root.h"
#include "TType.h"
#include "Token.h"
#include "Exceptions.h"

namespace Wisnia {
namespace AST {
// An abstract definition for Type node
class Type : public Root {
 public:
  explicit Type(const std::shared_ptr<Basic::Token> &tok) {
    type_ = tok->getType();
    convertTypeToStr();
  }

  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    Root::print(level);
  }

 private:
  void convertTypeToStr() {
    // Returns a string equivalent of an enum
    auto primTypeStr = [&]() -> std::string {
      switch (type_) {
        case Basic::TType::KW_VOID:
          return "void";
        case Basic::TType::KW_INT:
          return "int";
        case Basic::TType::KW_BOOL:
          return "bool";
        case Basic::TType::KW_FLOAT:
          return "float";
        case Basic::TType::KW_STRING:
          return "string";
        default:
          throw Utils::NotImplementedError{Basic::TokenType2Str[type_]};
      }
    };

    typeStr_ = primTypeStr();
  }

 public:
  Basic::TType type_;    // enum representing type
  std::string typeStr_;  // string representation of Type (for printing)
};

// Function Type node
class PrimitiveType : public Type {
 public:
  explicit PrimitiveType(const std::shared_ptr<Basic::Token> &tok) : Type(tok) { token_ = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "PrimitiveType" << " (" << typeStr_ << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Root::print(level);
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_TYPES_H
