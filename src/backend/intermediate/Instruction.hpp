// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_INSTRUCTION_HPP
#define WISNIALANG_INSTRUCTION_HPP

#include <memory>
// Wisnia
#include "Operation.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

class Instruction {
  using TokenValue = std::shared_ptr<Basic::Token>;

  static inline size_t sPrintTargetWidth{15};
  static inline size_t sPrintArgOneWidth{15};

 public:
  explicit Instruction(
    Operation op,
    const TokenValue &target = nullptr,
    const TokenValue &arg1   = nullptr,
    const TokenValue &arg2   = nullptr
  ) : m_operation{op}, m_target{target}, m_arg1{arg1}, m_arg2{arg2} {}

  const Operation &getOperation() const { return m_operation; }
  TokenValue &getTarget() { return m_target; }
  TokenValue &getArg1() { return m_arg1; }
  TokenValue &getArg2() { return m_arg2; }

  static void setPrintTargetWidth(size_t width) { sPrintTargetWidth = width; }
  static void setPrintArgOneWidth(size_t width) { sPrintArgOneWidth = width; }
  void print(std::ostream &output) const;

 private:
  Operation m_operation;
  TokenValue m_target;
  TokenValue m_arg1;
  TokenValue m_arg2;
};

}  // namespace Wisnia

#endif  // WISNIALANG_INSTRUCTION_HPP
