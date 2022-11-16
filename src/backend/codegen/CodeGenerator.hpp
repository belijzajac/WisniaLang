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
  using InstructionValue = std::shared_ptr<Instruction>;

  struct Data {
    size_t m_start;
    size_t m_offset;
  };

  struct Label {
    std::string m_name;
    size_t m_offset;
  };

 public:
  const ByteArray &getTextSection() const { return m_textSection; }
  const ByteArray &getDataSection() const { return m_dataSection; }
  void generateCode(const std::vector<InstructionValue> &instructions);

 private:
  void emitLea(const InstructionValue &instruction);
  void emitMove(const InstructionValue &instruction, bool label = false);
  void emitMoveMemory(const InstructionValue &instruction);
  void emitSysCall(const InstructionValue &instruction);
  void emitPush(const InstructionValue &instruction);
  void emitPop(const InstructionValue &instruction);
  void emitCall(const InstructionValue &instruction);
  void emitLabel(const InstructionValue &instruction);
  void emitCmp(const InstructionValue &instruction);
  void emitCmpBytePtr(const InstructionValue &instruction);
  void emitJmp(const InstructionValue &instruction);
  void emitInc(const InstructionValue &instruction);
  void emitDec(const InstructionValue &instruction);
  void emitAdd(const InstructionValue &instruction);
  void emitSub(const InstructionValue &instruction);
  void emitMul(const InstructionValue &instruction);
  void emitDiv(const InstructionValue &instruction);
  void emitXor(const InstructionValue &instruction);
  void emitOr(const InstructionValue &instruction);
  void emitTest(const InstructionValue &instruction);
  void emitRet(const InstructionValue &instruction);

 private:
  std::vector<InstructionValue> m_instructions;
  ByteArray m_textSection;
  ByteArray m_dataSection;
  std::vector<size_t> m_dataOffsets;
  std::vector<Data> m_data;
  std::vector<Label> m_labels;
  std::vector<Label> m_calls;
  std::vector<Label> m_jumps;
};

}  // namespace Wisnia

#endif  // WISNIALANG_CODEGENERATOR_HPP
