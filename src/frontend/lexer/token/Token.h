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

//using TokenValue = std::variant<int, float, std::string>;

// Move to `class token`
/*TokenValue getTokenValue() {
    // TODO: std::visit


}*/

class Token {
public:
    Token(TokenType type, const std::string &value, std::unique_ptr<PositionInFile> pif)
            : type_{type}, value_{value}, pif_{std::move(pif)} {}

    /*Token(TokenType type, const TokenValue &value, std::unique_ptr<PositionInFile> pif)
        : type_{type}, value_{value}, pif_{std::move(pif)} {}*/

    ~Token() = default;

    // Getters for token
    const TokenType getType() const { return type_; }
    const std::string getName() const { return TokenTypeToStr[type_]; }
    const std::string getValue() const { return value_; }

    // Getter for file information
    const PositionInFile *getFileInfo() const { return pif_.get(); }

private:
    TokenType type_;
    std::string value_;
    //TokenValue value_;

    std::unique_ptr<PositionInFile> pif_;
};

#endif // TOKEN_H
