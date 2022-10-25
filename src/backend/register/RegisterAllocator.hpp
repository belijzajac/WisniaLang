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

#ifndef WISNIALANG_REGISTERALLOCATOR_HPP
#define WISNIALANG_REGISTERALLOCATOR_HPP

#include <algorithm>
#include <array>
#include <memory>
// Wisnia
#include "InstructionPrintHelper.hpp"

namespace Wisnia {
class Instruction;

class RegisterAllocator {
  using instructions_list = std::vector<std::shared_ptr<Instruction>>;

  struct Registers {
    struct register_t {
      std::string m_name;
      bool m_assigned;
    };

    register_t &operator[](std::string_view reg) {
      auto it = std::find_if(m_registers.begin(), m_registers.end(), [&](const auto &r) { return r.m_name == reg; });
      if (it != m_registers.end()) return *it;
      throw std::runtime_error{"Failed to look up register"};
    }

    std::array<register_t, 16> m_registers {{
      {"rax", false}, {"rcx", false}, {"rdx", false}, {"rbx", false}, {"rbp", false},
      {"rsi", false}, {"rdi", false}, {"r8" , false}, {"r9",  false}, {"r10", false},
      {"r11", false}, {"r12", false}, {"r13", false}, {"r14", false}, {"r15", false},
    }};
  };

  struct Live {
    std::string m_variable;
    std::string m_register;
    size_t m_start, m_end;
  };

 public:
  const instructions_list &getInstructions() const { return m_instructions; }
  void printInstructions() const { InstructionPrintHelper::print(m_instructions); }
  void allocateRegisters(instructions_list &&instructions, bool allocateRegisters = true);

 private:
  instructions_list m_instructions;
};

}  // namespace Wisnia

#endif  // WISNIALANG_REGISTERALLOCATOR_HPP
