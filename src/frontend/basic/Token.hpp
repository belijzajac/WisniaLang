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

#ifndef WISNIALANG_TOKEN_HPP
#define WISNIALANG_TOKEN_HPP

#include <memory>
#include <string>
#include <variant>
// Wisnia
#include "Exceptions.hpp"
#include "Position.hpp"
#include "TType.hpp"

namespace Wisnia::Basic {

// Variant that holds all the possible values for token
using TokenValue = std::variant<int, float, bool, std::string, nullptr_t>;

// Helper type for the visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Token {
 public:
  Token(const TType &type, const TokenValue &value, std::unique_ptr<Position> pif = nullptr)
      : m_type{type}, m_value{value}, m_position{std::move(pif)} {}

  Token(const TType &type, const TokenValue &value, const Position &pif)
      : m_type{type}, m_value{value}, m_position{std::make_unique<Position>(pif)} {}

  template <typename T>
  T getValue() const {
    try {
      return std::get<T>(m_value);
    } catch (const std::bad_variant_access &ex) {
      throw Wisnia::TokenError{ex.what()};
    }
  }

  // Primarily used in AST output for pretty token's value printing
  std::string getASTValueStr() const {
    std::string strResult{};
    std::visit(overloaded {
      [&](const std::string& arg) {
        if (m_type == TType::LIT_STR || m_type == TType::IDENT_STRING) {
          std::string temp{};
          for (const auto ch : arg) {
            switch (ch) {
              case '\f': temp += "\\f"; break;
              case '\r': temp += "\\r"; break;
              case '\t': temp += "\\t"; break;
              case '\v': temp += "\\v"; break;
              case '\n': temp += "\\n"; break;
              case '\"': temp += "\\"; break;
              default  : temp += ch;
            }
          }
          strResult = "\"" + temp + "\"";
        } else {
          strResult = arg;
        }
      },
      [&](int arg)       { strResult = std::to_string(arg); },
      [&](float arg)     { strResult = std::to_string(arg); },
      [&](bool arg)      { strResult = arg ? "true" : "false"; },
      [&](nullptr_t arg) { strResult = "null"; },
    },
    m_value);
    return strResult;
  };

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
