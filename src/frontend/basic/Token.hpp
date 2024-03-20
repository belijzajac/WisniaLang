// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_TOKEN_HPP
#define WISNIALANG_TOKEN_HPP

#include <memory>
#include <string>
#include <variant>
// Wisnia
#include "Exceptions.hpp"
#include "Position.hpp"
#include "Register.hpp"
#include "TType.hpp"

namespace Wisnia::Basic {

// Variant that holds all the possible values for token
using TokenValue = std::variant<int, float, bool, std::string, nullptr_t, Basic::register_t>;

// Helper type for the visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Token {
 public:
  Token(TType type, const TokenValue &value, std::unique_ptr<Position> pif = nullptr)
      : m_type{type}, m_value{value}, m_position{std::move(pif)} {}

  Token(TType type, const TokenValue &value, const Position &pif)
      : m_type{type}, m_value{value}, m_position{std::make_unique<Position>(pif)} {}

  template <typename T>
  T getValue() const {
    try {
      return std::get<T>(m_value);
    } catch (const std::bad_variant_access &ex) {
      throw Wisnia::TokenError{ex.what()};
    }
  }

  std::string getValueStr() const {
    std::string strResult{};
    std::visit(overloaded {
      [&](const std::string &arg) { strResult = arg; },
      [&](int arg)                { strResult = std::to_string(arg); },
      [&](float arg)              { strResult = std::to_string(arg); },
      [&](bool arg)               { strResult = arg ? "true" : "false"; },
      [&](nullptr_t arg)          { strResult = "null"; },
      [&](Basic::register_t arg)  { strResult = Register2Str[arg]; },
    },
    m_value);
    return strResult;
  };

  // Primarily used in AST output for pretty token's value printing
  std::string getASTValueStr() const {
    std::string strResult{};
    std::visit(overloaded {
      [&](const std::string &arg) {
        if (m_type == TType::LIT_STR) {
          std::string temp{};
          for (const auto ch : arg) {
            switch (ch) {
              case '\f': temp += "\\f"; break;
              case '\r': temp += "\\r"; break;
              case '\t': temp += "\\t"; break;
              case '\v': temp += "\\v"; break;
              case '\n': temp += "\\n"; break;
              case '\"': temp += "\\"; break;
              case '\0': temp += "\\0"; break;
              default  : temp += ch;
            }
          }
          strResult = "\"" + temp + "\"";
        } else {
          strResult = arg;
        }
      },
      [&](int arg)               { strResult = std::to_string(arg); },
      [&](float arg)             { strResult = std::to_string(arg); },
      [&](bool arg)              { strResult = arg ? "true" : "false"; },
      [&](nullptr_t arg)         { strResult = "null"; },
      [&](Basic::register_t arg) { strResult = Register2Str[arg]; },
    },
    m_value);
    return strResult;
  };

  constexpr bool isIdentifierType() const {
    return m_type == TType::IDENT_INT    || m_type == TType::IDENT_FLOAT ||
           m_type == TType::IDENT_STRING || m_type == TType::IDENT_BOOL;
  }

  constexpr bool isLiteralType() const {
    return m_type == TType::LIT_INT  || m_type == TType::LIT_FLT || m_type == TType::LIT_STR ||
           m_type == TType::LIT_BOOL || m_type == TType::KW_TRUE || m_type == TType::KW_FALSE;
  }

  constexpr bool isLiteralIntegerType() const {
    return m_type == TType::LIT_INT;
  }

  TType getType() const { return m_type; }
  void setType(TType type) { m_type = type; }
  void setValue(const TokenValue &value) { m_value = value; }
  std::string &getName() const { return TokenType2Str[m_type]; }
  Position &getPosition() const { return *m_position; }

 private:
  TType m_type;
  TokenValue m_value;
  std::unique_ptr<Position> m_position;
};

}  // namespace Wisnia::Basic

#endif  // WISNIALANG_TOKEN_HPP
