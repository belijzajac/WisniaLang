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
    CMT_SINGLE,        // Single line comment: #
    CMT_I,             // Escape multi-line comment (1)
    CMT_II,            // Escape multi-line comment (2)
    CMT_III,           // Escape multi-line comment (3)
  };

  struct TokenState {
    // Info needed to construct a token and to tokenize a letter
    State m_state{State::START};
    std::string m_buff{};

    // Accessors to the actual data of the source file
    std::string m_data{};
    std::string::iterator m_iterator;

    // Vague info about the source file
    std::string m_fileName{};
    size_t m_lineNo{1};

    // String info
    size_t m_strStart{0};

    // Temp info
    std::string m_errType{};
  };

  // Having provided the TType, it constructs and returns a token
  std::shared_ptr<Basic::Token> finishTok(const Basic::TType &type, bool backtrack = false);

  // From an existing token buffer constructs and returns a token of identifier
  // (or keyword) type
  std::shared_ptr<Basic::Token> finishIdent();

  // Continues to tokenize the next letter
  std::optional<std::shared_ptr<Basic::Token>> tokNext(char ch);

  // Tokenizes whatever was passed to the tokenize function
  void tokenizeInput();

  // Preps up tokenization
  void tokenize(const std::string &filename);
  void tokenize(std::istringstream &sstream);

 public:
  explicit Lexer(const std::string &filename);
  explicit Lexer(std::istringstream &sstream);

  // Returns tokens
  const std::vector<std::shared_ptr<Basic::Token>> &getTokens() const { return m_tokens; }

  // Prints out tokens in a pretty table
  void prettyPrint() const;

 private:
  std::vector<std::shared_ptr<Basic::Token>> m_tokens;
  TokenState m_tokenState;
};

}  // namespace Wisnia

#endif  // WISNIALANG_LEXER_HPP
