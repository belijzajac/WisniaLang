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

#include <array>
#include <cassert>
#include <iostream>
// Wisnia
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

static inline std::unordered_map<std::string, ByteArray> MovMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc7}}},
};

static inline std::unordered_map<std::string, ByteArray> PushRegisterMachineCode {
  {"rax", ByteArray{std::byte{0x50}}},
  {"rcx", ByteArray{std::byte{0x51}}},
  {"rdx", ByteArray{std::byte{0x52}}},
  {"rbx", ByteArray{std::byte{0x53}}},
  {"rsp", ByteArray{std::byte{0x54}}},
  {"rbp", ByteArray{std::byte{0x55}}},
  {"rsi", ByteArray{std::byte{0x56}}},
  {"rdi", ByteArray{std::byte{0x57}}},
  {"r8",  ByteArray{std::byte{0x41}, std::byte{0x50}}},
  {"r9",  ByteArray{std::byte{0x41}, std::byte{0x51}}},
  {"r10", ByteArray{std::byte{0x41}, std::byte{0x52}}},
  {"r11", ByteArray{std::byte{0x41}, std::byte{0x53}}},
  {"r12", ByteArray{std::byte{0x41}, std::byte{0x54}}},
  {"r13", ByteArray{std::byte{0x41}, std::byte{0x55}}},
  {"r14", ByteArray{std::byte{0x41}, std::byte{0x56}}},
  {"r15", ByteArray{std::byte{0x41}, std::byte{0x57}}},
};

static inline std::unordered_map<std::string, ByteArray> PopRegisterMachineCode {
  {"rax", ByteArray{std::byte{0x58}}},
  {"rcx", ByteArray{std::byte{0x59}}},
  {"rdx", ByteArray{std::byte{0x5a}}},
  {"rbx", ByteArray{std::byte{0x5b}}},
  {"rsp", ByteArray{std::byte{0x5c}}},
  {"rbp", ByteArray{std::byte{0x5d}}},
  {"rsi", ByteArray{std::byte{0x5e}}},
  {"rdi", ByteArray{std::byte{0x5f}}},
  {"r8",  ByteArray{std::byte{0x41}, std::byte{0x58}}},
  {"r9",  ByteArray{std::byte{0x41}, std::byte{0x59}}},
  {"r10", ByteArray{std::byte{0x41}, std::byte{0x5a}}},
  {"r11", ByteArray{std::byte{0x41}, std::byte{0x5b}}},
  {"r12", ByteArray{std::byte{0x41}, std::byte{0x5c}}},
  {"r13", ByteArray{std::byte{0x41}, std::byte{0x5d}}},
  {"r14", ByteArray{std::byte{0x41}, std::byte{0x5e}}},
  {"r15", ByteArray{std::byte{0x41}, std::byte{0x5f}}},
};

std::byte calculateRM(std::string_view destination, std::string_view source) {
  constexpr std::array<std::string_view, 8> registers {
    "rax", "rcx", "rdx", "rbx",
    "rsp", "rbp", "rsi", "rdi"
  };

  auto dst{-1}, src{-1};
  for (auto i = 0; i < registers.size(); i++) {
    if (registers[i] == destination) dst = i;
    if (registers[i] == source) src = i;
    if (src > -1 && dst > -1) break;
  }

  assert((dst > -1 && src > -1) && "failed to look up registers");
  auto result = 0xc0 + (8 * src) + dst;
  assert(result < 255 && "result value is out of range");

  return std::byte(result);
}

void CodeGenerator::generateCode(const std::vector<CodeGenerator::InstructionValue> &instructions) {
  for (const auto &instruction : instructions) {
    switch (instruction->getOperation()) {
      case Operation::MOV:
        emitMove(instruction);
        break;
      case Operation::SYSCALL:
        emitSysCall(instruction);
        break;
      case Operation::PUSH:
        emitPush(instruction);
        break;
      case Operation::POP:
        emitPop(instruction);
        break;
      case Operation::CALL:
        emitCall(instruction);
        break;
      case Operation::LABEL:
        emitLabel(instruction);
        break;
      case Operation::CMP_BYTE_ADDR:
        emitCmpByteAddr(instruction);
        break;
      case Operation::JMP:
        emitJmp(instruction);
        break;
      case Operation::JE:
        emitJe(instruction);
        break;
      case Operation::INC:
        emitInc(instruction);
        break;
      case Operation::RET:
        emitRet(instruction);
        break;
      case Operation::NOP:
        emitNop(instruction);
        break;
      default:
        throw CodeGenerationError{"Unknown operation to generate the code for"};
    }
  }

  // Patch data
  for (const auto &patch : m_patches) {
    auto start{patch.m_start};
    auto offset{patch.m_offset};

    auto newAddress{kVirtualStartAddress + offset + m_textSection.size() + kTextOffset};
    ByteArray bytes{};
    bytes.putU32(newAddress);

    for (size_t i = 0; i < bytes.size(); i++) {
      m_textSection.insert(i + start, bytes.data()[i]);
    }
  }
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionValue &instruction, bool label) {
  // Move a number to register
  if (instruction->getTarget()->getType() == TType::REGISTER && instruction->getArg1()->getType() == TType::LIT_INT) {
    m_textSection.putBytes(MovMachineCode[instruction->getTarget()->getValue<std::string>()]);
    if (label) {
      m_patches.emplace_back(Patch{m_textSection.size(), (size_t)instruction->getArg1()->getValue<int>()});
    }
    m_textSection.putU32(instruction->getArg1()->getValue<int>());
  }
  // Move a string to register
  if (instruction->getTarget()->getType() == TType::REGISTER && instruction->getArg1()->getType() == TType::LIT_STR) {
    auto strVal = instruction->getArg1()->getValue<std::string>();
    for (const auto ch : strVal) {
      m_dataSection.putBytes(std::byte(ch));
    }
    instruction->getArg1()->setType(TType::LIT_INT);
    instruction->getArg1()->setValue(std::abs(static_cast<int>(m_dataSection.size() - strVal.size())));
    emitMove(instruction, true);
  }
  // Move a register to another register
  if (instruction->getTarget()->getType() == TType::REGISTER && instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(std::byte{0x48}, std::byte{0x89});
    m_textSection.putBytes(calculateRM(instruction->getTarget()->getValue<std::string>(), instruction->getArg1()->getValue<std::string>()));
  }
}

void CodeGenerator::emitSysCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0x0f}, std::byte{0x05});
}

void CodeGenerator::emitPush(const CodeGenerator::InstructionValue &instruction) {
  // Push a register on the stack
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PushRegisterMachineCode[instruction->getArg1()->getValue<std::string>()]);
  }
}

void CodeGenerator::emitPop(const CodeGenerator::InstructionValue &instruction) {
  // Pop stack value into a register
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PopRegisterMachineCode[instruction->getArg1()->getValue<std::string>()]);
  }
}

void CodeGenerator::emitCall(const CodeGenerator::InstructionValue &instruction) {
  //...
}

void CodeGenerator::emitLabel(const CodeGenerator::InstructionValue &instruction) {
  //...
}

void CodeGenerator::emitCmpByteAddr(const CodeGenerator::InstructionValue &instruction) {
  //...
}

void CodeGenerator::emitJmp(const CodeGenerator::InstructionValue &instruction) {
  //...
}

void CodeGenerator::emitJe(const CodeGenerator::InstructionValue &instruction) {
  //...
}

void CodeGenerator::emitInc(const CodeGenerator::InstructionValue &instruction) {
  //...
}

void CodeGenerator::emitRet(const CodeGenerator::InstructionValue &instruction) {
  //...
}

void CodeGenerator::emitNop(const CodeGenerator::InstructionValue &instruction) {
  //...
}
