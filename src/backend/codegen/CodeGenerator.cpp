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

#include <bitset>
#include <cstddef>
#include <iostream>
// Wisnia
#include "CodeGenerator.hpp"
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
  if (m_dataSection.size() > 0) std::cout << "Data section:\n" << m_dataSection.getString() << "\n";
  if (m_textSection.size() > 0) std::cout << "Text section:\n" << m_textSection.getString() << "\n";
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionValue &instruction) {
  // Moving a number to a register
  if (instruction->getTarget()->getType() == TType::REGISTER && instruction->getArg1()->getType() == TType::IDENT_INT) {
    m_textSection.putBytes(std::byte{0x48}, std::byte{0xc7}, RegisterNumber[instruction->getTarget()->getValue<std::string>()]);
    m_textSection.putU32(instruction->getArg1()->getValue<int>());
  }
}

void CodeGenerator::emitSysCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xcd});
  m_textSection.putBytes((std::byte)instruction->getArg1()->getValue<int>());
}
