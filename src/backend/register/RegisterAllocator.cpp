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
#include "RegisterAllocator.hpp"
#include "Instruction.hpp"

using namespace Wisnia;

void RegisterAllocator::printInstructions() const {
  size_t index = 0;
  fmt::print("{:^34}|{:^9}|{:^34}|{:^34}\n", "Target", "Op", "Arg1", "Arg2");
  fmt::print("{:->{}}{:->{}}{:->{}}{:->{}}\n", "+", 35, "+", 10, "+", 35, "", 34);
  for (const auto &ir : m_instructions) {
    ir->print();
    ++index;
  }
}

void RegisterAllocator::allocateRegisters(const instructions_list &instructions) {
  m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
}
