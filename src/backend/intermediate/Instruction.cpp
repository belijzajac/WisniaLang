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
// Wisnia
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

void Wisnia::Instruction::print() const {
  fmt::print("{:^{}} %% {:<15}|{:^14}|{:^{}} %% {:<15}|{:^15} %% {:<15}\n",
    // target
    m_target ? m_target->getASTValueStr() : "", sTargetWidth + 2,
    m_target ? TokenType2Str[m_target->getType()] : "",
    // operation
    Operation2Str[m_operation],
    // arg1
    m_arg1 ? m_arg1->getASTValueStr() : "", sArgOneWidth + 2,
    m_arg1 ? TokenType2Str[m_arg1->getType()] : "",
    // arg2
    m_arg2 ? m_arg2->getASTValueStr() : "",
    m_arg2 ? TokenType2Str[m_arg2->getType()] : ""
  );
}
