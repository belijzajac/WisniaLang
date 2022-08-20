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
  using token_ptr = std::shared_ptr<Basic::Token>;

 public:
  Instruction(
    Operation op,
    const token_ptr &target,
    const token_ptr &arg1,
    const token_ptr &arg2 = nullptr
  ) : m_operation{op}, m_target{target}, m_arg1{arg1}, m_arg2{arg2} {}

  Operation getOperation() const { return m_operation; }
  const token_ptr &getTarget() const { return m_target; }
  const token_ptr &getArg1() const { return m_arg1; }
  const token_ptr &getArg2() const { return m_arg2; }

  void print() const;

 private:
  Operation m_operation;
  token_ptr m_target;
  token_ptr m_arg1;
  token_ptr m_arg2;
};

}  // namespace Wisnia

#endif  // WISNIALANG_INSTRUCTION_HPP
