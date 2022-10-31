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

  static inline size_t sArgOneWidth{15};
  static inline size_t sTargetWidth{15};

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

  static void setTargetWidth(size_t width) { sTargetWidth = width; }
  static void setArgOneWidth(size_t width) { sArgOneWidth = width; }
  void print() const;

 private:
  Operation m_operation;
  TokenValue m_target;
  TokenValue m_arg1;
  TokenValue m_arg2;
};

}  // namespace Wisnia

#endif  // WISNIALANG_INSTRUCTION_HPP
