// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <algorithm>
#include <cassert>
#include <iostream>
// Wisnia
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "Instruction.hpp"
#include "MachineCodeTable.hpp"
#include "RegisterAllocator.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

struct RegisterContext {
  int source{-1};
  int destination{-1};
};

constexpr RegisterContext assignRegisters(Basic::register_t source, Basic::register_t destination) {
  RegisterContext assigned{};
  for (auto i = 0; i < RegisterAllocator::getFullRegisters(); i++) {
    if (RegisterAllocator::getAllRegisters[i] == source) assigned.source = i;
    if (RegisterAllocator::getAllRegisters[i] == destination) assigned.destination = i;
    if (assigned.source > -1 && assigned.destination > -1) break;
  }
  return assigned;
}

void CodeGenerator::generate(const std::vector<CodeGenerator::InstructionPtr> &instructions) {
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
        emitSysCall();
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
        emitRet();
        break;
      default: {
        assert(0 && "Unknown operation to generate the code for");
      }
    }
  }

  // Patch data
  for (const auto &data : m_data) {
    const auto start{data.m_start};
    const auto offset{data.m_offset};

    const auto address{static_cast<uint32_t>(kVirtualStartAddress + offset + m_textSection.size() + kTextOffset)};
    const ByteArray bytes{address};

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

void CodeGenerator::emitLea(const CodeGenerator::InstructionPtr &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // lea reg, [rsp + number]
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto bytes = MachineCodeTable<uint32_t>::getLeaMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argOne->getValue<int>());
    return;
  }

  throw CodeGenerationError{"Unknown lea instruction"};
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionPtr &instruction, bool label) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // mov reg, number
  if (target->getType() == TType::REGISTER && (argOne->isLiteralIntegerType() || argOne->getType() == TType::LIT_BOOL)) {
    const auto bytes = MachineCodeTable<uint32_t>::getMovMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    if (label) {
      m_data.emplace_back(Data{m_textSection.size(), static_cast<size_t>(argOne->getValue<int>())});
    }
    const auto value = argOne->isLiteralIntegerType() ? argOne->getValue<int>() : (argOne->getValue<bool>() ? 1 : 0);
    m_textSection.putValue<uint32_t>(value);
    return;
  }

  // mov reg, bool
  if (target->getType() == TType::REGISTER && (argOne->getType() == TType::KW_TRUE || argOne->getType() == TType::KW_FALSE)) {
    const auto bytes = MachineCodeTable<uint32_t>::getMovMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argOne->getValue<bool>() ? 1 : 0);
    return;
  }

  // mov reg, string
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
    const auto [src, dst] = assignRegisters(argOne->getValue<Basic::register_t>(), target->getValue<Basic::register_t>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for mov instruction");

    // <-        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x89});
    } else if (dst < RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x89});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x89});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x89});
    } else {
      assert(0 && "Unknown table entry for mov instruction");
    }

    const auto result =
        0xc0 + (RegisterAllocator::getHalfRegisters() * (src % RegisterAllocator::getHalfRegisters())) +
        (dst % RegisterAllocator::getHalfRegisters());
    assert(result <= 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown mov instruction"};
}

void CodeGenerator::emitMoveMemory(const CodeGenerator::InstructionPtr &instruction) {
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

void CodeGenerator::emitSysCall() {
  m_textSection.putBytes(std::byte{0x0f}, std::byte{0x05});
}

void CodeGenerator::emitPush(const CodeGenerator::InstructionPtr &instruction) {
  // push reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {

    const auto reg = instruction->getArg1()->getValue<Basic::register_t>();
    const auto pushMachineCode = [&reg]() -> ByteArray {
      switch (reg) {
        case Basic::register_t::RAX: return {std::byte{0x50}};
        case Basic::register_t::RCX: return {std::byte{0x51}};
        case Basic::register_t::RDX: return {std::byte{0x52}};
        case Basic::register_t::RBX: return {std::byte{0x53}};
        case Basic::register_t::RSP: return {std::byte{0x54}};
        case Basic::register_t::RBP: return {std::byte{0x55}};
        case Basic::register_t::RSI: return {std::byte{0x56}};
        case Basic::register_t::RDI: return {std::byte{0x57}};
        case Basic::register_t::R8:  return {std::byte{0x41}, std::byte{0x50}};
        case Basic::register_t::R9:  return {std::byte{0x41}, std::byte{0x51}};
        case Basic::register_t::R10: return {std::byte{0x41}, std::byte{0x52}};
        case Basic::register_t::R11: return {std::byte{0x41}, std::byte{0x53}};
        case Basic::register_t::R12: return {std::byte{0x41}, std::byte{0x54}};
        case Basic::register_t::R13: return {std::byte{0x41}, std::byte{0x55}};
        case Basic::register_t::R14: return {std::byte{0x41}, std::byte{0x56}};
        case Basic::register_t::R15: return {std::byte{0x41}, std::byte{0x57}};
        default: { assert(0 && "Unknown register for push instruction"); }
      }
    };

    m_textSection.putBytes(pushMachineCode());
    return;
  }

  throw CodeGenerationError{"Unknown push instruction"};
}

void CodeGenerator::emitPop(const CodeGenerator::InstructionPtr &instruction) {
  // pop reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {

    const auto reg = instruction->getArg1()->getValue<Basic::register_t>();
    const auto popMachineCode = [&reg]() -> ByteArray {
      switch (reg) {
        case Basic::register_t::RAX: return {std::byte{0x58}};
        case Basic::register_t::RCX: return {std::byte{0x59}};
        case Basic::register_t::RDX: return {std::byte{0x5a}};
        case Basic::register_t::RBX: return {std::byte{0x5b}};
        case Basic::register_t::RSP: return {std::byte{0x5c}};
        case Basic::register_t::RBP: return {std::byte{0x5d}};
        case Basic::register_t::RSI: return {std::byte{0x5e}};
        case Basic::register_t::RDI: return {std::byte{0x5f}};
        case Basic::register_t::R8:  return {std::byte{0x41}, std::byte{0x58}};
        case Basic::register_t::R9:  return {std::byte{0x41}, std::byte{0x59}};
        case Basic::register_t::R10: return {std::byte{0x41}, std::byte{0x5a}};
        case Basic::register_t::R11: return {std::byte{0x41}, std::byte{0x5b}};
        case Basic::register_t::R12: return {std::byte{0x41}, std::byte{0x5c}};
        case Basic::register_t::R13: return {std::byte{0x41}, std::byte{0x5d}};
        case Basic::register_t::R14: return {std::byte{0x41}, std::byte{0x5e}};
        case Basic::register_t::R15: return {std::byte{0x41}, std::byte{0x5f}};
        default: { assert(0 && "Unknown register for pop instruction"); }
      }
    };

    m_textSection.putBytes(popMachineCode());
    return;
  }

  throw CodeGenerationError{"Unknown pop instruction"};
}

void CodeGenerator::emitCall(const CodeGenerator::InstructionPtr &instruction) {
  m_textSection.putBytes(std::byte{0xe8});

  // call label
  const auto &label{instruction->getTarget()->getValue<std::string>()};
  const auto offset{m_textSection.size()};
  m_calls.emplace_back(Label{label, offset});

  m_textSection.putValue<uint32_t>(0);
}

void CodeGenerator::emitLabel(const CodeGenerator::InstructionPtr &instruction) {
  const auto &label = instruction->getArg1()->getValue<std::string>();
  const auto offset = m_textSection.size();
  m_labels.emplace_back(Label{label, offset});
}

void CodeGenerator::emitCmp(const CodeGenerator::InstructionPtr &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // cmp reg, number
  if (argOne->getType() == TType::REGISTER) {
    const auto bytes = MachineCodeTable<uint32_t>::getCmpMachineCode(argOne->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argTwo->getValue<int>());
    return;
  }

  throw CodeGenerationError{"Unknown cmp instruction"};
}

void CodeGenerator::emitCmpBytePtr(const CodeGenerator::InstructionPtr &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // cmp byte ptr [reg], number
  if (argOne->getType() == TType::REGISTER) {
    const auto bytes = MachineCodeTable<uint8_t>::getCmpPtrMachineCode(argOne->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putBytes(std::byte(argTwo->getValue<int>()));
    return;
  }

  throw CodeGenerationError{"Unknown cmp byte ptr instruction"};
}

void CodeGenerator::emitJmp(const CodeGenerator::InstructionPtr &instruction) {
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

void CodeGenerator::emitInc(const CodeGenerator::InstructionPtr &instruction) {
  // inc reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {

    const auto reg = instruction->getArg1()->getValue<Basic::register_t>();
    const auto incMachineCode = [&reg]() -> ByteArray {
      switch (reg) {
        case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc0}};
        case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc1}};
        case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc2}};
        case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc3}};
        case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc4}};
        case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc5}};
        case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc6}};
        case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc7}};
        case Basic::register_t::R8:  return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc0}};
        case Basic::register_t::R9:  return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc1}};
        case Basic::register_t::R10: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc2}};
        case Basic::register_t::R11: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc3}};
        case Basic::register_t::R12: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc4}};
        case Basic::register_t::R13: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc5}};
        case Basic::register_t::R14: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc6}};
        case Basic::register_t::R15: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc7}};
        default: { assert(0 && "Unknown register for inc instruction"); }
      }
    };

    m_textSection.putBytes(incMachineCode());
    return;
  }

  throw CodeGenerationError{"Unknown inc instruction"};
}

void CodeGenerator::emitDec(const CodeGenerator::InstructionPtr &instruction) {
  // dec reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {

    const auto reg = instruction->getArg1()->getValue<Basic::register_t>();
    const auto decMachineCode = [&reg]() -> ByteArray {
      switch (reg) {
        case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc8}};
        case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xc9}};
        case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xca}};
        case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xcb}};
        case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xcc}};
        case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xcd}};
        case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xce}};
        case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0xff}, std::byte{0xcf}};
        case Basic::register_t::R8:  return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc8}};
        case Basic::register_t::R9:  return {std::byte{0x49}, std::byte{0xff}, std::byte{0xc9}};
        case Basic::register_t::R10: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xca}};
        case Basic::register_t::R11: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xcb}};
        case Basic::register_t::R12: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xcc}};
        case Basic::register_t::R13: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xcd}};
        case Basic::register_t::R14: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xce}};
        case Basic::register_t::R15: return {std::byte{0x49}, std::byte{0xff}, std::byte{0xcf}};
        default: { assert(0 && "Unknown register for dec instruction"); }
      }
    };

    m_textSection.putBytes(decMachineCode());
    return;
  }

  throw CodeGenerationError{"Unknown dec instruction"};
}

void CodeGenerator::emitAdd(const CodeGenerator::InstructionPtr &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // add reg, number
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto bytes = MachineCodeTable<uint32_t>::getAddMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argOne->getValue<int>());
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
    if (dst < RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x01});
    } else if (dst < RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x01});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x01});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x01});
    } else {
      assert(0 && "Unknown table entry for add instruction");
    }

    const auto result =
        0xc0 + (RegisterAllocator::getHalfRegisters() * (src % RegisterAllocator::getHalfRegisters())) +
        (dst % RegisterAllocator::getHalfRegisters());
    assert(result <= 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown add instruction"};
}

void CodeGenerator::emitSub(const CodeGenerator::InstructionPtr &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // sub reg, number
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto bytes = MachineCodeTable<uint32_t>::getSubMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argOne->getValue<int>());
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
    if (dst < RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x29});
    } else if (dst < RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x29});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x29});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x29});
    } else {
      assert(0 && "Unknown table entry for sub instruction");
    }

    const auto result =
        0xc0 + (RegisterAllocator::getHalfRegisters() * (src % RegisterAllocator::getHalfRegisters())) +
        (dst % RegisterAllocator::getHalfRegisters());
    assert(result <= 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown sub instruction"};
}

void CodeGenerator::emitMul(const CodeGenerator::InstructionPtr &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // imul reg, number
  if (target->getType() == TType::REGISTER && argOne->isLiteralIntegerType()) {
    const auto bytes = MachineCodeTable<uint32_t>::getMulMachineCode(target->getValue<Basic::register_t>());
    m_textSection.putBytes(bytes);
    m_textSection.putValue<uint32_t>(argOne->getValue<int>());
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
    if (dst < RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst < RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // top right
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src < RegisterAllocator::getHalfRegisters()) {
      // bottom left
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst >= RegisterAllocator::getHalfRegisters() && src >= RegisterAllocator::getHalfRegisters()) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x0f}, std::byte{0xaf});
    } else {
      assert(0 && "Unknown table entry for imul instruction");
    }

    const auto result =
        0xc0 + (src % RegisterAllocator::getHalfRegisters()) +
        (RegisterAllocator::getHalfRegisters() * (dst % RegisterAllocator::getHalfRegisters()));
    assert(result <= 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown imul instruction"};
}

void CodeGenerator::emitDiv(const CodeGenerator::InstructionPtr &instruction) {
  // div reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {

    const auto reg = instruction->getArg1()->getValue<Basic::register_t>();
    const auto divMachineCode = [&reg]() -> ByteArray {
      switch (reg) {
        case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf0}};
        case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf1}};
        case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf2}};
        case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf3}};
        case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf4}};
        case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf5}};
        case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf6}};
        case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0xf7}, std::byte{0xf7}};
        case Basic::register_t::R8:  return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf0}};
        case Basic::register_t::R9:  return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf1}};
        case Basic::register_t::R10: return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf2}};
        case Basic::register_t::R11: return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf3}};
        case Basic::register_t::R12: return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf4}};
        case Basic::register_t::R13: return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf5}};
        case Basic::register_t::R14: return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf6}};
        case Basic::register_t::R15: return {std::byte{0x49}, std::byte{0xf7}, std::byte{0xf7}};
        default: { assert(0 && "Unknown register for div instruction"); }
      }
    };

    m_textSection.putBytes(divMachineCode());
    return;
  }

  throw CodeGenerationError{"Unknown div instruction"};
}

void CodeGenerator::emitXor(const CodeGenerator::InstructionPtr &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // xor reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<Basic::register_t>() == argTwo->getValue<Basic::register_t>()) {

    const auto reg = argOne->getValue<Basic::register_t>();
    const auto xorMachineCode = [&reg]() -> ByteArray {
      switch (reg) {
        case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xc0}};
        case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xc9}};
        case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xd2}};
        case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xdb}};
        case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xe4}};
        case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xed}};
        case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xf6}};
        case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0x31}, std::byte{0xff}};
        case Basic::register_t::R8:  return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xc0}};
        case Basic::register_t::R9:  return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xc9}};
        case Basic::register_t::R10: return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xd2}};
        case Basic::register_t::R11: return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xdb}};
        case Basic::register_t::R12: return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xe4}};
        case Basic::register_t::R13: return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xed}};
        case Basic::register_t::R14: return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xf6}};
        case Basic::register_t::R15: return {std::byte{0x4d}, std::byte{0x31}, std::byte{0xff}};
        case Basic::register_t::EDX: return {std::byte{0x31}, std::byte{0xd2}};
        default: { assert(0 && "Unknown register for xor instruction"); }
      }
    };

    m_textSection.putBytes(xorMachineCode());
    return;
  }

  throw CodeGenerationError{"Unknown xor instruction"};
}

void CodeGenerator::emitTest(const CodeGenerator::InstructionPtr &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // test reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<Basic::register_t>() == argTwo->getValue<Basic::register_t>()) {

    const auto reg = argOne->getValue<Basic::register_t>();
    const auto testMachineCode = [&reg]() -> ByteArray {
      switch (reg) {
        case Basic::register_t::RAX: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xc0}};
        case Basic::register_t::RCX: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xc9}};
        case Basic::register_t::RDX: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xd2}};
        case Basic::register_t::RBX: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xdb}};
        case Basic::register_t::RSP: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xe4}};
        case Basic::register_t::RBP: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xed}};
        case Basic::register_t::RSI: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xf6}};
        case Basic::register_t::RDI: return {std::byte{0x48}, std::byte{0x85}, std::byte{0xff}};
        case Basic::register_t::R8:  return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xc0}};
        case Basic::register_t::R9:  return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xc9}};
        case Basic::register_t::R10: return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xd2}};
        case Basic::register_t::R11: return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xdb}};
        case Basic::register_t::R12: return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xe4}};
        case Basic::register_t::R13: return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xed}};
        case Basic::register_t::R14: return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xf6}};
        case Basic::register_t::R15: return {std::byte{0x4d}, std::byte{0x85}, std::byte{0xff}};
        default: { assert(0 && "Unknown register for test instruction"); }
      }
    };

    m_textSection.putBytes(testMachineCode());
    return;
  }

  throw CodeGenerationError{"Unknown test instruction"};
}

void CodeGenerator::emitRet() {
  m_textSection.putBytes(std::byte{0xc3});
}
