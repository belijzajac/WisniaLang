// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_REGISTER_ALLOCATOR_HPP
#define WISNIALANG_REGISTER_ALLOCATOR_HPP

#include <algorithm>
#include <array>
#include <memory>
// Wisnia
#include "IRPrintHelper.hpp"

namespace Wisnia {
class Instruction;

class RegisterAllocator {
  using InstructionList = std::vector<std::shared_ptr<Instruction>>;

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

    std::array<register_t, 15> m_registers {{
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
  const InstructionList &getInstructions() const { return m_instructions; }
  void print(std::ostream &output) const { IRPrintHelper::print(output, m_instructions); }
  void allocate(InstructionList &&instructions, bool allocateRegisters = true);

  static constexpr std::array<std::string_view, 15> getAllRegisters() {
    return {"rax", "rcx", "rdx", "rbx", "rbp", "rsi", "rdi", "r8",
            "r9",  "r10", "r11", "r12", "r13", "r14", "r15"};
  };

 private:
  InstructionList m_instructions;
};

}  // namespace Wisnia

#endif  // WISNIALANG_REGISTER_ALLOCATOR_HPP
