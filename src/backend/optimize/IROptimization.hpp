// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_IR_OPTIMIZATION_HPP
#define WISNIALANG_IR_OPTIMIZATION_HPP

#include <memory>
// Wisnia
#include "IRPrintHelper.hpp"

namespace Wisnia {
class Instruction;

class IROptimization {
  using InstructionList = std::vector<std::shared_ptr<Instruction>>;

 public:
  void optimize(InstructionList &&instructions);
  const InstructionList &getInstructions() const { return m_instructions; }
  void print(std::ostream &output) const { IRPrintHelper::print(output, m_instructions); }

 private:
  void removeRedundantInstructions(InstructionList &instructions) const;

 private:
  InstructionList m_instructions;
};

}  // namespace Wisnia

#endif  // WISNIALANG_IR_OPTIMIZATION_HPP
