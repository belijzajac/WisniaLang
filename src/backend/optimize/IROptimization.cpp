// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

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
