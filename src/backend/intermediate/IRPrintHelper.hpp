// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_IR_PRINT_HELPER_HPP
#define WISNIALANG_IR_PRINT_HELPER_HPP

#include <memory>
#include <vector>

namespace Wisnia {
class Instruction;

class IRPrintHelper {
  using InstructionList = std::vector<std::shared_ptr<Instruction>>;

 public:
  static void print(std::ostream &output, const InstructionList &instructions);
};

}  // namespace Wisnia

#endif  // WISNIALANG_IR_PRINT_HELPER_HPP
