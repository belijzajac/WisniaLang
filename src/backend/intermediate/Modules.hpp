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

#include <memory>
#include <unordered_map>
#include <vector>

namespace Wisnia {
class Instruction;

enum class Module {
  CALCULATE_STRING_LENGTH,
  EXIT
};

static inline std::unordered_map<Module, std::string> Module2Str {
  {Module::CALCULATE_STRING_LENGTH, "_calculate_string_length_"},
  {Module::EXIT, "_exit_"},
};

class Modules {
  using instructions_list = std::vector<std::shared_ptr<Instruction>>;

 private:
  static instructions_list moduleCalculateStringLength();
  static instructions_list moduleExit();

 public:
  static instructions_list getModule(Module module);
};

}  // namespace Wisnia

#endif  // WISNIALANG_MODULES_HPP
