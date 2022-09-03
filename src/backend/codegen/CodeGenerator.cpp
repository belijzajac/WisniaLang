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

#include <iostream>
// Wisnia
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

static inline std::unordered_map<std::string, std::byte> RegisterNumber {
  {"rax", std::byte{0xc0}},
  {"rcx", std::byte{0xc1}},
  {"rdx", std::byte{0xc2}},
  {"rbx", std::byte{0xc3}},
  {"rsp", std::byte{0xc4}},
  {"rbp", std::byte{0xc5}},
  {"rsi", std::byte{0xc6}},
  {"rdi", std::byte{0xc7}}
};

void CodeGenerator::generateCode(const std::vector<CodeGenerator::InstructionValue> &instructions) {
  for (const auto &instruction : instructions) {
    switch (instruction->getOperation()) {
      case Operation::MOV:
        emitMove(instruction);
        break;
      case Operation::SYSCALL:
        emitSysCall(instruction);
        break;
    }
  }

  // Patch data
  for (const auto &patch : m_patches) {
    auto start{patch.start};
    auto offset{patch.offset};

    auto newAddress{kVirtualStartAddress + offset + m_textSection.size() + kTextOffset};
    ByteArray bytes{};
    bytes.putU32(newAddress);

    for (size_t i = 0; i < bytes.size(); i++) {
      m_textSection.insert(i + start, bytes.data()[i]);
    }
  }

  if (m_dataSection.size() > 0) std::cout << "<~~~ data section ~~~>\n" << m_dataSection.getString() << "\n";
  if (m_textSection.size() > 0) std::cout << "<~~~ text section ~~~>\n" << m_textSection.getString() << "\n";
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionValue &instruction, bool label) {
  // Moving a number to a register
  if (instruction->getTarget()->getType() == TType::REGISTER && instruction->getArg1()->getType() == TType::IDENT_INT) {
    m_textSection.putBytes(std::byte{0x48}, std::byte{0xc7}, RegisterNumber[instruction->getTarget()->getValue<std::string>()]);
    if (label) {
      m_patches.emplace_back(Patch{m_textSection.size(), (size_t)instruction->getArg1()->getValue<int>()});
    }
    m_textSection.putU32(instruction->getArg1()->getValue<int>());
  }
  // Moving a string to a register
  if (instruction->getTarget()->getType() == TType::REGISTER && instruction->getArg1()->getType() == TType::IDENT_STRING) {
    auto strVal = instruction->getArg1()->getValue<std::string>();
    for (const auto ch : strVal) {
      m_dataSection.putBytes(std::byte(ch));
    }
    instruction->getArg1()->setType(TType::IDENT_INT);
    instruction->getArg1()->setValue(std::abs(static_cast<int>(m_dataSection.size() - strVal.size())));
    emitMove(instruction, true);
  }
}

void CodeGenerator::emitSysCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xcd});
  m_textSection.putBytes((std::byte)instruction->getArg1()->getValue<int>());
}
