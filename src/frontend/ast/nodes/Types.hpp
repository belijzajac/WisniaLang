// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0
  
#ifndef WISNIALANG_AST_TYPES_HPP
#define WISNIALANG_AST_TYPES_HPP

#include <sstream>
// Wisnia
#include "Root.hpp"
#include "TType.hpp"
#include "Token.hpp"
#include "Exceptions.hpp"

namespace Wisnia {
namespace AST {

class BaseType : public Root {
 public:
  explicit BaseType(const std::shared_ptr<Basic::Token> &tok)
      : Root(tok) { m_strType = typeToStr(getType()); }

  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    Root::print(output, level);
  }

  Basic::TType getType() const {
    return m_token->getType();
  }

  const std::string &getStrType() const {
    return m_strType;
  }

 private:
  // Returns a string representation of the enum
  std::string typeToStr(Basic::TType type) {
      switch (type) {
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
  }

 private:
  std::string m_strType;
};

class PrimitiveType : public BaseType {
 public:
  explicit PrimitiveType(const std::shared_ptr<Basic::Token> &tok)
      : BaseType(tok) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "PrimitiveType" << " (" << getStrType() << ")";
    return ss.str();
  }

  void print(std::ostream &output, size_t level) const override {
    BaseType::print(output, level);
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_TYPES_HPP
