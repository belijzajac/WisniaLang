// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_MODULES_HPP
#define WISNIALANG_MODULES_HPP

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Wisnia {
class Instruction;

enum Module : uint8_t {
  CALCULATE_STRING_LENGTH,
  PRINT_NUMBER,
  PRINT_BOOLEAN,
  EXIT
};

static inline std::unordered_map<Module, std::string_view> Module2Str {
  {Module::CALCULATE_STRING_LENGTH, "__builtin_calculate_string_length"},
  {Module::PRINT_NUMBER,            "__builtin_print_number"           },
  {Module::PRINT_BOOLEAN,           "__builtin_print_boolean"          },
  {Module::EXIT,                    "__builtin_exit"                   },
};

class Modules {
  using InstructionList = std::vector<std::shared_ptr<Instruction>>;

  static inline std::array<bool, 4> m_isUsed {
    false, false, false, false
  };

 private:
  static InstructionList moduleCalculateStringLength();
  static InstructionList modulePrintUintNumber();
  static InstructionList modulePrintBoolean();
  static InstructionList moduleExit();

 public:
  static std::tuple<InstructionList, bool> getModule(Module module);
  static void markAsUsed(Module module) { m_isUsed[module] = true; }
  static void markAllAsUnused() { m_isUsed.fill(false); }
};

}  // namespace Wisnia

#endif  // WISNIALANG_MODULES_HPP
