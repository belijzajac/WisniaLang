// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_CODE_GENERATOR_HPP
#define WISNIALANG_CODE_GENERATOR_HPP

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
  void generate(const std::vector<InstructionValue> &instructions);

 private:
  void emitLea(const InstructionValue &instruction);
  void emitMove(const InstructionValue &instruction, bool label = false);
  void emitMoveMemory(const InstructionValue &instruction);
  void emitSysCall();
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
  void emitTest(const InstructionValue &instruction);
  void emitRet();

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

#endif  // WISNIALANG_CODE_GENERATOR_HPP
