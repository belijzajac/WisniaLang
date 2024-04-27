// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <fmt/format.h>
// Wisnia
#include "Exceptions.hpp"
#include "Lexer.hpp"
#include "SemanticAnalysis.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

constexpr std::array kSimpleOperands{'.', '*', '(', ')', '{', '}', ',', ':', ';'};

Lexer::TokenPtr Lexer::finishTok(const TType &type, const bool goBack) {
  // We've over-gone by 1 character further by returning the token earlier, so step back
  if (goBack) {
    --m_tokenState.m_iterator;
  }

  m_tokenState.m_state = State::START;
  const size_t lineNo = (type == TType::LIT_STR) ? m_tokenState.m_strStart : m_tokenState.m_lineNo;
  auto pif = std::make_unique<Position>(m_tokenState.m_fileName, lineNo);

  auto TokValue = [&]() -> TokenValue {
    switch (type) {
      // Integer
      case TType::LIT_INT:
        int value;
        try {
          value = std::stoi(m_tokenState.m_buff);
        } catch (const std::out_of_range &) {
          throw TokenError{fmt::format("Value '{}' is out of supported range ('{}') in {}:{}",
                                       m_tokenState.m_buff,
                                       kMaxIntValue,
                                       pif->getFileName(),
                                       pif->getLineNo())};
        }
        return value;
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

Lexer::TokenPtr Lexer::finishIdent() {
  if (const auto it = Str2TokenKw.find(m_tokenState.m_buff); it != Str2TokenKw.end()) {
    return finishTok(it->second, true);
  }
  return finishTok(TType::IDENT, true);
}

std::optional<Lexer::TokenPtr> Lexer::tokNext(const char ch) {
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
      else if (std::any_of(kSimpleOperands.begin(), kSimpleOperands.end(), [&](const char op) { return ch == op; })) {
        m_tokenState.m_buff = ch;
        return finishTok(Str2TokenOp[m_tokenState.m_buff]);
      }
      else if (isspace(ch)) {
        if (ch == '\n') {
          ++m_tokenState.m_lineNo;
        }
      }
      else {
        throw LexerError{fmt::format("Unrecognized character '{}' in {}:{}",
                                     ch,
                                     m_tokenState.m_fileName,
                                     m_tokenState.m_lineNo)};
      }
      return {};

    /* ~~~ CASE: IDENT ~~~ */
    case State::IDENT:
      if (isspace(ch)) {
        return finishIdent();
      }
      if (!isalpha(ch) && !isdigit(ch) && ch != '_') {
        return finishIdent();
      }
      m_tokenState.m_buff += ch;
      return {};

    /* ~~~ CASE: OP_COMPARE ~~~ */
    case State::OP_COMPARE:
      if (ch == '=') {
        // (!=, <=, >=, ==)
        m_tokenState.m_buff += '=';
        return finishTok(Str2TokenOp[m_tokenState.m_buff]);
      }
      // (!, <, >, =)
      return finishTok(Str2TokenOp[m_tokenState.m_buff], true);

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
          throw LexerError{fmt::format("String was not terminated in {}:{}",
                                       m_tokenState.m_fileName,
                                       m_tokenState.m_lineNo)};
        }
        m_tokenState.m_buff += ch;
      }
      return {};

    /* ~~~ CASE: ESCAPE_SEQ ~~~ */
    case State::ESCAPE_SEQ:
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
      }
      if (ch == '.') {
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
      }
      if (isalpha(ch) || ch == '.') {
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
        if (ch == '\n') {
          ++m_tokenState.m_lineNo;
        }
        return finishTok(TType::TOK_INVALID);
      }
      m_tokenState.m_buff += ch;
      return {};

    /* ~~~ CASE: LOGIC_AND ~~~ */
    case State::LOGIC_AND:
      if (ch == '&') {
        return finishTok(TType::OP_AND);
      }
      m_tokenState.m_errType = "ampersand";
      return finishTok(TType::TOK_INVALID, true);

    /* ~~~ CASE: LOGIC_OR ~~~ */
    case State::LOGIC_OR:
      if (ch == '|') {
        return finishTok(TType::OP_OR);
      }
      m_tokenState.m_errType = "tilde";
      return finishTok(TType::TOK_INVALID, true);

    /* ~~~ CASE: OP_PP ~~~ */
    case State::OP_PP:
      if (ch == '+') {
        return finishTok(TType::OP_UADD);
      }
      return finishTok(TType::OP_ADD, true);

    /* ~~~ CASE: OP_MM ~~~ */
    case State::OP_MM:
      if (ch == '-') {
        m_tokenState.m_buff += ch;
        return finishTok(TType::OP_USUB);
      }
      if (ch == '>') {
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
        m_tokenState.m_state = State::CMT_II;
        return {};
      }
      if (ch == '/') {
        m_tokenState.m_state = State::CMT_SINGLE;
        return {};
      }
      // It was just a division symbol
      m_tokenState.m_state = State::START;
      return finishTok(TType::OP_DIV, true);

    /* ~~~ CASE: CMT_II ~~~ */
    case State::CMT_II:
      if (ch == '*') {
        m_tokenState.m_state = State::CMT_III;
      } else if (ch == '\n') {
        ++m_tokenState.m_lineNo;
      } else if (ch == -1) {
        // Reached EOF
        throw LexerError{fmt::format("Multi-line comment was not closed in {}:{}",
                                     m_tokenState.m_fileName,
                                     m_tokenState.m_lineNo)};
      }
      return {};

    /* ~~~ CASE: CMT_III ~~~ */
    case State::CMT_III:
      if (ch == '/') {
        m_tokenState.m_state = State::START;
      } else if (ch == -1) {
        // Reached EOF
        throw LexerError{fmt::format("Multi-line comment was not closed in {}:{}",
                                     m_tokenState.m_fileName,
                                     m_tokenState.m_lineNo)};
      } else if (ch != '*') {
        m_tokenState.m_state = State::CMT_II;
      }
      return {};

    /* ~~~ CASE: ERRONEOUS ~~~ */
    default:
      throw LexerError{fmt::format("Reached an unexpected state in {}:{}",
                                   m_tokenState.m_fileName,
                                   m_tokenState.m_lineNo)};
  }
}

void Lexer::tokenize(std::string_view filename) {
  std::ifstream sourceFile{filename.data()};
  m_tokenState.m_data = {std::istreambuf_iterator(sourceFile), std::istreambuf_iterator<char>()};
  m_tokenState.m_fileName = filename;
  tokenizeInput();
}

void Lexer::tokenize(std::istringstream &stream) {
  m_tokenState.m_data = {std::istreambuf_iterator(stream), std::istreambuf_iterator<char>()};
  m_tokenState.m_fileName = "in-memory";
  tokenizeInput();
}

void Lexer::tokenizeInput() {
  assert(!m_tokenState.m_data.empty() && !m_tokenState.m_fileName.empty() &&
         "the provided input was either empty or Lexer::tokenize wasn't called");

  // Add a newline at the end of the data if there's none already.
  // This comes in handy to save the last token from getting dismissed
  // when we reach the end of file and escape the condition `if(currState == MAIN)`
  if (m_tokenState.m_data.back() != '\n') {
    m_tokenState.m_data += '\n';
  }

  // Set an iterator to the beginning of the data
  // And file name for the file being tokenized at the moment
  m_tokenState.m_iterator = m_tokenState.m_data.begin();
  while (m_tokenState.m_iterator != m_tokenState.m_data.end()) {
    if (auto result = tokNext(*m_tokenState.m_iterator); result.has_value()) {
      if (result.value()->getType() != TType::TOK_INVALID) {
        m_tokens.push_back(*result);
      } else {
        throw LexerError{fmt::format("Invalid suffix for {} in {}:{}",
                                     m_tokenState.m_errType,
                                     m_tokenState.m_fileName,
                                     m_tokenState.m_lineNo)};
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

Lexer::Lexer(const std::string_view filename) { tokenize(filename); }
Lexer::Lexer(std::istringstream &stream) { tokenize(stream); }
