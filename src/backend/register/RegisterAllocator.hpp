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

#ifndef WISNIALANG_REGISTERALLOCATOR_HPP
#define WISNIALANG_REGISTERALLOCATOR_HPP

#include <memory>
#include <vector>

namespace Wisnia {
class Instruction;

class RegisterAllocator {
  using instructions_list = std::vector<std::shared_ptr<Instruction>>;

 public:
  void printInstructions() const;
  void allocateRegisters(const instructions_list &instructions);

 private:
  instructions_list m_instructions;
};

}  // namespace Wisnia

#endif  // WISNIALANG_REGISTERALLOCATOR_HPP
