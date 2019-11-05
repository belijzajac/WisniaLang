#ifndef TOKEN_H
#define TOKEN_H

#include "TokenType.h"
#include <variant>
#include <memory>
#include <string>

class PositionInFile {
public:
    PositionInFile(const std::string &file, int line)
        : fileName_{file}, lineNo_{line} {}

    ~PositionInFile() = default;

    // Getters for file information
    const std::string getFileName() const { return fileName_; }
    int getLineNo() const { return lineNo_; }

private:
    std::string fileName_;
    int lineNo_;
};

// Variant, that holds all the possible values
using TokenValue = std::variant<int, float, std::string, nullptr_t>;

// Helper type for the visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class Token {
public:
    Token(TokenType type, const TokenValue &value, std::unique_ptr<PositionInFile> pif)
        : type_{type}, value_{value}, pif_{std::move(pif)} {}

    ~Token() = default;

    // Getters for token
    const TokenType getType() const { return type_; }
    const std::string getName() const { return TokenTypeToStr[type_]; }

    // Returns a string representation of value_
    const std::string getValueStr() const {
        std::string valueStr;

        // Visit the variant (type-matching visitor)
        std::visit(overloaded {
            [&](int arg) { valueStr = std::to_string(arg); },
            [&](float arg) { valueStr = std::to_string(arg); },
            [&](const std::string& arg) { valueStr = arg; },
            [&](nullptr_t arg) { valueStr = ""; }
        }, value_);

        return valueStr;
    }

    // Getter for file information
    const PositionInFile *getFileInfo() const { return pif_.get(); }

private:
    TokenType type_;
    TokenValue value_;
    std::unique_ptr<PositionInFile> pif_;
};

#endif // TOKEN_H
