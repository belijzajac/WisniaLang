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

#include <algorithm>
// Wisnia
#include "IROptimization.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

void IROptimization::optimize(IROptimization::InstructionList &&instructions) {
  removeRedundantInstructions(instructions);
  m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
}

void IROptimization::removeRedundantInstructions(InstructionList &instructions) {
  // Redundant instructions, e.g. mov rax, rax
  instructions.erase(
    std::remove_if(instructions.begin(), instructions.end(),
    [](const auto &instruction) {
      const auto &op     = instruction->getOperation();
      const auto &target = instruction->getTarget();
      const auto &argOne = instruction->getArg1();

      return op == Operation::MOV && target->getType() == TType::REGISTER &&
             argOne->getType() == TType::REGISTER &&
             target->template getValue<Basic::register_t>() == argOne->template getValue<Basic::register_t>();
    }),
    instructions.end()
  );
}
