// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_MACHINE_CODE_TABLE_HPP
#define WISNIALANG_MACHINE_CODE_TABLE_HPP

#include <cstdint>
// Wisnia
#include "ByteArray.hpp"
#include "Exceptions.hpp"
#include "Register.hpp"

namespace Wisnia {

template <typename>
class MachineCodeTable {};

template <>
class MachineCodeTable<uint8_t> {
public:
  // machine code for `cmp byte ptr` instruction
  static constexpr ByteArray getCmpPtrMachineCode(const Basic::register_t reg) {
    switch (reg) {
      case Basic::register_t::RAX: return {std::byte{0x80}, std::byte{0x38}};
      case Basic::register_t::RCX: return {std::byte{0x80}, std::byte{0x39}};
      case Basic::register_t::RDX: return {std::byte{0x80}, std::byte{0x3a}};
      case Basic::register_t::RBX: return {std::byte{0x80}, std::byte{0x3b}};
      case Basic::register_t::RSP: return {std::byte{0x80}, std::byte{0x3c}};
      case Basic::register_t::RBP: return {std::byte{0x80}, std::byte{0x7d}};
      case Basic::register_t::RSI: return {std::byte{0x80}, std::byte{0x3e}};
      case Basic::register_t::RDI: return {std::byte{0x80}, std::byte{0x3f}};
      case Basic::register_t::R8:  return {std::byte{0x41}, std::byte{0x80}, std::byte{0x38}};
      case Basic::register_t::R9:  return {std::byte{0x41}, std::byte{0x80}, std::byte{0x39}};
      case Basic::register_t::R10: return {std::byte{0x41}, std::byte{0x80}, std::byte{0x3a}};
      case Basic::register_t::R11: return {std::byte{0x41}, std::byte{0x80}, std::byte{0x3b}};
      case Basic::register_t::R12: return {std::byte{0x41}, std::byte{0x80}, std::byte{0x3c}};
      case Basic::register_t::R13: return {std::byte{0x41}, std::byte{0x80}, std::byte{0x7d}};
      case Basic::register_t::R14: return {std::byte{0x41}, std::byte{0x80}, std::byte{0x3e}};
      case Basic::register_t::R15: return {std::byte{0x41}, std::byte{0x80}, std::byte{0x3f}};
      default: throw CodeGenerationError{"Unknown register for cmp byte ptr instruction"};
    }
  }
};

template <>
class MachineCodeTable<uint32_t> {
public:
  // machine code for 32-bit mov instruction
  static constexpr ByteArray getMovMachineCode(const Basic::register_t reg) {
    switch (reg) {
      case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc0}};
      case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc1}};
      case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc2}};
      case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc3}};
      case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc4}};
      case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc5}};
      case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc6}};
      case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0xc7}, std::byte{0xc7}};
      case Basic::register_t::R8:  return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc0}};
      case Basic::register_t::R9:  return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc1}};
      case Basic::register_t::R10: return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc2}};
      case Basic::register_t::R11: return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc3}};
      case Basic::register_t::R12: return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc4}};
      case Basic::register_t::R13: return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc5}};
      case Basic::register_t::R14: return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc6}};
      case Basic::register_t::R15: return {std::byte{0x49}, std::byte{0xc7}, std::byte{0xc7}};
      default: throw CodeGenerationError{"Unknown register for mov instruction"};
    }
  }

  // machine code for 32-bit lea instruction
  static constexpr ByteArray getLeaMachineCode(const Basic::register_t reg) {
    switch (reg) {
      case Basic::register_t::EDX: return {std::byte{0x8d}, std::byte{0x94}, std::byte{0x24}};
      default: throw CodeGenerationError{"Unknown register for lea instruction"};
    }
  }

  // machine code for 32-bit cmp instruction
  static constexpr ByteArray getCmpMachineCode(const Basic::register_t reg) {
    switch (reg) {
      case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0x3d}};
      case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xf9}};
      case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xfa}};
      case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xfb}};
      case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xfc}};
      case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xfd}};
      case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xfe}};
      case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xff}};
      case Basic::register_t::R8:  return {std::byte{0x49}, std::byte{0x81}, std::byte{0xf8}};
      case Basic::register_t::R9:  return {std::byte{0x49}, std::byte{0x81}, std::byte{0xf9}};
      case Basic::register_t::R10: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xfa}};
      case Basic::register_t::R11: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xfb}};
      case Basic::register_t::R12: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xfc}};
      case Basic::register_t::R13: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xfd}};
      case Basic::register_t::R14: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xfe}};
      case Basic::register_t::R15: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xff}};
      default: throw CodeGenerationError{"Unknown register for cmp instruction"};
    }
  }

  // machine code for 32-bit add instruction
  static constexpr ByteArray getAddMachineCode(const Basic::register_t reg) {
    switch (reg) {
      case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0x05}};
      case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xc1}};
      case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xc2}};
      case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xc3}};
      case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xc4}};
      case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xc5}};
      case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xc6}};
      case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xc7}};
      case Basic::register_t::R8:  return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc0}};
      case Basic::register_t::R9:  return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc1}};
      case Basic::register_t::R10: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc2}};
      case Basic::register_t::R11: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc3}};
      case Basic::register_t::R12: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc4}};
      case Basic::register_t::R13: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc5}};
      case Basic::register_t::R14: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc6}};
      case Basic::register_t::R15: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xc7}};
      case Basic::register_t::EDX: return {std::byte{0x81}, std::byte{0xc2}};
      default: throw CodeGenerationError{"Unknown register for add instruction"};
    }
  }

  // machine code for 32-bit sub instruction
  static constexpr ByteArray getSubMachineCode(const Basic::register_t reg) {
    switch (reg) {
      case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0x2d}};
      case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xe9}};
      case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xea}};
      case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xeb}};
      case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xec}};
      case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xed}};
      case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xee}};
      case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0x81}, std::byte{0xef}};
      case Basic::register_t::R8:  return {std::byte{0x49}, std::byte{0x81}, std::byte{0xe8}};
      case Basic::register_t::R9:  return {std::byte{0x49}, std::byte{0x81}, std::byte{0xe9}};
      case Basic::register_t::R10: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xea}};
      case Basic::register_t::R11: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xeb}};
      case Basic::register_t::R12: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xec}};
      case Basic::register_t::R13: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xed}};
      case Basic::register_t::R14: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xee}};
      case Basic::register_t::R15: return {std::byte{0x49}, std::byte{0x81}, std::byte{0xef}};
      default: throw CodeGenerationError{"Unknown register for sub instruction"};
    }
  }

  // machine code for 32-bit imul instruction
  static constexpr ByteArray getMulMachineCode(const Basic::register_t reg) {
    switch (reg) {
      case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xc0}};
      case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xc9}};
      case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xd2}};
      case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xdb}};
      case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xe4}};
      case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xed}};
      case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xf6}};
      case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0x69}, std::byte{0xff}};
      case Basic::register_t::R8:  return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xc0}};
      case Basic::register_t::R9:  return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xc9}};
      case Basic::register_t::R10: return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xd2}};
      case Basic::register_t::R11: return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xdb}};
      case Basic::register_t::R12: return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xe4}};
      case Basic::register_t::R13: return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xed}};
      case Basic::register_t::R14: return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xf6}};
      case Basic::register_t::R15: return {std::byte{0x4d}, std::byte{0x69}, std::byte{0xff}};
      default: throw CodeGenerationError{"Unknown register for imul instruction"};
    }
  }
};

}  // namespace Wisnia

#endif  // WISNIALANG_MACHINE_CODE_TABLE_HPP
