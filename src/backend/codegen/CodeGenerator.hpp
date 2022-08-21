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

#ifndef WISNIALANG_CODE_GENERATOR_HPP
#define WISNIALANG_CODE_GENERATOR_HPP

#include <memory>
#include <vector>

namespace Wisnia {
class Instruction;

class CodeGenerator {
  using instruction_list = std::vector<std::unique_ptr<Instruction>>;
  using byte_list = std::vector<std::byte>;
 public:
  const byte_list &getTextSection() const { return m_textSection; }
  const byte_list &getDataSection() const { return m_dataSection; }
  void generateCode(const instruction_list &instructions);
 private:
  void emitMove(const std::unique_ptr<Instruction> &instruction);
 private:
  instruction_list m_instructions;
  byte_list m_textSection;
  byte_list m_dataSection;
};

}  // namespace Wisnia

#endif  // WISNIALANG_CODE_GENERATOR_HPP
