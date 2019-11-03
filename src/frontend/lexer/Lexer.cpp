#include "Lexer.h"
#include "token/Token.h"
#include "../../utilities/Exception.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

// Operands of length 1
static inline std::vector<char> simpleOperands {'.', '*', '(', ')', '{', '}', ',', ':', ';'};

// Finish tokenize token
std::shared_ptr<Token> Lexer::finishTok(const TokenType &type_, bool backtrack) {
    tokenState_.state_ = START;

    // We've over-gone by 1 character further by returning the token earlier,
    // so step back the iterator
    if (backtrack)
        --tokenState_.it_;

    std::unique_ptr<PositionInFile> pif_;
    // Construct token
    if (type_ == TokenType::LIT_STR)
        pif_ = std::make_unique<PositionInFile>(tokenState_.fileName_, tokenState_.stringStart);
    else
        pif_ = std::make_unique<PositionInFile>(tokenState_.fileName_, tokenState_.lineNo);

    auto token = std::make_shared<Token>(type_, tokenState_.buff_, std::move(pif_));

    // Clear existing token buffer and return parsed token
    tokenState_.buff_.clear();
    return token;
}

// Finish tokenize identifier
std::shared_ptr<Token> Lexer::finishIdent() {
    // It's either a known keyword
    if (auto search = StrToTokenKw.find(tokenState_.buff_); search != StrToTokenKw.end())
        return finishTok(search->second, true);
    // Or simply an identifier
    else
        return finishTok(TokenType::IDENT, true);
}

// Tokenize the following character
// TODO: std::optional ?
// TODO: a vector of possible states of type std::function<void(bool)> tokMyState ???
std::shared_ptr<Token> Lexer::tokNext(char ch) {
    switch (tokenState_.state_) {

        /* ~~~ CASE: START ~~~ */
        case START:
            if (isalpha(ch) || ch == '_') {
                tokenState_.state_ = IDENT;
                tokenState_.buff_ += ch;
            }

            else if (ch == '!' || ch == '<' || ch == '>' || ch == '=') {
                tokenState_.buff_ += ch;
                tokenState_.state_ = OP_COMPARE;
            }

            else if (ch == '\"') {
                tokenState_.stringStart = tokenState_.lineNo;
                tokenState_.state_ = STRING;
            }

            else if (isdigit(ch)) {
                tokenState_.state_ = INTEGER;
                tokenState_.buff_ += ch;
            }

            else if (ch == '&')
                tokenState_.state_ = LOGIC_AND;

            else if (ch == '|')
                tokenState_.state_ = LOGIC_OR;

            else if (ch == '+')
                tokenState_.state_ = OP_PP;

            else if (ch == '-')
                tokenState_.state_ = OP_ARROW;

            else if (ch == '/') {
                tokenState_.state_ = CMT_MAYBE_MULTI_CMT;
                return nullptr;
            }

            else if (ch == '#')
                tokenState_.state_ = CMT_SINGLE;

            // Finish the below tokens instantly
            else if (std::any_of(simpleOperands.begin(), simpleOperands.end(), [&](char op){ return ch == op; })) {
                std::string opToStr {ch};
                return finishTok(StrToTokenOp[opToStr]);
            }

            // Skip white-spaces
            else if (isspace(ch)) {
                if (ch == '\n')
                    ++tokenState_.lineNo;
                return nullptr;
            }

            else
                throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": Unrecognized char"};

            return nullptr;

        /* ~~~ CASE: IDENT ~~~ */
        case IDENT:
            if (isspace(ch))
                return finishIdent();

            if (!isalpha(ch) && !isdigit(ch) && ch != '_')
                return finishIdent();

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: OP_COMPARE ~~~ */
        case OP_COMPARE:
            if (ch == '=') {
                std::string tokKey {tokenState_.buff_ + '='};
                tokenState_.buff_.clear();
                return finishTok(StrToTokenOp[tokKey]); // (!=, <=, >=, ==)
            } else {
                std::string tokKey {tokenState_.buff_};
                tokenState_.buff_.clear();
                return finishTok(StrToTokenOp[tokKey], true); // (!, <, >, =)
            }

        /* ~~~ CASE: STRING ~~~ */
        case STRING: {
            // We got an escape symbol -- need to escape it
            if (ch == '\\')
                tokenState_.state_ = ESCAPE_SEQ;

            // It's some other symbol
            else {
                if (ch == '\n')
                    ++tokenState_.lineNo;

                else if (ch == '\"')
                    return finishTok(TokenType::LIT_STR);

                else if (ch == -1)
                    throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": Unterminated string"};

                tokenState_.buff_ += ch;
            }
            return nullptr;
        }

        /* ~~~ CASE: ESCAPE_SEQ ~~~ */
        case ESCAPE_SEQ:
            // Escape sequences for strings
            switch (ch) {
                case 'f':
                    tokenState_.buff_ += '\f';
                    break;
                case 'r':
                    tokenState_.buff_ += '\r';
                    break;
                case 't':
                    tokenState_.buff_ += '\t';
                    break;
                case 'v':
                    tokenState_.buff_ += '\v';
                    break;
                case 'n':
                    tokenState_.buff_ += '\n';
                    break;
                case '\"':
                    tokenState_.buff_ += '\"';
                    break;
                default:
                    tokenState_.buff_ += ch;
                    throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": Unknown escape symbol"};
            }
            // Continue parsing
            tokenState_.state_ = STRING;
            return nullptr;

        /* ~~~ CASE: INTEGER ~~~ */
        case INTEGER:
            if (isspace(ch))
                return finishTok(TokenType::LIT_INT);

            else if (ch == '.')
                tokenState_.state_ = FLOAT;

            else if (isalpha(ch)) {
                tokenState_.state_ = ERRONEOUS_NUMBER;
                tokenState_.erroneousType_ = "integer";
            }

            else if(!isdigit(ch))
                return finishTok(TokenType::LIT_INT, true);

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: FLOAT ~~~ */
        case FLOAT:
            if (isspace(ch))
                return finishTok(TokenType::LIT_FLT);

            else if (isalpha(ch) || ch == '.') {
                tokenState_.state_ = ERRONEOUS_NUMBER;
                tokenState_.erroneousType_ = "float";
            }

            else if (!isdigit(ch))
                return finishTok(TokenType::LIT_FLT, true);

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: ERRONEOUS_NUMBER ~~~ */
        case ERRONEOUS_NUMBER:
            if (isspace(ch)) {
                if (ch == '\n')
                    ++tokenState_.lineNo;

                return finishTok(TokenType::TOK_INVALID);
            }

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: LOGIC_AND ~~~ */
        case LOGIC_AND:
            if (ch == '&')
                return finishTok(TokenType::OP_AND);

            tokenState_.erroneousType_ = "ampersand";
            return finishTok(TokenType::TOK_INVALID, true);

        /* ~~~ CASE: LOGIC_OR ~~~ */
        case LOGIC_OR:
            if (ch == '|')
                return finishTok(TokenType::OP_OR);

            tokenState_.erroneousType_ = "tilde";
            return finishTok(TokenType::TOK_INVALID, true);

        /* ~~~ CASE: OP_PP ~~~ */
        case OP_PP:
            if (ch == '+')
                return finishTok(TokenType::OP_UADD);
            return finishTok(TokenType::OP_ADD, true);

        /* ~~~ CASE: OP_ARROW ~~~ */
        case OP_ARROW:
            if (ch == '>')
                return finishTok(TokenType::OP_FN_ARROW);
            return finishTok(TokenType::OP_SUB, true);

        /* ~~~ CASE: OP_ARROW ~~~ */
        case CMT_SINGLE:
            if (ch == '\n') {
                tokenState_.state_ = START;
                ++tokenState_.lineNo;
            }
            return nullptr;

        /* ~~~ CASE: CMT_MAYBE_MULTI_CMT ~~~ */
        case CMT_MAYBE_MULTI_CMT:
            // It's indeed a multi-line comment
            if (ch == '*') {
                tokenState_.state_ = CMT_MULTI;
                return nullptr;
            }
            // It was just a division symbol
            else {
                tokenState_.state_ = START;
                return finishTok(TokenType::OP_DIV, true);
            }

        /* ~~~ CASE: CMT_MULTI ~~~ */
        case CMT_MULTI:
            // Maybe we've reach the multi-line comment closing
            if (ch == '*')
                tokenState_.state_ = CMT_MAYBE_FINISH_MULTI;

            // Newline
            else if (ch == '\n')
                ++tokenState_.lineNo;

            // Found EOF
            else if(ch == -1)
                throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": comment wasn't closed"};

            return nullptr;

        /* ~~~ CASE: CMT_MAYBE_FINISH_MULTI ~~~ */
        case CMT_MAYBE_FINISH_MULTI:
            // It was indeed a multi-line comment closing
            if (ch == '/')
                tokenState_.state_ = START;

            // Reached EOF
            else if (ch == -1)
                throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": comment wasn't closed"};

            // Any other symbol other than '/' must be skipped
            else if (ch != '*')
                tokenState_.state_ = CMT_MULTI;

            return nullptr;

        /* ~~~ CASE: ERRONEOUS ~~~ */
        default:
            throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": Unexpected state"};
    }
}

void Lexer::tokenize(const std::string &input) {
    // Opens the `input` file and copies its content into `data`
    std::ifstream sourceFile {input};
    tokenState_.data_ = {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};

    if (tokenState_.data_.empty())
        throw Exception{input + ":" + "Empty file"};

    // Add a newline at the end of the data if there's none already.
    // This comes in handy to save the last token from getting dismissed
    // when we reach the end of file and escape the condition `if(currState == MAIN)`
    if (tokenState_.data_.back() != '\n')
        tokenState_.data_ += '\n';

    tokenState_.it_ = tokenState_.data_.begin();    // Set an iterator to the begin of the data
    tokenState_.fileName_ = input;                  // Set file name for the file being tokenized at the moment
    std::shared_ptr<Token> result;                  // This is where we temporarily save the token result

    while (tokenState_.it_ != tokenState_.data_.end()) {
        try {
            result = tokNext(*tokenState_.it_);
        } catch (const Exception &error) {
            std::cerr << "Lexer error: " << error.what() << ": " << *tokenState_.it_ << "\n";
        }
        // Put only valid tokens into the tokens vector
        if (result != nullptr) {
            if (result->getType() == TokenType::TOK_INVALID)
                // Muh user-friendly error messages for the end-users
                std::cerr << "Lexer error: " << tokenState_.fileName_ << ":" << std::to_string(tokenState_.lineNo) << ": Invalid suffix for " << tokenState_.erroneousType_ << "\n";
            else
                tokens_.push_back(result);
        }
        ++tokenState_.it_;
    }

    // Add the EOF token to mark the file's ending
    auto pif   = std::make_unique<PositionInFile>(input, tokenState_.lineNo);
    auto token = std::make_shared<Token>(TokenType::TOK_EOF, "", std::move(pif));
    tokens_.push_back(token);
}

void Lexer::prettyPrint() {
    size_t index = 0;

    // Print place-holder
    std::cout << std::left << std::setw(5)  << "ID" << " | "
                           << std::setw(5)  << "LN" << " | "
                           << std::setw(15) << "TYPE" << " | " << "VALUE\n";
    std::cout << "------+-------+-----------------+-----------------\n";

    for(auto & token : tokens_){
        std::cout << std::left << std::setw(5)   << index << " | "
                               << std::setw(5)   << token->getFileInfo()->getLineNo() << " | "
                               << std::setw(15)  << token->getName() << " | "
                               << token->getValue() << "\n";
        ++index;
    }
}
