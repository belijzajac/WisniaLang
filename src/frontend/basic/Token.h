#ifndef TOKEN_H
#define TOKEN_H

#include <memory>
#include <string>
#include <variant>
// Wisnia
#include "Exceptions.h"
#include "PositionInFile.h"
#include "TType.h"

namespace Wisnia::Basic {

// Variant that holds all the possible values for token
using TokenValue = std::variant<int, float, bool, std::string, nullptr_t>;

// Helper type for the visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Token {
 public:
  Token(TType type, const TokenValue &value, std::unique_ptr<PositionInFile> pif)
      : type_{type}, value_{value}, pif_{std::move(pif)} {}

  Token(TType type, const TokenValue &value, const PositionInFile &pif)
      : type_{type}, value_{value}, pif_{std::make_unique<PositionInFile>(pif)} {}

  template <typename T>
  T getValue() const {
    try {
      return std::get<T>(value_);
    } catch (const std::bad_variant_access &ex) {
      throw Wisnia::Utils::TokenError{ex.what()};
    }
  }

  // Primarily used in AST output for pretty token's value printing
  std::string getValueStr() const {
    std::string result{};
    std::visit(overloaded{
                   [&](const std::string &arg) { result = (type_ == TType::LIT_STR) ? "\"" + arg + "\"" : arg; },
                   [&](int arg) { result = std::to_string(arg); },
                   [&](float arg) { result = std::to_string(arg); },
                   [&](bool arg) { result = arg ? "true" : "false"; },
                   [&](nullptr_t arg) { result = "null"; },
               },
               value_);
    return result;
  };

  TType getType() const { return type_; }
  std::string &getName() const { return TokenType2Str[type_]; }
  PositionInFile &getFileInfo() const { return *pif_; }

 private:
  TType type_;
  TokenValue value_;
  std::unique_ptr<PositionInFile> pif_;
};

}  // namespace Wisnia::Basic

#endif  // TOKEN_H
