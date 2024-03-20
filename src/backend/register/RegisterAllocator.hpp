// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_REGISTER_ALLOCATOR_HPP
#define WISNIALANG_REGISTER_ALLOCATOR_HPP

#include <algorithm>
#include <array>
#include <memory>
// Wisnia
#include "IRPrintHelper.hpp"
#include "Register.hpp"

namespace Wisnia {
class Instruction;

class RegisterAllocator {
  using InstructionList = std::vector<std::shared_ptr<Instruction>>;

  struct Registers {
    struct RegisterState {
      Basic::register_t m_register;
      bool m_assigned;
    };

    RegisterState &operator[](Basic::register_t reg) {
      auto it = std::find_if(m_registers.begin(), m_registers.end(), [&](const auto &r) { return r.m_register == reg; });
      if (it != m_registers.end()) return *it;
      throw std::runtime_error{"Failed to look up register"};
    }

    std::array<RegisterState, 15> m_registers {{
      {Basic::register_t::RAX, false},
      {Basic::register_t::RCX, false},
      {Basic::register_t::RDX, false},
      {Basic::register_t::RBX, false},
      {Basic::register_t::RBP, false},
      {Basic::register_t::RSI, false},
      {Basic::register_t::RDI, false},
      {Basic::register_t::R8,  false},
      {Basic::register_t::R9,  false},
      {Basic::register_t::R10, false},
      {Basic::register_t::R11, false},
      {Basic::register_t::R12, false},
      {Basic::register_t::R13, false},
      {Basic::register_t::R14, false},
      {Basic::register_t::R15, false},
    }};
  };

  struct Live {
    std::string m_variable;
    Basic::register_t m_register;
    size_t m_start, m_end;
  };

 public:
  const InstructionList &getInstructions() const { return m_instructions; }
  void print(std::ostream &output) const { IRPrintHelper::print(output, m_instructions); }
  void allocate(InstructionList &&instructions, bool allocateRegisters = true);

  // All the registers that the RegisterAllocator works with, except for `RSP`
  static constexpr std::array<Basic::register_t, 15> getAllocatableRegisters {
    Basic::register_t::RAX, Basic::register_t::RCX, Basic::register_t::RDX,
    Basic::register_t::RBX, Basic::register_t::RBP, Basic::register_t::RSI,
    Basic::register_t::RDI, Basic::register_t::R8,  Basic::register_t::R9,
    Basic::register_t::R10, Basic::register_t::R11, Basic::register_t::R12,
    Basic::register_t::R13, Basic::register_t::R14, Basic::register_t::R15,
  };

  // All the registers that are not to be used by RegisterAllocator, including `RSP`
  static constexpr std::array<Basic::register_t, 16> getAllRegisters {
    Basic::register_t::RAX, Basic::register_t::RCX, Basic::register_t::RDX,
    Basic::register_t::RBX, Basic::register_t::RSP, Basic::register_t::RBP,
    Basic::register_t::RSI, Basic::register_t::RDI, Basic::register_t::R8,
    Basic::register_t::R9,  Basic::register_t::R10, Basic::register_t::R11,
    Basic::register_t::R12, Basic::register_t::R13, Basic::register_t::R14,
    Basic::register_t::R15,
  };

  static constexpr auto kHalfRegisters{getAllRegisters.size() / 2};

 private:
  InstructionList m_instructions;
};

}  // namespace Wisnia

#endif  // WISNIALANG_REGISTER_ALLOCATOR_HPP
