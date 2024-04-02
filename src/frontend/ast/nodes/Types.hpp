// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0
  
#ifndef WISNIALANG_AST_TYPES_HPP
#define WISNIALANG_AST_TYPES_HPP

#include <sstream>
#include <utility>
// Wisnia
#include "Root.hpp"
#include "TType.hpp"
#include "Token.hpp"
#include "Exceptions.hpp"

namespace Wisnia {
namespace AST {

class BaseType : public Root {
 public:
  explicit BaseType(TokenPtr token)
      : Root(std::move(token)) { m_type = typeToString(getType()); }

  void accept(Visitor &) override = 0;

  void print(std::ostream &output, size_t level) const override {
    Root::print(output, level);
  }

  Basic::TType getType() const {
    return m_token->getType();
  }

  const std::string &getStringType() const {
    return m_type;
  }

 private:
  std::string typeToString(Basic::TType type) {
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
  std::string m_type;
};

class PrimitiveType : public BaseType {
 public:
  explicit PrimitiveType(TokenPtr token)
      : BaseType(std::move(token)) {}

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  std::string kind() const override {
    std::stringstream ss;
    ss << "PrimitiveType" << " (" << getStringType() << ")";
    return ss.str();
  }

  void print(std::ostream &output, size_t level) const override {
    BaseType::print(output, level);
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_TYPES_HPP
