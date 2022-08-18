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

#ifndef WISNIALANG_EXCEPTION_HPP
#define WISNIALANG_EXCEPTION_HPP

#include <exception>
#include <string>

namespace Wisnia::Utils {

class WisniaError : public std::exception {
 public:
  explicit WisniaError(const std::string& msg) : m_msg{msg} {}
  const char* what() const noexcept override { return m_msg.c_str(); }

 private:
  std::string m_msg;
};

class TokenError : public WisniaError {
 public:
  explicit TokenError(const std::string& msg)
      : WisniaError("Token Error: " + msg)
  {}
};

class LexerError : public WisniaError {
 public:
  explicit LexerError(const std::string& msg)
      : WisniaError("Lexical Analysis Error: " + msg)
  {}
};

class ParserError : public WisniaError {
 public:
  explicit ParserError(const std::string& msg)
      : WisniaError("Syntax Analysis Error: " + msg)
  {}
};

class SemanticError : public WisniaError {
 public:
  explicit SemanticError(const std::string& msg)
      : WisniaError("Semantic Analysis Error: " + msg)
  {}
};

class InstructionError : public WisniaError {
 public:
  explicit InstructionError(const std::string& msg)
      : WisniaError("Instruction Error: " + msg)
  {}
};

class NotImplementedError : public WisniaError {
 public:
  explicit NotImplementedError(const std::string& msg)
      : WisniaError("Not Implemented Error: " + msg)
  {}
};

}  // namespace Wisnia::Utils

#endif  // WISNIALANG_EXCEPTION_HPP
