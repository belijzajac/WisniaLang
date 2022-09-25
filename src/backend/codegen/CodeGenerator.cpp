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

static inline std::unordered_map<std::string, ByteArray> IncRegisterMachineCode {
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
      case Operation::CMP_BYTE_PTR:
        emitCmpBytePtr(instruction);
        break;
      case Operation::JMP:
      case Operation::JE:
        emitJmp(instruction);
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
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionValue &instruction, bool label) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // Move a number to register
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(MovMachineCode[target->getValue<std::string>()]);
    if (label) {
      m_data.emplace_back(Data{m_textSection.size(), static_cast<size_t>(argOne->getValue<int>())});
    }
    m_textSection.putU32(argOne->getValue<int>());
  }
  // Move a string to register
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_STR) {
    auto strVal = argOne->getValue<std::string>();
    for (const auto ch : strVal) {
      m_dataSection.putBytes(std::byte(ch));
    }
    argOne->setType(TType::LIT_INT);
    argOne->setValue(std::abs(static_cast<int>(m_dataSection.size() - strVal.size())));
    emitMove(instruction, true);
  }
  // Move a register to another register
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    m_textSection.putBytes(std::byte{0x48}, std::byte{0x89});
    m_textSection.putBytes(calculateRM(target->getValue<std::string>(), argOne->getValue<std::string>()));
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
  m_textSection.putBytes(std::byte{0xe8});

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

  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(CmpBytePtrMachineCode[instruction->getArg1()->getValue<std::string>()]);
    m_textSection.putBytes(std::byte(argTwo->getValue<int>()));
  }
}

void CodeGenerator::emitJmp(const CodeGenerator::InstructionValue &instruction) {
  const auto getOperandByte = [&]() -> std::byte {
    switch (instruction->getOperation()) {
      case Operation::JMP: return std::byte{0xeb};
      case Operation::JE:  return std::byte{0x74};
      default: throw InstructionError{"Unknown jump operand"};
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
  // Increment the contents of a register
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(IncRegisterMachineCode[instruction->getArg1()->getValue<std::string>()]);
  }
}

void CodeGenerator::emitRet(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xc3});
}

void CodeGenerator::emitNop(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0x90});
}
