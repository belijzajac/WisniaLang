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

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
// Wisnia
#include "Lexer.hpp"
#include "Exceptions.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

constexpr std::array<char, 9> kSimpleOperands{'.', '*', '(', ')', '{', '}', ',', ':', ';'};

// Finish tokenizing a token
std::shared_ptr<Token> Lexer::finishTok(const TType &type, bool backtrack) {
  // We've over-gone by 1 character further by returning the token earlier, so step back
  if (backtrack) --m_tokenState.m_iterator;

  m_tokenState.m_state = State::START;
  size_t lineNo = (type == TType::LIT_STR) ? m_tokenState.m_strStart : m_tokenState.m_lineNo;
  auto pif = std::make_unique<Position>(m_tokenState.m_fileName, lineNo);

  // Convert token's buffer to appropriate type
  auto TokValue = [&]() -> TokenValue {
    switch (type) {
      // Integer
      case TType::LIT_INT:
        return std::stoll(m_tokenState.m_buff);
      // Float
      case TType::LIT_FLT:
        return std::stof(m_tokenState.m_buff);
      // Bool
      case TType::KW_TRUE:
      case TType::KW_FALSE:
        return m_tokenState.m_buff == "true";
      default:
        return m_tokenState.m_buff;
    }
  };

  auto token = std::make_shared<Token>(type, TokValue(), std::move(pif));
  m_tokenState.m_buff.clear();
  return token;
}

// Finish tokenize identifier
std::shared_ptr<Token> Lexer::finishIdent() {
  // It's either a known keyword
  if (auto search = Str2TokenKw.find(m_tokenState.m_buff); search != Str2TokenKw.end())
    return finishTok(search->second, true);
  // Or simply an identifier
  return finishTok(TType::IDENT, true);
}

// Tokenize the following character
std::optional<std::shared_ptr<Token>> Lexer::tokNext(const char ch) {
  switch (m_tokenState.m_state) {
    /* ~~~ CASE: START ~~~ */
    case State::START:
      if (isalpha(ch) || ch == '_') {
        m_tokenState.m_state = State::IDENT;
        m_tokenState.m_buff += ch;
      }
      else if (ch == '!' || ch == '<' || ch == '>' || ch == '=') {
        m_tokenState.m_state = State::OP_COMPARE;
        m_tokenState.m_buff += ch;
      }
      else if (ch == '\"') {
        m_tokenState.m_strStart = m_tokenState.m_lineNo;
        m_tokenState.m_state = State::STRING;
      }
      else if (isdigit(ch)) {
        m_tokenState.m_state = State::INTEGER;
        m_tokenState.m_buff += ch;
      }
      else if (ch == '&') {
        m_tokenState.m_state = State::LOGIC_AND;
        m_tokenState.m_buff += ch;
      }
      else if (ch == '|') {
        m_tokenState.m_state = State::LOGIC_OR;
        m_tokenState.m_buff += ch;
      }
      else if (ch == '+') {
        m_tokenState.m_state = State::OP_PP;
        m_tokenState.m_buff += ch;
      }
      else if (ch == '-') {
        m_tokenState.m_state = State::OP_MM;
        m_tokenState.m_buff += ch;
      }
      else if (ch == '/') {
        m_tokenState.m_state = State::CMT_I;
      }
      else if (ch == '#') {
        m_tokenState.m_state = State::CMT_SINGLE;
      }
      else if (std::any_of(kSimpleOperands.begin(), kSimpleOperands.end(), [&](char op) { return ch == op; })) {
        m_tokenState.m_buff = ch;
        return finishTok(Str2TokenOp[m_tokenState.m_buff]);
      }
      else if (isspace(ch)) {
        if (ch == '\n') ++m_tokenState.m_lineNo;
      }
      else {
        throw LexerError{
          m_tokenState.m_fileName + ":" + std::to_string(m_tokenState.m_lineNo) +
          ": Unrecognized character"
        };
      }
      return {};

    /* ~~~ CASE: IDENT ~~~ */
    case State::IDENT:
      if (isspace(ch)) return finishIdent();
      if (!isalpha(ch) && !isdigit(ch) && ch != '_') return finishIdent();
      m_tokenState.m_buff += ch;
      return {};

    /* ~~~ CASE: OP_COMPARE ~~~ */
    case State::OP_COMPARE:
      if (ch == '=') {
        // (!=, <=, >=, ==)
        m_tokenState.m_buff += '=';
        return finishTok(Str2TokenOp[m_tokenState.m_buff]);
      } else {
        // (!, <, >, =)
        return finishTok(Str2TokenOp[m_tokenState.m_buff], true);
      }

    /* ~~~ CASE: STRING ~~~ */
    case State::STRING:
      if (ch == '\\') {
        m_tokenState.m_state = State::ESCAPE_SEQ;
      }
      else {
        if (ch == '\n') {
          ++m_tokenState.m_lineNo;
        } else if (ch == '\"') {
          return finishTok(TType::LIT_STR);
        } else if (ch == -1) {
          throw LexerError{
            m_tokenState.m_fileName + ":" + std::to_string(m_tokenState.m_lineNo) +
            ": Unterminated string"
          };
        }
        m_tokenState.m_buff += ch;
      }
      return {};

    /* ~~~ CASE: ESCAPE_SEQ ~~~ */
    case State::ESCAPE_SEQ:
      // Escape sequences for strings
      switch (ch) {
        case 'f':
          m_tokenState.m_buff += '\f';
          break;
        case 'r':
          m_tokenState.m_buff += '\r';
          break;
        case 't':
          m_tokenState.m_buff += '\t';
          break;
        case 'v':
          m_tokenState.m_buff += '\v';
          break;
        case 'n':
          m_tokenState.m_buff += '\n';
          break;
        case '\"':
          m_tokenState.m_buff += '\"';
          break;
        default:
          m_tokenState.m_buff += "\\";
          m_tokenState.m_buff += ch;
          break;
      }
      // Continue parsing
      m_tokenState.m_state = State::STRING;
      return {};

    /* ~~~ CASE: INTEGER ~~~ */
    case State::INTEGER:
      if (isspace(ch)) {
        return finishTok(TType::LIT_INT);
      } else if (ch == '.') {
        m_tokenState.m_state = State::FLOAT;
      } else if (isalpha(ch)) {
        m_tokenState.m_state = State::ERRONEOUS_NUMBER;
        m_tokenState.m_errType = "integer";
      } else if (!isdigit(ch)) {
        return finishTok(TType::LIT_INT, true);
      }
      m_tokenState.m_buff += ch;
      return {};

    /* ~~~ CASE: FLOAT ~~~ */
    case State::FLOAT:
      if (isspace(ch)) {
        return finishTok(TType::LIT_FLT);
      } else if (isalpha(ch) || ch == '.') {
        m_tokenState.m_state = State::ERRONEOUS_NUMBER;
        m_tokenState.m_errType = "float";
      } else if (!isdigit(ch)) {
        return finishTok(TType::LIT_FLT, true);
      }
      m_tokenState.m_buff += ch;
      return {};

    /* ~~~ CASE: ERRONEOUS_NUMBER ~~~ */
    case State::ERRONEOUS_NUMBER:
      if (isspace(ch)) {
        if (ch == '\n') ++m_tokenState.m_lineNo;
        return finishTok(TType::TOK_INVALID);
      }
      m_tokenState.m_buff += ch;
      return {};

    /* ~~~ CASE: LOGIC_AND ~~~ */
    case State::LOGIC_AND:
      if (ch == '&') return finishTok(TType::OP_AND);
      m_tokenState.m_errType = "ampersand";
      return finishTok(TType::TOK_INVALID, true);

    /* ~~~ CASE: LOGIC_OR ~~~ */
    case State::LOGIC_OR:
      if (ch == '|') return finishTok(TType::OP_OR);
      m_tokenState.m_errType = "tilde";
      return finishTok(TType::TOK_INVALID, true);

    /* ~~~ CASE: OP_PP ~~~ */
    case State::OP_PP:
      if (ch == '+') return finishTok(TType::OP_UADD);
      return finishTok(TType::OP_ADD, true);

    /* ~~~ CASE: OP_MM ~~~ */
    case State::OP_MM:
      if (ch == '-') {
        m_tokenState.m_buff += ch;
        return finishTok(TType::OP_USUB);
      } else if (ch == '>') {
        m_tokenState.m_buff += ch;
        return finishTok(TType::OP_FN_ARROW);
      }
      return finishTok(TType::OP_SUB, true);

    /* ~~~ CASE: CMT_SINGLE ~~~ */
    case State::CMT_SINGLE:
      if (ch == '\n') {
        m_tokenState.m_state = State::START;
        ++m_tokenState.m_lineNo;
      }
      return {};

    /* ~~~ CASE: CMT_I ~~~ */
    case State::CMT_I:
      if (ch == '*') {
        // It's indeed a multi-line comment
        m_tokenState.m_state = State::CMT_II;
        return {};
      } else {
        // It was just a division symbol
        m_tokenState.m_state = State::START;
        return finishTok(TType::OP_DIV, true);
      }

    /* ~~~ CASE: CMT_II ~~~ */
    case State::CMT_II:
      if (ch == '*') {
        // Maybe we've reached the multi-line comment closing
        m_tokenState.m_state = State::CMT_III;
      } else if (ch == '\n') {
        // Newline
        ++m_tokenState.m_lineNo;
      } else if (ch == -1) {
        // Found EOF
        throw LexerError{
          m_tokenState.m_fileName + ":" + std::to_string(m_tokenState.m_lineNo) +
          ": comment wasn't closed"
        };
      }
      return {};

    /* ~~~ CASE: CMT_III ~~~ */
    case State::CMT_III:
      if (ch == '/') {
        // It was indeed a multi-line comment closing
        m_tokenState.m_state = State::START;
      } else if (ch == -1) {
        // Reached EOF
        throw LexerError{
          m_tokenState.m_fileName + ":" + std::to_string(m_tokenState.m_lineNo) +
          ": comment wasn't closed"
        };
      } else if (ch != '*') {
        // Any other symbol other than '/' must be skipped
        m_tokenState.m_state = State::CMT_II;
      }
      return {};

    /* ~~~ CASE: ERRONEOUS ~~~ */
    default:
      throw LexerError{
        m_tokenState.m_fileName + ":" + std::to_string(m_tokenState.m_lineNo) +
        ": Unexpected state"
      };
  }
}

void Lexer::tokenize(std::string_view filename) {
  // Opens the `input` file and copies its content into `data`
  std::ifstream sourceFile{filename.data()};
  m_tokenState.m_data = {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};
  m_tokenState.m_fileName = filename;
  tokenizeInput();
}

void Lexer::tokenize(std::istringstream &stream) {
  m_tokenState.m_data = {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
  m_tokenState.m_fileName = "string stream";
  tokenizeInput();
}

void Lexer::tokenizeInput() {
  assert(!m_tokenState.m_data.empty() && !m_tokenState.m_fileName.empty() &&
         "the provided input was either empty or Lexer::tokenize wasn't called");

  // Add a newline at the end of the data if there's none already.
  // This comes in handy to save the last token from getting dismissed
  // when we reach the end of file and escape the condition `if(currState == MAIN)`
  if (m_tokenState.m_data.back() != '\n') m_tokenState.m_data += '\n';

  // Set an iterator to the beginning of the data
  // And file name for the file being tokenized at the moment
  m_tokenState.m_iterator = m_tokenState.m_data.begin();
  while (m_tokenState.m_iterator != m_tokenState.m_data.end()) {
    if (auto result = tokNext(*m_tokenState.m_iterator); result.has_value()) {
      if (result.value()->getType() != TType::TOK_INVALID) {
        m_tokens.push_back(*result);
      } else {
        throw LexerError{
            m_tokenState.m_fileName + ":" + std::to_string(m_tokenState.m_lineNo) +
            ": Invalid suffix for " + m_tokenState.m_errType};
      }
    }
    ++m_tokenState.m_iterator;
  }

  // Add the EOF token to mark the file's ending
  auto pif = std::make_unique<Position>(m_tokenState.m_fileName, m_tokenState.m_lineNo);
  m_tokens.emplace_back(std::make_shared<Token>(TType::TOK_EOF, "[EOF]", std::move(pif)));
}

void Lexer::print(std::ostream &output) const {
  size_t index = 0;
  output << fmt::format("{:^6}|{:^6}|{:^17}|{:^17}\n", "ID", "LN", "TYPE", "VALUE");
  output << fmt::format("------+------+-----------------+-----------------\n");
  for (const auto &token : m_tokens) {
    output << fmt::format(
      "{:^6}|{:^6}|{:^17}|{:^17}\n",
      index,
      token->getPosition().getLineNo(),
      token->getName(),
      token->getASTValueStr()
    );
    ++index;
  }
}

Lexer::Lexer(std::string_view filename) { tokenize(filename); }
Lexer::Lexer(std::istringstream &sstream) { tokenize(sstream); }
