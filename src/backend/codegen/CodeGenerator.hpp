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
  using InstructionPtr = std::shared_ptr<Instruction>;

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
  void generate(const std::vector<InstructionPtr> &instructions);

 private:
  void emitLea(const InstructionPtr &instruction);
  void emitMove(const InstructionPtr &instruction, bool label = false);
  void emitMoveMemory(const InstructionPtr &instruction);
  void emitSysCall();
  void emitPush(const InstructionPtr &instruction);
  void emitPop(const InstructionPtr &instruction);
  void emitCall(const InstructionPtr &instruction);
  void emitLabel(const InstructionPtr &instruction);
  void emitCmp(const InstructionPtr &instruction);
  void emitCmpBytePtr(const InstructionPtr &instruction);
  void emitJmp(const InstructionPtr &instruction);
  void emitInc(const InstructionPtr &instruction);
  void emitDec(const InstructionPtr &instruction);
  void emitAdd(const InstructionPtr &instruction);
  void emitSub(const InstructionPtr &instruction);
  void emitMul(const InstructionPtr &instruction);
  void emitDiv(const InstructionPtr &instruction);
  void emitXor(const InstructionPtr &instruction);
  void emitTest(const InstructionPtr &instruction);
  void emitRet();

 private:
  std::vector<InstructionPtr> m_instructions;
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
