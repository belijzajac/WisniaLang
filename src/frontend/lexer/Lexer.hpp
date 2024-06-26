// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_LEXER_HPP
#define WISNIALANG_LEXER_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Wisnia {
namespace Basic {
class Token;
enum class TType;
}  // namespace Basic

class Lexer {
  using TokenPtr = std::shared_ptr<Basic::Token>;

  enum class State {
    START,             // Start state
    IDENT,             // Identifier
    OP_COMPARE,        // Either a single operand (!, <, >, =) or (!=, <=, >=, ==)
    STRING,            // String literal
    INTEGER,           // Integer literal
    FLOAT,             // Float literal
    ERRONEOUS_NUMBER,  // Invalid number
    LOGIC_AND,         // Logical AND: &&
    LOGIC_OR,          // Logical OR: ||
    OP_PP,             // Unary prefix: ++
    OP_MM,             // Unary prefix: --
    ESCAPE_SEQ,        // Escapes \t, \n, etc.
    CMT_SINGLE,        // Single line comment: //
    CMT_I,             // Escape multi-line comment (1)
    CMT_II,            // Escape multi-line comment (2)
    CMT_III,           // Escape multi-line comment (3)
  };

  struct TokenState {
    // Info needed to construct a token and to tokenize a letter
    State m_state{State::START};
    std::string m_buff;

    // Accessors to the actual data of the source file
    std::string m_data;
    std::string::iterator m_iterator;

    // Vague info about the source file
    std::string m_fileName;
    size_t m_lineNo{1};

    // String info
    size_t m_strStart{0};

    // Temp info
    std::string m_errType;
  };

  // Having provided the TType, it constructs and returns a token
  TokenPtr finishTok(const Basic::TType &type, bool goBack = false);

  // From an existing token buffer constructs and returns a token of identifier
  // (or keyword) type
  TokenPtr finishIdent();

  // Continues to tokenize the next letter
  std::optional<TokenPtr> tokNext(char ch);

  // Tokenizes whatever was passed to the tokenize function
  void tokenizeInput();

  // Preps up tokenization
  void tokenize(std::string_view filename);
  void tokenize(std::istringstream &stream);

 public:
  explicit Lexer(std::string_view filename);
  explicit Lexer(std::istringstream &stream);

  // Returns tokens
  const std::vector<TokenPtr> &getTokens() const { return m_tokens; }

  // Prints out tokens in a pretty table
  void print(std::ostream &output) const;

 private:
  std::vector<TokenPtr> m_tokens;
  TokenState m_tokenState;
};

}  // namespace Wisnia

#endif  // WISNIALANG_LEXER_HPP
