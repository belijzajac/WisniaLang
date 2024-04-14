// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_INSTRUCTION_HPP
#define WISNIALANG_INSTRUCTION_HPP

#include <memory>
#include <utility>
// Wisnia
#include "Operation.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

class Instruction {
  using TokenPtr = std::shared_ptr<Basic::Token>;

  static inline size_t sPrintTargetWidth{15};
  static inline size_t sPrintArgOneWidth{15};

 public:
  explicit Instruction(
    Operation op,
    TokenPtr target = nullptr,
    TokenPtr arg1   = nullptr,
    TokenPtr arg2   = nullptr
  ) : m_operation{op}, m_target{std::move(target)}, m_arg1{std::move(arg1)}, m_arg2{std::move(arg2)} {}

  const Operation &getOperation() const { return m_operation; }
  TokenPtr &getTarget() { return m_target; }
  TokenPtr &getArg1() { return m_arg1; }
  TokenPtr &getArg2() { return m_arg2; }

  static void setPrintTargetWidth(size_t width) { sPrintTargetWidth = width; }
  static void setPrintArgOneWidth(size_t width) { sPrintArgOneWidth = width; }
  void print(std::ostream &output) const;

 private:
  Operation m_operation;
  TokenPtr m_target, m_arg1, m_arg2;
};

}  // namespace Wisnia

#endif  // WISNIALANG_INSTRUCTION_HPP
