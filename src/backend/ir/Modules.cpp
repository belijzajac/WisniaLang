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

// Wisnia
#include "Modules.hpp"
#include "Exceptions.hpp"
#include "Instruction.hpp"
#include "TType.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

Modules::instructions_list Modules::getModule(Module module) {
  switch (module) {
    case Module::CALCULATE_STRING_LENGTH:
      return moduleCalculateStringLength();
    default:
      throw InstructionError{"Unknown module"};
  }
}

/*
  mov rdx, 0x00             ; zero the length
  push rsi                  ; save the string
len_loop:
  cmp byte ptr [rsi], 0x00  ; is it null-terminated yet?
  je len_loop_over          ; if it is, break the loop
  inc rdx                   ; increment length
  inc rsi                   ; look at the next character
  jmp len_loop              ; loop again
len_loop_over:
  pop rsi                   ; restore the string
  ret                       ; return
*/
Modules::instructions_list Modules::moduleCalculateStringLength() {
  instructions_list instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 0)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "len_loop")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::CMP_BYTE_ADDR,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 0)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JE,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "len_loop_over")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::INC,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::INC,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JMP,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "len_loop")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "len_loop_over")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::RET
  ));

  return instructions;
}
