// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <fmt/format.h>
// Wisnia
#include "IRPrintHelper.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;

void IRPrintHelper::print(std::ostream &output, const InstructionList &instructions) {
  size_t maxTargetWidth{0}, maxArgOneWidth{0};
  for (const auto &ir : instructions) {
    if (const auto &token = ir->getTarget(); token) {
      maxTargetWidth = std::max(maxTargetWidth, token->getASTValueStr().size());
    }
    if (const auto &instr = ir->getArg1(); instr) {
      maxArgOneWidth = std::max(maxArgOneWidth, instr->getASTValueStr().size());
    }
  }

  Instruction::setPrintTargetWidth(maxTargetWidth);
  Instruction::setPrintArgOneWidth(maxArgOneWidth);

  output << fmt::format("{:^{}}|{:^14}|{:^{}}|{:^34}\n", "Target", maxTargetWidth + 21, "Op", "Arg1", maxArgOneWidth + 21, "Arg2");
  output << fmt::format("{:->{}}{:->{}}{:->{}}{:->{}}\n", "+", maxTargetWidth + 22, "+", 15, "+", maxArgOneWidth + 22, "", 34);
  for (const auto &ir : instructions) {
    ir->print(output);
  }
}
