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
      throw Wisnia::Utils::TokenError{ex.what()};
    }
  }

  // Primarily used in AST output for pretty token's value printing
  std::string getASTValueStr() const {
    std::string strResult{};
    std::visit(overloaded {
      [&](const std::string& arg) { strResult = (m_type == TType::LIT_STR) ? ("\"" + arg + "\"") : arg; },
      [&](int arg)                { strResult = std::to_string(arg); },
      [&](float arg)              { strResult = std::to_string(arg); },
      [&](bool arg)               { strResult = arg ? "true" : "false"; },
      [&](nullptr_t arg)          { strResult = "null"; },
    },
    m_value);
    return strResult;
  };

  TType getType() const { return m_type; }
  void setType(TType type) { m_type = type; }
  std::string &getName() const { return TokenType2Str[m_type]; }
  Position &getPosition() const { return *m_position; }

 private:
  TType m_type;
  TokenValue m_value;
  std::unique_ptr<Position> m_position;
};

}  // namespace Wisnia::Basic

#endif  // WISNIALANG_TOKEN_HPP
