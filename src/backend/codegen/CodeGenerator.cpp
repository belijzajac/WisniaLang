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

#include <algorithm>
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

static inline std::unordered_map<std::string, ByteArray> LeaMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x44}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x4c}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x54}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x5c}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x64}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x6c}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x74}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x7c}}},
  {"r8",  ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x44}}},
  {"r9",  ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x4c}}},
  {"r10", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x54}}},
  {"r11", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x5c}}},
  {"r12", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x64}}},
  {"r13", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x6c}}},
  {"r14", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x74}}},
  {"r15", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x7c}}},
  {"edx", ByteArray{std::byte{0x8d}, std::byte{0x54}, std::byte{0x24}}},
};

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

static inline std::unordered_map<std::string, ByteArray> PushMachineCode {
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

static inline std::unordered_map<std::string, ByteArray> PopMachineCode {
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

static inline std::unordered_map<std::string, ByteArray> IncMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc7}}},
};

static inline std::unordered_map<std::string, ByteArray> DecMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc8}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xca}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcc}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcd}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xce}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcf}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc8}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc9}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xca}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcb}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcc}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcd}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xce}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcf}}},
};

static inline std::unordered_map<std::string, ByteArray> CmpBytePtrMachineCode {
  {"rax", ByteArray{std::byte{0x80}, std::byte{0x38}}},
  {"rcx", ByteArray{std::byte{0x80}, std::byte{0x39}}},
  {"rdx", ByteArray{std::byte{0x80}, std::byte{0x3a}}},
  {"rbx", ByteArray{std::byte{0x80}, std::byte{0x3b}}},
  {"rsp", ByteArray{std::byte{0x80}, std::byte{0x3c}}},
  {"rbp", ByteArray{std::byte{0x80}, std::byte{0x7d}}},
  {"rsi", ByteArray{std::byte{0x80}, std::byte{0x3e}}},
  {"rdi", ByteArray{std::byte{0x80}, std::byte{0x3f}}},
  {"r8",  ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x38}}},
  {"r9",  ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x39}}},
  {"r10", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3a}}},
  {"r11", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3b}}},
  {"r12", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3c}}},
  {"r13", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x7d}}},
  {"r14", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3e}}},
  {"r15", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3f}}},
};

static inline std::unordered_map<std::string, ByteArray> AddMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xc7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xc7}}},
  {"edx", ByteArray{std::byte{0x83}, std::byte{0xc2}}},
};

static inline std::unordered_map<std::string, ByteArray> SubMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xe8}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xe9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xea}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xeb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xec}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xed}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xee}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x83}, std::byte{0xef}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xe8}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xe9}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xea}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xeb}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xec}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xed}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xee}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0x83}, std::byte{0xef}}},
};

static inline std::unordered_map<std::string, ByteArray> DivMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf7}}},
};

static inline std::unordered_map<std::string, ByteArray> XorMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xc9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xd2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xdb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xe4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xed}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xf6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xff}}},
  {"r8",  ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xc9}}},
  {"r10", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xd2}}},
  {"r11", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xdb}}},
  {"r12", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xe4}}},
  {"r13", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xed}}},
  {"r14", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xf6}}},
  {"r15", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xff}}},
  {"edx", ByteArray{std::byte{0x31}, std::byte{0xd2}}},
};

static inline std::unordered_map<std::string, ByteArray> TestMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xc9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xd2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xdb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xe4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xed}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xf6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xff}}},
  {"r8",  ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xc9}}},
  {"r10", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xd2}}},
  {"r11", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xdb}}},
  {"r12", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xe4}}},
  {"r13", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xed}}},
  {"r14", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xf6}}},
  {"r15", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xff}}},
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

  assert((dst > -1 && src > -1) && "Failed to look up registers");
  auto result = 0xc0 + (8 * src) + dst;
  assert(result < 255 && "Result value is out of range");

  return std::byte(result);
}

void CodeGenerator::generateCode(const std::vector<CodeGenerator::InstructionValue> &instructions) {
  for (const auto &instruction : instructions) {
    switch (instruction->getOperation()) {
      case Operation::LEA:
        emitLea(instruction);
        break;
      case Operation::MOV:
        emitMove(instruction);
        break;
      case Operation::MOV_MEMORY:
        emitMoveMemory(instruction);
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
      case Operation::CMP_BYTE_PTR:
        emitCmpBytePtr(instruction);
        break;
      case Operation::JMP:
      case Operation::JE:
      case Operation::JZ:
      case Operation::JNE:
      case Operation::JNZ:
        emitJmp(instruction);
        break;
      case Operation::INC:
        emitInc(instruction);
        break;
      case Operation::DEC:
        emitDec(instruction);
        break;
      case Operation::IADD:
        emitAdd(instruction);
        break;
      case Operation::ISUB:
        emitSub(instruction);
        break;
      case Operation::IDIV:
        emitDiv(instruction);
        break;
      case Operation::XOR:
        emitXor(instruction);
        break;
      case Operation::OR:
        emitOr(instruction);
        break;
      case Operation::TEST:
        emitTest(instruction);
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
  for (const auto &data : m_data) {
    const auto start{data.m_start};
    const auto offset{data.m_offset};

    const uint32_t newAddress{
        static_cast<uint32_t>(kVirtualStartAddress + offset + m_textSection.size() + kTextOffset)};
    const ByteArray bytes{newAddress};

    // Overwrite the instruction
    for (size_t i = 0; i < bytes.size(); i++) {
      m_textSection.insert(i + start, bytes.data()[i]);
    }
  }

  // Patch jumps
  for (const auto &jump : m_jumps) {
    const auto label = std::find_if(m_labels.begin(), m_labels.end(),
                                    [&](const auto &label) { return label.m_name == jump.m_name; });
    assert(label != m_labels.end() && "No such label to jump to");
    const auto offset{label->m_offset};
    const auto diff{jump.m_offset - offset};

    // Overwrite the instruction
    m_textSection.insert(jump.m_offset, std::byte(0xff - diff));
  }

  // Patch calls
  for (const auto &call : m_calls) {
    const auto label = std::find_if(m_labels.begin(), m_labels.end(),
                                    [&](const auto &label) { return label.m_name == call.m_name; });
    assert(label != m_labels.end() && "No such label to call");
    const auto offset{label->m_offset};

    // The offset of the position is a byte
    const uint32_t diff{static_cast<uint32_t>(call.m_offset - offset + 4)};
    const uint32_t x{0xffffffff - static_cast<uint32_t>(diff - 1)};
    const ByteArray bytes{x};

    // Overwrite the instruction
    for (size_t i = 0; i < bytes.size(); i++) {
      m_textSection.insert(i + call.m_offset, bytes.data()[i]);
    }
  }
}

void CodeGenerator::emitLea(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // lea reg, [rsp + number]
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(LeaMachineCode[target->getValue<std::string>()]);
    m_textSection.putBytes(std::byte(argOne->getValue<int>()));
    return;
  }

  assert(0 && "Unknown lea operation");
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionValue &instruction, bool label) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // mov reg, number
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(MovMachineCode[target->getValue<std::string>()]);
    if (label) {
      m_data.emplace_back(Data{m_textSection.size(), static_cast<size_t>(argOne->getValue<int>())});
    }
    m_textSection.putU32(argOne->getValue<int>());
    return;
  }

  // mov reg, "string"
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_STR) {
    auto strVal = argOne->getValue<std::string>();
    for (const auto ch : strVal) {
      m_dataSection.putBytes(std::byte(ch));
    }
    argOne->setType(TType::LIT_INT);
    argOne->setValue(std::abs(static_cast<int>(m_dataSection.size() - strVal.size())));
    emitMove(instruction, true);
    return;
  }

  // mov reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    m_textSection.putBytes(std::byte{0x48}, std::byte{0x89});
    m_textSection.putBytes(calculateRM(target->getValue<std::string>(), argOne->getValue<std::string>()));
    return;
  }

  assert(0 && "Unknown move operation");
}

void CodeGenerator::emitMoveMemory(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // mov [rsi], dl
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER &&
      target->getValue<std::string>() == "rsi" && argOne->getValue<std::string>() == "dl") {
    m_textSection.putBytes(std::byte{0x88}, std::byte{0x16});
    return;
  }

  assert(0 && "Unknown move memory operation");
}

void CodeGenerator::emitSysCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0x0f}, std::byte{0x05});
}

void CodeGenerator::emitPush(const CodeGenerator::InstructionValue &instruction) {
  // push reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PushMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  assert(0 && "Unknown push operation");
}

void CodeGenerator::emitPop(const CodeGenerator::InstructionValue &instruction) {
  // pop reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PopMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  assert(0 && "Unknown pop operation");
}

void CodeGenerator::emitCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xe8});

  // call label
  const auto &label{instruction->getTarget()->getValue<std::string>()};
  const auto offset{m_textSection.size()};
  m_calls.emplace_back(Label{label, offset});

  m_textSection.putU32(0);
}

void CodeGenerator::emitLabel(const CodeGenerator::InstructionValue &instruction) {
  const auto &label = instruction->getArg1()->getValue<std::string>();
  const auto offset = m_textSection.size();
  m_labels.emplace_back(Label{label, offset});
}

void CodeGenerator::emitCmpBytePtr(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // cmp byte ptr [reg], number
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(CmpBytePtrMachineCode[instruction->getArg1()->getValue<std::string>()]);
    m_textSection.putBytes(std::byte(argTwo->getValue<int>()));
    return;
  }

  assert(0 && "Unknown cmp byte ptr operation");
}

void CodeGenerator::emitJmp(const CodeGenerator::InstructionValue &instruction) {
  const auto getOperandByte = [&]() -> std::byte {
    switch (instruction->getOperation()) {
      case Operation::JMP:
        return std::byte{0xeb};
      case Operation::JE:
      case Operation::JZ:
        return std::byte{0x74};
      case Operation::JNE:
      case Operation::JNZ:
        return std::byte{0x75};
      default: assert(0 && "Unknown jump operation");
    }
  };

  m_textSection.putBytes(getOperandByte());

  const auto &label = instruction->getArg1()->getValue<std::string>();
  const auto offset = m_textSection.size();
  m_jumps.emplace_back(Label{label, offset});

  // Empty displacement
  m_textSection.putBytes(std::byte{0x00});
}

void CodeGenerator::emitInc(const CodeGenerator::InstructionValue &instruction) {
  // inc reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(IncMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  assert(0 && "Unknown inc operation");
}

void CodeGenerator::emitDec(const CodeGenerator::InstructionValue &instruction) {
  // dec reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(DecMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  assert(0 && "Unknown dec operation");
}

void CodeGenerator::emitAdd(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // add reg, number
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(AddMachineCode[target->getValue<std::string>()]);
    m_textSection.putBytes(std::byte(argOne->getValue<int>()));
    return;
  }

  // add reg1, reg2
  if (target->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER) {
    constexpr std::array<std::string_view, 16> registers {
      "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
      "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15",
    };

    constexpr auto halfRegisters{registers.size() / 2};
    auto dst{-1}, src{-1};

    for (auto i = 0; i < registers.size(); i++) {
      if (registers[i] == target->getValue<std::string>()) dst = i;
      if (registers[i] == argTwo->getValue<std::string>()) src = i;
      if (src > -1 && dst > -1) break;
    }

    assert((dst > -1 && src > -1) && "Failed to look up registers for add operation");

    //  +        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < halfRegisters && src < halfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x01});
    } else if (dst < halfRegisters && src >= halfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x01});
    } else if (dst >= halfRegisters && src < halfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x01});
    } else if (dst >= halfRegisters && src >= halfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x01});
    } else {
      assert(0 && "Unknown table entry for add operation");
    }

    auto result = 0xc0 + (8 * (src % halfRegisters)) + (dst % halfRegisters);
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  assert(0 && "Unknown add operation");
}

void CodeGenerator::emitSub(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // sub reg, number
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(SubMachineCode[target->getValue<std::string>()]);
    m_textSection.putBytes(std::byte(argOne->getValue<int>()));
    return;
  }

  // sub edx, esi
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER &&
      target->getValue<std::string>() == "edx" && argOne->getValue<std::string>() == "esi") {
    m_textSection.putBytes(std::byte{0x29}, std::byte{0xf2});
    return;
  }

  assert(0 && "Unknown sub operation");
}

void CodeGenerator::emitDiv(const CodeGenerator::InstructionValue &instruction) {
  // div reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(DivMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  assert(0 && "Unknown div operation");
}

void CodeGenerator::emitXor(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // xor reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<std::string>() == argTwo->getValue<std::string>()) {
    m_textSection.putBytes(XorMachineCode[argOne->getValue<std::string>()]);
    return;
  }

  assert(0 && "Unknown xor operation");
}

void CodeGenerator::emitOr(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // or dl, 0x30
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::LIT_INT &&
      argOne->getValue<std::string>() == "dl") {
    m_textSection.putBytes(std::byte{0x80}, std::byte{0xca});
    m_textSection.putBytes(std::byte(argTwo->getValue<int>()));
    return;
  }

  assert(0 && "Unknown or operation");
}

void CodeGenerator::emitTest(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // test reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<std::string>() == argTwo->getValue<std::string>()) {
    m_textSection.putBytes(TestMachineCode[argOne->getValue<std::string>()]);
    return;
  }

  assert(0 && "Unknown test operation");
}

void CodeGenerator::emitRet(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xc3});
}

void CodeGenerator::emitNop(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0x90});
}
