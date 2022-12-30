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
#include "CodeFragment.hpp"
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

static inline std::unordered_map<Basic::register_t, ByteArray> PushMachineCode {
  {Basic::register_t::RAX, ByteArray{std::byte{0x50}}},
  {Basic::register_t::RCX, ByteArray{std::byte{0x51}}},
  {Basic::register_t::RDX, ByteArray{std::byte{0x52}}},
  {Basic::register_t::RBX, ByteArray{std::byte{0x53}}},
  {Basic::register_t::RSP, ByteArray{std::byte{0x54}}},
  {Basic::register_t::RBP, ByteArray{std::byte{0x55}}},
  {Basic::register_t::RSI, ByteArray{std::byte{0x56}}},
  {Basic::register_t::RDI, ByteArray{std::byte{0x57}}},
  {Basic::register_t::R8,  ByteArray{std::byte{0x41}, std::byte{0x50}}},
  {Basic::register_t::R9,  ByteArray{std::byte{0x41}, std::byte{0x51}}},
  {Basic::register_t::R10, ByteArray{std::byte{0x41}, std::byte{0x52}}},
  {Basic::register_t::R11, ByteArray{std::byte{0x41}, std::byte{0x53}}},
  {Basic::register_t::R12, ByteArray{std::byte{0x41}, std::byte{0x54}}},
  {Basic::register_t::R13, ByteArray{std::byte{0x41}, std::byte{0x55}}},
  {Basic::register_t::R14, ByteArray{std::byte{0x41}, std::byte{0x56}}},
  {Basic::register_t::R15, ByteArray{std::byte{0x41}, std::byte{0x57}}},
};

static inline std::unordered_map<Basic::register_t, ByteArray> PopMachineCode {
  {Basic::register_t::RAX, ByteArray{std::byte{0x58}}},
  {Basic::register_t::RCX, ByteArray{std::byte{0x59}}},
  {Basic::register_t::RDX, ByteArray{std::byte{0x5a}}},
  {Basic::register_t::RBX, ByteArray{std::byte{0x5b}}},
  {Basic::register_t::RSP, ByteArray{std::byte{0x5c}}},
  {Basic::register_t::RBP, ByteArray{std::byte{0x5d}}},
  {Basic::register_t::RSI, ByteArray{std::byte{0x5e}}},
  {Basic::register_t::RDI, ByteArray{std::byte{0x5f}}},
  {Basic::register_t::R8,  ByteArray{std::byte{0x41}, std::byte{0x58}}},
  {Basic::register_t::R9,  ByteArray{std::byte{0x41}, std::byte{0x59}}},
  {Basic::register_t::R10, ByteArray{std::byte{0x41}, std::byte{0x5a}}},
  {Basic::register_t::R11, ByteArray{std::byte{0x41}, std::byte{0x5b}}},
  {Basic::register_t::R12, ByteArray{std::byte{0x41}, std::byte{0x5c}}},
  {Basic::register_t::R13, ByteArray{std::byte{0x41}, std::byte{0x5d}}},
  {Basic::register_t::R14, ByteArray{std::byte{0x41}, std::byte{0x5e}}},
  {Basic::register_t::R15, ByteArray{std::byte{0x41}, std::byte{0x5f}}},
};

static inline std::unordered_map<Basic::register_t, ByteArray> IncMachineCode {
  {Basic::register_t::RAX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc0}}},
  {Basic::register_t::RCX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc1}}},
  {Basic::register_t::RDX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc2}}},
  {Basic::register_t::RBX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc3}}},
  {Basic::register_t::RSP, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc4}}},
  {Basic::register_t::RBP, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc5}}},
  {Basic::register_t::RSI, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc6}}},
  {Basic::register_t::RDI, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc7}}},
  {Basic::register_t::R8,  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc0}}},
  {Basic::register_t::R9,  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc1}}},
  {Basic::register_t::R10, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc2}}},
  {Basic::register_t::R11, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc3}}},
  {Basic::register_t::R12, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc4}}},
  {Basic::register_t::R13, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc5}}},
  {Basic::register_t::R14, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc6}}},
  {Basic::register_t::R15, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc7}}},
};

static inline std::unordered_map<Basic::register_t, ByteArray> DecMachineCode {
  {Basic::register_t::RAX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc8}}},
  {Basic::register_t::RCX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc9}}},
  {Basic::register_t::RDX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xca}}},
  {Basic::register_t::RBX, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcb}}},
  {Basic::register_t::RSP, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcc}}},
  {Basic::register_t::RBP, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcd}}},
  {Basic::register_t::RSI, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xce}}},
  {Basic::register_t::RDI, ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcf}}},
  {Basic::register_t::R8,  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc8}}},
  {Basic::register_t::R9,  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc9}}},
  {Basic::register_t::R10, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xca}}},
  {Basic::register_t::R11, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcb}}},
  {Basic::register_t::R12, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcc}}},
  {Basic::register_t::R13, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcd}}},
  {Basic::register_t::R14, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xce}}},
  {Basic::register_t::R15, ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcf}}},
};

static inline std::unordered_map<Basic::register_t, ByteArray> DivMachineCode {
  {Basic::register_t::RAX, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf0}}},
  {Basic::register_t::RCX, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf1}}},
  {Basic::register_t::RDX, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf2}}},
  {Basic::register_t::RBX, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf3}}},
  {Basic::register_t::RSP, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf4}}},
  {Basic::register_t::RBP, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf5}}},
  {Basic::register_t::RSI, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf6}}},
  {Basic::register_t::RDI, ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf7}}},
  {Basic::register_t::R8,  ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf0}}},
  {Basic::register_t::R9,  ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf1}}},
  {Basic::register_t::R10, ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf2}}},
  {Basic::register_t::R11, ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf3}}},
  {Basic::register_t::R12, ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf4}}},
  {Basic::register_t::R13, ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf5}}},
  {Basic::register_t::R14, ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf6}}},
  {Basic::register_t::R15, ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf7}}},
};

static inline std::unordered_map<Basic::register_t, ByteArray> XorMachineCode {
  {Basic::register_t::RAX, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xc0}}},
  {Basic::register_t::RCX, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xc9}}},
  {Basic::register_t::RDX, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xd2}}},
  {Basic::register_t::RBX, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xdb}}},
  {Basic::register_t::RSP, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xe4}}},
  {Basic::register_t::RBP, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xed}}},
  {Basic::register_t::RSI, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xf6}}},
  {Basic::register_t::RDI, ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xff}}},
  {Basic::register_t::R8,  ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xc0}}},
  {Basic::register_t::R9,  ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xc9}}},
  {Basic::register_t::R10, ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xd2}}},
  {Basic::register_t::R11, ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xdb}}},
  {Basic::register_t::R12, ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xe4}}},
  {Basic::register_t::R13, ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xed}}},
  {Basic::register_t::R14, ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xf6}}},
  {Basic::register_t::R15, ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xff}}},
  {Basic::register_t::EDX, ByteArray{std::byte{0x31}, std::byte{0xd2}}},
};

static inline std::unordered_map<Basic::register_t, ByteArray> TestMachineCode {
  {Basic::register_t::RAX, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xc0}}},
  {Basic::register_t::RCX, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xc9}}},
  {Basic::register_t::RDX, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xd2}}},
  {Basic::register_t::RBX, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xdb}}},
  {Basic::register_t::RSP, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xe4}}},
  {Basic::register_t::RBP, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xed}}},
  {Basic::register_t::RSI, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xf6}}},
  {Basic::register_t::RDI, ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xff}}},
  {Basic::register_t::R8,  ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xc0}}},
  {Basic::register_t::R9,  ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xc9}}},
  {Basic::register_t::R10, ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xd2}}},
  {Basic::register_t::R11, ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xdb}}},
  {Basic::register_t::R12, ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xe4}}},
  {Basic::register_t::R13, ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xed}}},
  {Basic::register_t::R14, ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xf6}}},
  {Basic::register_t::R15, ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xff}}},
};

constexpr std::array<Basic::register_t, 16> kRegisters { // todo: put to register alloc class
  Basic::register_t::RAX, Basic::register_t::RCX, Basic::register_t::RDX, Basic::register_t::RBX,
  Basic::register_t::RSP, Basic::register_t::RBP, Basic::register_t::RSI, Basic::register_t::RDI,
  Basic::register_t::R8,  Basic::register_t::R9,  Basic::register_t::R10, Basic::register_t::R11,
  Basic::register_t::R12, Basic::register_t::R13, Basic::register_t::R14, Basic::register_t::R15,
};

constexpr auto kHalfRegisters{kRegisters.size() / 2}; // todo: put to register alloc class

struct RegisterContext {
  int source{-1};
  int destination{-1};
};

constexpr RegisterContext assignRegisters(Basic::register_t source, Basic::register_t destination) {
  RegisterContext assigned{};
  for (auto i = 0; i < kRegisters.size(); i++) {
    if (kRegisters[i] == source) assigned.source = i;
    if (kRegisters[i] == destination) assigned.destination = i;
    if (assigned.source > -1 && assigned.destination > -1) break;
  }
  return assigned;
}

void CodeGenerator::generate(const std::vector<CodeGenerator::InstructionValue> &instructions) {
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
      case Operation::CMP:
        emitCmp(instruction);
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
      case Operation::IMUL:
        emitMul(instruction);
        break;
      case Operation::IDIV:
        emitDiv(instruction);
        break;
      case Operation::XOR:
        emitXor(instruction);
        break;
      case Operation::TEST:
        emitTest(instruction);
        break;
      case Operation::RET:
        emitRet(instruction);
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
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto bytes = CodeFragment<uint32_t>::getLeaMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argOne->getValue<int>());
    return;
  }

  throw CodeGenerationError{"Unknown lea instruction"};
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionValue &instruction, bool label) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // mov reg, number
  if (target->getType() == TType::REGISTER && (argOne->isLiteralIntegerType() || argOne->getType() == TType::LIT_BOOL)) {
    const auto dataType = [&]() {
      switch (argOne->getType()) {
        case Basic::TType::LIT_INT:
        case Basic::TType::LIT_INT_U32:
          return CodeFragment<uint32_t>::type;
        default: {
          assert(0 && "Unknown data type for mov instruction");
        }
      }
    };

    using data_type = decltype(dataType());
    const auto bytes = CodeFragment<data_type>::getMovMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    if (label) {
      m_data.emplace_back(Data{m_textSection.size(), static_cast<size_t>(argOne->getValue<int>())});
    }
    const auto value = argOne->isLiteralIntegerType() ? argOne->getValue<int>()
                                                      : (argOne->getValue<bool>() ? 1 : 0);
    m_textSection.putValue<data_type>(value);
    return;
  }

  // mov reg, bool
  if (target->getType() == TType::REGISTER && (argOne->getType() == TType::KW_TRUE || argOne->getType() == TType::KW_FALSE)) {
    const auto bytes = CodeFragment<uint32_t>::getMovMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argOne->getValue<bool>() ? 1 : 0); // todo: u8 ??
    return;
  }

  // mov reg, string
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_STR) {
    auto strVal = argOne->getValue<std::string>();
    for (const auto ch : strVal) {
      m_dataSection.putBytes(std::byte(ch));
    }
    argOne->setType(TType::LIT_INT); // todo: hmm
    argOne->setValue(std::abs(static_cast<int>(m_dataSection.size() - strVal.size())));
    emitMove(instruction, true);
    return;
  }

  // mov reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<Basic::register_t>(), target->getValue<Basic::register_t>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for mov instruction");

    // <-        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x89});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x89});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x89});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x89});
    } else {
      assert(0 && "Unknown table entry for mov instruction");
    }

    auto result = 0xc0 + (8 * (src % kHalfRegisters)) + (dst % kHalfRegisters);
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown mov instruction"};
}

void CodeGenerator::emitMoveMemory(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // mov [rsi], dl
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER &&
      target->getValue<Basic::register_t>() == Basic::register_t::RSI &&
      argOne->getValue<Basic::register_t>() == Basic::register_t::DL) {
    m_textSection.putBytes(std::byte{0x88}, std::byte{0x16});
    return;
  }

  throw CodeGenerationError{"Unknown mov memory instruction"};
}

void CodeGenerator::emitSysCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0x0f}, std::byte{0x05});
}

void CodeGenerator::emitPush(const CodeGenerator::InstructionValue &instruction) {
  // push reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PushMachineCode[instruction->getArg1()->getValue<Basic::register_t>()]);
    return;
  }

  throw CodeGenerationError{"Unknown push instruction"};
}

void CodeGenerator::emitPop(const CodeGenerator::InstructionValue &instruction) {
  // pop reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PopMachineCode[instruction->getArg1()->getValue<Basic::register_t>()]);
    return;
  }

  throw CodeGenerationError{"Unknown pop instruction"};
}

void CodeGenerator::emitCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xe8});

  // call label
  const auto &label{instruction->getTarget()->getValue<std::string>()};
  const auto offset{m_textSection.size()};
  m_calls.emplace_back(Label{label, offset});

  m_textSection.putValue<uint32_t>(0);
}

void CodeGenerator::emitLabel(const CodeGenerator::InstructionValue &instruction) {
  const auto &label = instruction->getArg1()->getValue<std::string>();
  const auto offset = m_textSection.size();
  m_labels.emplace_back(Label{label, offset});
}

void CodeGenerator::emitCmp(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // cmp reg, number
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    const auto bytes = CodeFragment<uint32_t>::getCmpMachineCode(instruction->getArg1()->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argTwo->getValue<int>());
    return;
  }

  throw CodeGenerationError{"Unknown cmp instruction"};
}

void CodeGenerator::emitCmpBytePtr(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // cmp byte ptr [reg], number
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    const auto bytes = CodeFragment<uint8_t>::getCmpPtrMachineCode(instruction->getArg1()->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putBytes(std::byte(argTwo->getValue<int>()));
    return;
  }

  throw CodeGenerationError{"Unknown cmp byte ptr instruction"};
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
      default: {
        assert(0 && "Unknown jump instruction");
      }
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
    m_textSection.putBytes(IncMachineCode[instruction->getArg1()->getValue<Basic::register_t>()]);
    return;
  }

  throw CodeGenerationError{"Unknown inc instruction"};
}

void CodeGenerator::emitDec(const CodeGenerator::InstructionValue &instruction) {
  // dec reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(DecMachineCode[instruction->getArg1()->getValue<Basic::register_t>()]);
    return;
  }

  throw CodeGenerationError{"Unknown dec instruction"};
}

void CodeGenerator::emitAdd(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // add reg, number
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto dataType = [&]() {
      switch (argOne->getType()) {
        case Basic::TType::LIT_INT:
        case Basic::TType::LIT_INT_U32:
          return CodeFragment<uint32_t>::type;
        default: {
          assert(0 && "Unknown data type for add instruction");
        }
      }
    };

    using data_type = decltype(dataType());
    const auto bytes = CodeFragment<data_type>::getAddMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<data_type>(argOne->getValue<int>());
    return;
  }

  // add reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<Basic::register_t>(), target->getValue<Basic::register_t>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for add instruction");

    //  +        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x01});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x01});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x01});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x01});
    } else {
      assert(0 && "Unknown table entry for add instruction");
    }

    auto result = 0xc0 + (8 * (src % kHalfRegisters)) + (dst % kHalfRegisters);
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown add instruction"};
}

void CodeGenerator::emitSub(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // sub reg, number
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto dataType = [&]() {
      switch (argOne->getType()) {
        case Basic::TType::LIT_INT:
        case Basic::TType::LIT_INT_U32:
          return CodeFragment<uint32_t>::type;
        default: {
          assert(0 && "Unknown data type for sub instruction");
        }
      }
    };

    using data_type = decltype(dataType());
    const auto bytes = CodeFragment<data_type>::getSubMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<data_type>(argOne->getValue<int>());
    return;
  }

  // sub edx, esi
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER &&
      target->getValue<Basic::register_t>() == Basic::register_t::EDX &&
      argOne->getValue<Basic::register_t>() == Basic::register_t::ESI) {
    m_textSection.putBytes(std::byte{0x29}, std::byte{0xf2});
    return;
  }

  // sub reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<Basic::register_t>(), target->getValue<Basic::register_t>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for sub instruction");

    //  -        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x29});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x29});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x29});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x29});
    } else {
      assert(0 && "Unknown table entry for sub instruction");
    }

    auto result = 0xc0 + (8 * (src % kHalfRegisters)) + (dst % kHalfRegisters);
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown sub instruction"};
}

void CodeGenerator::emitMul(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // imul reg, number
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto dataType = [&]() {
      switch (argOne->getType()) {
        case Basic::TType::LIT_INT:
        case Basic::TType::LIT_INT_U32:
          return CodeFragment<uint32_t>::type;
        default: {
          assert(0 && "Unknown data type for imul instruction");
        }
      }
    };

    using data_type = decltype(dataType());
    const auto bytes = CodeFragment<data_type>::getMulMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<data_type>(argOne->getValue<int>());
    return;
  }

  // imul reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<Basic::register_t>(), target->getValue<Basic::register_t>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for imul instruction");

    //  *        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x0f}, std::byte{0xaf});
    } else {
      assert(0 && "Unknown table entry for imul instruction");
    }

    auto result = 0xc0 + (src % kHalfRegisters) + (8 * (dst % kHalfRegisters));
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown imul instruction"};
}

void CodeGenerator::emitDiv(const CodeGenerator::InstructionValue &instruction) {
  // div reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(DivMachineCode[instruction->getArg1()->getValue<Basic::register_t>()]);
    return;
  }

  throw CodeGenerationError{"Unknown div instruction"};
}

void CodeGenerator::emitXor(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // xor reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<Basic::register_t>() == argTwo->getValue<Basic::register_t>()) {
    m_textSection.putBytes(XorMachineCode[argOne->getValue<Basic::register_t>()]);
    return;
  }

  throw CodeGenerationError{"Unknown xor instruction"};
}

void CodeGenerator::emitTest(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // test reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<Basic::register_t>() == argTwo->getValue<Basic::register_t>()) {
    m_textSection.putBytes(TestMachineCode[argOne->getValue<Basic::register_t>()]);
    return;
  }

  throw CodeGenerationError{"Unknown test instruction"};
}

void CodeGenerator::emitRet(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xc3});
}
