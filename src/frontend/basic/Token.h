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

// Variant, that holds all the possible values
using TokenValue = std::variant<int, float, std::string, nullptr_t>;

// Helper type for the visitor
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

class Token {
 public:
  Token(TType type, const TokenValue &value,
        std::unique_ptr<PositionInFile> pif)
      : type_{type}, value_{value}, pif_{std::move(pif)} {}

  ~Token() = default;

  template <typename T>
  T getValue() {
    try {
      return std::get<T>(value_);
    } catch (const std::bad_variant_access &ex) {
      throw Wisnia::Utils::TokenError{ex.what()};
    }
  }

  TType getType() const { return type_; }
  std::string getName() const { return TokenTypeToStr[type_]; }
  const PositionInFile *getFileInfo() const { return pif_.get(); }

 private:
  TType type_;
  TokenValue value_;
  std::unique_ptr<PositionInFile> pif_;
};

}  // namespace Wisnia::Basic

#endif  // TOKEN_H
