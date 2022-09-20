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
#include "InstructionPrintHelper.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;

void InstructionPrintHelper::print(const InstructionPrintHelper::instructions_list& instructions) {
  size_t maxTargetWidth{0}, maxArgOneWidth{0};
  for (const auto &ir : instructions) {
    if (const auto &token = ir->getTarget(); token) {
      maxTargetWidth = std::max(maxTargetWidth, token->getASTValueStr().size());
    }
    if (const auto &instr = ir->getArg1(); instr) {
      maxArgOneWidth = std::max(maxArgOneWidth, instr->getASTValueStr().size());
    }
  }

  Instruction::setTargetWidth(maxTargetWidth);
  Instruction::setArgOneWidth(maxArgOneWidth);

  fmt::print("{:^{}}|{:^9}|{:^{}}|{:^34}\n", "Target", maxTargetWidth + 21, "Op", "Arg1", maxArgOneWidth + 21, "Arg2");
  fmt::print("{:->{}}{:->{}}{:->{}}{:->{}}\n", "+", maxTargetWidth + 22, "+", 10, "+", maxArgOneWidth + 22, "", 34);
  for (const auto &ir : instructions) {
    ir->print();
  }
}
