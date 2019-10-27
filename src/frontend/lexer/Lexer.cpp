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
static inline std::vector<char> simpleOperands {'.', '*', '/', '(', ')', '{', '}', ',', ':', ';'};

// Searches for string `val` in the provided map, which represents a string-to-TokenType relationship
auto findInMap = [](const std::unordered_map<std::string, TokenType> &map, const std::string &val) -> bool {
    auto found {map.find(val)};
    return found != map.end();
};

// Skips single-line comments
bool remSingleLnCmts(std::string::iterator &it, const std::string &data, int &lineNo) {
    // It's a single-line comment opener
    if(*it == '#') {
        // Skip everything until the end of the line
        while(it != data.end() && *it != '\n')
            ++it;

        return true; // it was a comment
    }
    return false; // it wasn't a comment
}

// Skips multiple-line comments
bool remMultipleLnCmts(std::string::iterator &it, const std::string &data, int &lineNo, const std::string &file) {
    bool reachedNewLn = false;

    // If it's a comment opener ("/*")
    if(*it == '/' && it+1 != data.end() && *(it+1) == '*') {
        // Skip characters as long as we find an occurrence of "*/"
        while(*it != '*' || it+1 == data.end() || *(it+1) != '/') {
            ++it;

            // Check if we've reached the following line
            if (*it == '\n') {
                ++lineNo;
                reachedNewLn = true;
            }

            // We've reached the end of the file and didn't find the "*/"
            if (it == data.end()) {
                throw Exception{file + ":" + std::to_string(lineNo) + ": comment wasn't closed"};
            }
        }
        // Take care of the closing comment
        it += 2;
    }
    // Instead of "/*" we found "*/", which shouldn't have happened
    else if(*it == '*' && it+1 != data.end() && *(it+1) == '/') {
        throw Exception{file + ":" + std::to_string(lineNo) + ": comment wasn't opened"};
    }
    return reachedNewLn;
}

std::shared_ptr<Token> Lexer::finish_lexem(const TokenType &type_, bool backtrack) {
    tokenState_.state_ = START;

    // We've over-gone by 1 character further by returning the token earlier
    if (backtrack)
        --tokenState_.it_;

    auto pif = std::make_unique<PositionInFile>(tokenState_.fileName_, tokenState_.lineNo);
    auto token = std::make_shared<Token>(type_, tokenState_.buff_, std::move(pif));

    tokenState_.buff_.clear();
    return token;
}

std::shared_ptr<Token> Lexer::finish_ident() {
    // It's either a known keyword
    if (auto search = StrToTokenKw.find(tokenState_.buff_); search != StrToTokenKw.end())
        return finish_lexem(search->second, true);
    // Or simply an identifier
    else
        return finish_lexem(TokenType::IDENT, true);
}

// Tokenize the following character
std::shared_ptr<Token> Lexer::lex_next(char ch) {
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

            else if (ch == '\"')
                tokenState_.state_ = STRING;

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

            // Finish the below tokens instantly
            else if (std::any_of(simpleOperands.begin(), simpleOperands.end(), [&](char op){ return ch == op; })) {
                std::string opToStr {ch};
                return finish_lexem(StrToTokenOp[opToStr]);
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
                return finish_ident();

            if (!isalpha(ch) && !isdigit(ch) && ch != '_')
                return finish_ident();

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: OP_COMPARE ~~~ */
        case OP_COMPARE:
            if (ch == '=') {
                std::string tokKey = tokenState_.buff_ + '=';
                tokenState_.buff_.clear();
                return finish_lexem(StrToTokenOp[tokKey]); // (!=, <=, >=, ==)
            } else {
                std::string tokKey = tokenState_.buff_;
                tokenState_.buff_.clear();
                return finish_lexem(StrToTokenOp[tokKey], true); // (!, <, >, =)
            }

        /* ~~~ CASE: STRING ~~~ */
        case STRING: {
            const auto strLen = tokenState_.buff_.length();

            // We got an escape symbol -- need to escape it
            if (strLen >= 1 && tokenState_.buff_[strLen - 1] == '\\') {
                switch (ch) {
                    case 'f':
                        tokenState_.buff_[strLen - 1] = '\f';
                        break;
                    case 'n':
                        tokenState_.buff_[strLen - 1] = '\n';
                        break;
                    case 'r':
                        tokenState_.buff_[strLen - 1] = '\r';
                        break;
                    case 't':
                        tokenState_.buff_[strLen - 1] = '\t';
                        break;
                    case 'v':
                        tokenState_.buff_[strLen - 1] = '\v';
                        break;
                    case '\"':
                        tokenState_.buff_[strLen - 1] = '\"';
                        break;
                    default:
                        tokenState_.buff_ += ch;
                        throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": Unknown escape symbol"};
                }
            }
            // It's some other symbol
            else {
                if (ch == '\"')
                    return finish_lexem(TokenType::LIT_STR);

                if (ch == '\n')
                    ++tokenState_.lineNo;

                else if (ch == -1)
                    throw Exception{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) + ": Unterminated string"};

                tokenState_.buff_ += ch;
            }
            return nullptr;
        }

        /* ~~~ CASE: INTEGER ~~~ */
        case INTEGER:
            if (isspace(ch))
                return finish_lexem(TokenType::LIT_INT);

            else if (ch == '.')
                tokenState_.state_ = FLOAT;

            else if (isalpha(ch))
                tokenState_.state_ = ERRONEOUS_NUMBER;

            else if(!isdigit(ch))
                return finish_lexem(TokenType::LIT_INT, true);

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: FLOAT ~~~ */
        case FLOAT:
            if (isspace(ch))
                return finish_lexem(TokenType::LIT_FLT);

            else if (isalpha(ch) || ch == '.')
                tokenState_.state_ = ERRONEOUS_NUMBER;

            else if (!isdigit(ch))
                return finish_lexem(TokenType::LIT_FLT, true);

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: ERRONEOUS_NUMBER ~~~ */
        case ERRONEOUS_NUMBER:
            if (isspace(ch))
                return finish_lexem(TokenType::TOK_INVALID);

            tokenState_.buff_ += ch;
            return nullptr;

        /* ~~~ CASE: LOGIC_AND ~~~ */
        case LOGIC_AND:
            if (ch == '&')
                return finish_lexem(TokenType::OP_AND);
            return finish_lexem(TokenType::TOK_INVALID, true);

        /* ~~~ CASE: LOGIC_OR ~~~ */
        case LOGIC_OR:
            if (ch == '|')
                return finish_lexem(TokenType::OP_OR);
            return finish_lexem(TokenType::TOK_INVALID, true);

        /* ~~~ CASE: OP_PP ~~~ */
        case OP_PP:
            if (ch == '+')
                return finish_lexem(TokenType::OP_UADD);
            return finish_lexem(TokenType::OP_ADD, true);

        /* ~~~ CASE: OP_ARROW ~~~ */
        case OP_ARROW:
            if (ch == '>')
                return finish_lexem(TokenType::OP_FN_ARROW);
            return finish_lexem(TokenType::OP_SUB, true);

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

    tokenState_.it_ = tokenState_.data_.begin();
    tokenState_.fileName_ = input;

    std::shared_ptr<Token> result;

    while (tokenState_.it_ != tokenState_.data_.end()) {
        // Treat single-line comments
        remSingleLnCmts(tokenState_.it_, tokenState_.data_, tokenState_.lineNo);

        // Nothing left to read, so break the while-loop
        if (tokenState_.it_ == tokenState_.data_.end())
            break;

        // Treat multiple-line comments
        remMultipleLnCmts(tokenState_.it_, tokenState_.data_, tokenState_.lineNo, input);

        // Nothing left to read, so break the while-loop
        if(tokenState_.it_ == tokenState_.data_.end())
            break;

        try {
            result = lex_next(*tokenState_.it_);
        } catch (const Exception &error) {
            std::cerr << "Lexer error: " << error.what() << ": " << *tokenState_.it_ << "\n";
        }
        // Put only valid tokens into the tokens vector
        if (result != nullptr) {
            if (result->getType() == TokenType::TOK_INVALID)
               std::cerr << "Lexer error: " << tokenState_.fileName_ << ":" << std::to_string(tokenState_.lineNo) << ": Invalid Token" << "\n";
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
