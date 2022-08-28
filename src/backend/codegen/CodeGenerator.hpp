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

#ifndef WISNIALANG_CODEGENERATOR_HPP
#define WISNIALANG_CODEGENERATOR_HPP

#include <memory>
#include <vector>
// Wisnia
#include "ByteArray.hpp"

namespace Wisnia {
class Instruction;

class CodeGenerator {
  using InstructionValue = std::unique_ptr<Instruction>;

 public:
  const ByteArray &getTextSection() const { return m_textSection; }
  const ByteArray &getDataSection() const { return m_dataSection; }
  void generateCode(const std::vector<InstructionValue> &instructions);

 private:
  void emitMove(const InstructionValue &instruction);

 private:
  std::vector<InstructionValue> m_instructions;
  ByteArray m_textSection;
  ByteArray m_dataSection;
};

}  // namespace Wisnia

#endif  // WISNIALANG_CODEGENERATOR_HPP
