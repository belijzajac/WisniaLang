#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <fmt/format.h>
// Wisnia
#include "Lexer.h"
#include "Exceptions.h"
#include "Token.h"

using namespace Wisnia;
using namespace Basic;
using namespace Utils;

// Operands of length 1
constexpr std::array<char, 9> simpleOps{'.', '*', '(', ')', '{', '}', ',', ':', ';'};

// Finish tokenizing a token
std::shared_ptr<Token> Lexer::finishTok(const TType &type_, bool backtrack) {
  // We've over-gone by 1 character further by returning the token earlier, so step back
  if (backtrack) --tokenState_.it_;

  tokenState_.state_ = State::START;
  size_t lineNo = (type_ == TType::LIT_STR) ? tokenState_.stringStart : tokenState_.lineNo;
  auto pif = std::make_unique<PositionInFile>(tokenState_.fileName_, lineNo);

  // Convert token's buffer to appropriate type
  const auto TokValue = [&]() -> TokenValue {
    switch (type_) {
      // Integer
      case TType::LIT_INT:
        return std::stoi(tokenState_.buff_);
      // Float
      case TType::LIT_FLT:
        return std::stof(tokenState_.buff_);
      // Bool
      case TType::KW_TRUE:
      case TType::KW_FALSE:
        return tokenState_.buff_ == "true";
      default:
        return tokenState_.buff_;
    }
  };

  auto token = std::make_shared<Token>(type_, TokValue(), std::move(pif));
  tokenState_.buff_.clear();
  return token;
}

// Finish tokenize identifier
std::shared_ptr<Token> Lexer::finishIdent() {
  // It's either a known keyword
  if (auto search = Str2TokenKw.find(tokenState_.buff_); search != Str2TokenKw.end())
    return finishTok(search->second, true);
  // Or simply an identifier
  return finishTok(TType::IDENT, true);
}

// Tokenize the following character
// TODO: std::optional ?
// TODO: a vector of possible states of type std::function<void(bool)>
// tokMyState ???
std::shared_ptr<Token> Lexer::tokNext(char ch) {
  switch (tokenState_.state_) {
    /* ~~~ CASE: START ~~~ */
    case State::START:
      if (isalpha(ch) || ch == '_') {
        tokenState_.state_ = State::IDENT;
        tokenState_.buff_ += ch;
      }
      else if (ch == '!' || ch == '<' || ch == '>' || ch == '=') {
        tokenState_.state_ = State::OP_COMPARE;
        tokenState_.buff_ += ch;
      }
      else if (ch == '\"') {
        tokenState_.stringStart = tokenState_.lineNo;
        tokenState_.state_ = State::STRING;
      }
      else if (isdigit(ch)) {
        tokenState_.state_ = State::INTEGER;
        tokenState_.buff_ += ch;
      }
      else if (ch == '&') {
        tokenState_.state_ = State::LOGIC_AND;
        tokenState_.buff_ += ch;
      }
      else if (ch == '|') {
        tokenState_.state_ = State::LOGIC_OR;
        tokenState_.buff_ += ch;
      }
      else if (ch == '+') {
        tokenState_.state_ = State::OP_PP;
        tokenState_.buff_ += ch;
      }
      else if (ch == '-') {
        tokenState_.state_ = State::OP_MM;
        tokenState_.buff_ += ch;
      }
      else if (ch == '/') {
        tokenState_.state_ = State::CMT_I;
        return nullptr;
      }
      else if (ch == '#') {
        tokenState_.state_ = State::CMT_SINGLE;
      }
      else if (std::any_of(simpleOps.begin(), simpleOps.end(), [&](char op) { return ch == op; })) {
        tokenState_.buff_ = ch;
        return finishTok(Str2TokenOp[tokenState_.buff_]);
      }
      else if (isspace(ch)) {
        if (ch == '\n') ++tokenState_.lineNo;
        return nullptr;
      }
      else {
        throw LexerError{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) +
                         ": Unrecognized char"};
      }
      return nullptr;

    /* ~~~ CASE: IDENT ~~~ */
    case State::IDENT:
      if (isspace(ch)) return finishIdent();
      if (!isalpha(ch) && !isdigit(ch) && ch != '_') return finishIdent();
      tokenState_.buff_ += ch;
      return nullptr;

    /* ~~~ CASE: OP_COMPARE ~~~ */
    case State::OP_COMPARE:
      if (ch == '=') {
        // (!=, <=, >=, ==)
        tokenState_.buff_ += '=';
        return finishTok(Str2TokenOp[tokenState_.buff_]);
      } else {
        // (!, <, >, =)
        return finishTok(Str2TokenOp[tokenState_.buff_], true);
      }

    /* ~~~ CASE: STRING ~~~ */
    case State::STRING:
      if (ch == '\\') {
        tokenState_.state_ = State::ESCAPE_SEQ;
      }
      else {
        if (ch == '\n') {
          ++tokenState_.lineNo;
        } else if (ch == '\"') {
          return finishTok(TType::LIT_STR);
        } else if (ch == -1) {
          throw LexerError{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) +
                           ": Unterminated string"};
        }
        tokenState_.buff_ += ch;
      }
      return nullptr;

    /* ~~~ CASE: ESCAPE_SEQ ~~~ */
    case State::ESCAPE_SEQ:
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
          throw LexerError{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) +
                           ": Unknown escape symbol"};
      }
      // Continue parsing
      tokenState_.state_ = State::STRING;
      return nullptr;

    /* ~~~ CASE: INTEGER ~~~ */
    case State::INTEGER:
      if (isspace(ch)) {
        return finishTok(TType::LIT_INT);
      } else if (ch == '.') {
        tokenState_.state_ = State::FLOAT;
      } else if (isalpha(ch)) {
        tokenState_.state_ = State::ERRONEOUS_NUMBER;
        tokenState_.erroneousType_ = "integer";
      } else if (!isdigit(ch)) {
        return finishTok(TType::LIT_INT, true);
      }
      tokenState_.buff_ += ch;
      return nullptr;

    /* ~~~ CASE: FLOAT ~~~ */
    case State::FLOAT:
      if (isspace(ch)) {
        return finishTok(TType::LIT_FLT);
      } else if (isalpha(ch) || ch == '.') {
        tokenState_.state_ = State::ERRONEOUS_NUMBER;
        tokenState_.erroneousType_ = "float";
      } else if (!isdigit(ch)) {
        return finishTok(TType::LIT_FLT, true);
      }
      tokenState_.buff_ += ch;
      return nullptr;

    /* ~~~ CASE: ERRONEOUS_NUMBER ~~~ */
    case State::ERRONEOUS_NUMBER:
      if (isspace(ch)) {
        if (ch == '\n') ++tokenState_.lineNo;
        return finishTok(TType::TOK_INVALID);
      }
      tokenState_.buff_ += ch;
      return nullptr;

    /* ~~~ CASE: LOGIC_AND ~~~ */
    case State::LOGIC_AND:
      if (ch == '&') return finishTok(TType::OP_AND);
      tokenState_.erroneousType_ = "ampersand";
      return finishTok(TType::TOK_INVALID, true);

    /* ~~~ CASE: LOGIC_OR ~~~ */
    case State::LOGIC_OR:
      if (ch == '|') return finishTok(TType::OP_OR);
      tokenState_.erroneousType_ = "tilde";
      return finishTok(TType::TOK_INVALID, true);

    /* ~~~ CASE: OP_PP ~~~ */
    case State::OP_PP:
      if (ch == '+') return finishTok(TType::OP_UADD);
      return finishTok(TType::OP_ADD, true);

    /* ~~~ CASE: OP_MM ~~~ */
    case State::OP_MM:
      if (ch == '-') {
        tokenState_.buff_ += ch;
        return finishTok(TType::OP_USUB);
      } else if (ch == '>') {
        tokenState_.buff_ += ch;
        return finishTok(TType::OP_FN_ARROW);
      }
      return finishTok(TType::OP_SUB, true);

    /* ~~~ CASE: CMT_SINGLE ~~~ */
    case State::CMT_SINGLE:
      if (ch == '\n') {
        tokenState_.state_ = State::START;
        ++tokenState_.lineNo;
      }
      return nullptr;

    /* ~~~ CASE: CMT_I ~~~ */
    case State::CMT_I:
      if (ch == '*') {
        // It's indeed a multi-line comment
        tokenState_.state_ = State::CMT_II;
        return nullptr;
      } else {
        // It was just a division symbol
        tokenState_.state_ = State::START;
        return finishTok(TType::OP_DIV, true);
      }

    /* ~~~ CASE: CMT_II ~~~ */
    case State::CMT_II:
      if (ch == '*') {
        // Maybe we've reached the multi-line comment closing
        tokenState_.state_ = State::CMT_III;
      } else if (ch == '\n') {
        // Newline
        ++tokenState_.lineNo;
      } else if (ch == -1) {
        // Found EOF
        throw LexerError{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) +
                         ": comment wasn't closed"};
      }
      return nullptr;

    /* ~~~ CASE: CMT_III ~~~ */
    case State::CMT_III:
      if (ch == '/') {
        // It was indeed a multi-line comment closing
        tokenState_.state_ = State::START;
      } else if (ch == -1) {
        // Reached EOF
        throw LexerError{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) +
                         ": comment wasn't closed"};
      } else if (ch != '*') {
        // Any other symbol other than '/' must be skipped
        tokenState_.state_ = State::CMT_II;
      }
      return nullptr;

    /* ~~~ CASE: ERRONEOUS ~~~ */
    default:
      throw LexerError{tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) +
                       ": Unexpected state"};
  }
}

void Lexer::tokenize(const std::string &filename) {
  // Opens the `input` file and copies its content into `data`
  std::ifstream sourceFile{filename};
  tokenState_.data_ = {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};
  tokenState_.fileName_ = filename;
  tokenizeInput();
}

void Lexer::tokenize(std::istringstream &sstream) {
  tokenState_.data_ = {std::istreambuf_iterator<char>(sstream), std::istreambuf_iterator<char>()};
  tokenState_.fileName_ = "string stream";
  tokenizeInput();
}

void Lexer::tokenizeInput()
{
  assert(!tokenState_.data_.empty() && !tokenState_.fileName_.empty() &&
         "the provided input was either empty or Lexer::tokenize wasn't called");

  // Add a newline at the end of the data if there's none already.
  // This comes in handy to save the last token from getting dismissed
  // when we reach the end of file and escape the condition `if(currState == MAIN)`
  if (tokenState_.data_.back() != '\n')
    tokenState_.data_ += '\n';

  // Set an iterator to the begin of the data
  // And file name for the file being tokenized at the moment
  tokenState_.it_ = tokenState_.data_.begin();
  while (tokenState_.it_ != tokenState_.data_.end()) {
    auto result = tokNext(*tokenState_.it_);
    // Put only valid tokens into the tokens vector
    if (result != nullptr) {
      if (result->getType() != TType::TOK_INVALID) {
        tokens_.push_back(result);
      } else {
        throw LexerError(tokenState_.fileName_ + ":" + std::to_string(tokenState_.lineNo) +
                         ": Invalid suffix for " + tokenState_.erroneousType_);
      }
    }
    ++tokenState_.it_;
  }

  // Add the EOF token to mark the file's ending
  auto pif = std::make_unique<PositionInFile>(tokenState_.fileName_, tokenState_.lineNo);
  auto token = std::make_shared<Token>(TType::TOK_EOF, "[EOF]", std::move(pif));
  tokens_.push_back(token);
}

void Lexer::prettyPrint() {
  size_t index = 0;
  fmt::print("{:^6}|{:^6}|{:^17}|{:^17}\n", "ID", "LN", "TYPE", "VALUE");
  fmt::print("------+------+-----------------+-----------------\n");
  for (const auto &token : tokens_) {
    fmt::print("{:^6}|{:^6}|{:^17}|{:^17}\n",
               index, token->getFileInfo()->getLineNo(),
               token->getName(), token->getValueStr());
    ++index;
  }
}

Lexer::Lexer(const std::string &filename) { tokenize(filename); }
Lexer::Lexer(std::istringstream &sstream) { tokenize(sstream); }
