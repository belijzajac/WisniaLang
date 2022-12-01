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
  {Module::CALCULATE_STRING_LENGTH, "_calculate_string_length_"},
  {Module::PRINT_NUMBER,            "_print_number_"           },
  {Module::PRINT_BOOLEAN,           "_print_boolean_"          },
  {Module::EXIT,                    "_exit_"                   },
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
