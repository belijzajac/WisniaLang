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

class BaseType : public Root {
 public:
  explicit BaseType(const std::shared_ptr<Basic::Token> &tok) {
    m_type = tok->getType();
    convertTypeToStr();
  }

  void accept(Visitor *v) override = 0;

  void print(size_t level) const override {
    Root::print(level);
  }

 private:
  void convertTypeToStr() {
    // Returns a string representation of the enum
    auto primTypeStr = [&]() -> std::string {
      switch (m_type) {
        case Basic::TType::KW_CLASS:
          return "class";
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
          return "null";
      }
    };

    m_strType = primTypeStr();
  }

 public:
  Basic::TType m_type;   // enum representing type
  std::string m_strType; // string representation of Type (for printing)
};

class PrimitiveType : public BaseType {
 public:
  explicit PrimitiveType(const std::shared_ptr<Basic::Token> &tok) : BaseType(tok) { m_token = tok; }

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "PrimitiveType" << " (" << m_strType << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    BaseType::print(level);
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_TYPES_H
