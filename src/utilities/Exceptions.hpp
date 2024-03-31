// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_EXCEPTION_HPP
#define WISNIALANG_EXCEPTION_HPP

#include <exception>
#include <string>
#include <utility>

namespace Wisnia {

class WisniaError : public std::exception {
 public:
  explicit WisniaError(std::string msg) : m_msg{std::move(msg)} {}
  const char *what() const noexcept override { return m_msg.c_str(); }
 private:
  std::string m_msg;
};

class TokenError : public WisniaError {
 public:
  explicit TokenError(const std::string &msg)
      : WisniaError("Token Error: " + msg) {}
};

class LexerError : public WisniaError {
 public:
  explicit LexerError(const std::string &msg)
      : WisniaError("Lexical Analysis Error: " + msg) {}
};

class ParserError : public WisniaError {
 public:
  explicit ParserError(const std::string &msg)
      : WisniaError("Syntax Analysis Error: " + msg) {}
};

class SemanticError : public WisniaError {
 public:
  explicit SemanticError(const std::string &msg)
      : WisniaError("Semantic Analysis Error: " + msg) {}
};

class InstructionError : public WisniaError {
 public:
  explicit InstructionError(const std::string &msg)
      : WisniaError("Instruction Error: " + msg) {}
};

class CodeGenerationError : public WisniaError {
 public:
  explicit CodeGenerationError(const std::string &msg)
      : WisniaError("Code Generation Error: " + msg) {}
};

class NotImplementedError : public WisniaError {
 public:
  explicit NotImplementedError(const std::string &msg)
      : WisniaError("Not Implemented Error: " + msg) {}
};

}  // namespace Wisnia

#endif  // WISNIALANG_EXCEPTION_HPP
