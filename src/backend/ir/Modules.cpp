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
    case Module::EXIT:
      return moduleExit();
    default:
      throw InstructionError{"Unknown module"};
  }
}

/*
_calculate_string_length_:
  mov rdx, 0x00             ; zero the length
  push rsi                  ; save the string
_loop_:
  cmp byte ptr [rsi], 0x00  ; is it null-terminated yet?
  je _exit_loop_            ; if it is, break the loop
  inc rdx                   ; increment length
  inc rsi                   ; look at the next character
  jmp _loop_                ; loop again
_exit_loop_:
  pop rsi                   ; restore the string
  ret                       ; return
*/
Modules::instructions_list Modules::moduleCalculateStringLength() {
  instructions_list instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::CALCULATE_STRING_LENGTH])
  ));
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "_loop_")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::CMP_BYTE_PTR,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 0)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JE,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "_exit_loop_")
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "_loop_")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, "_exit_loop_")
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

/*
_exit_:
  mov rax, 0x3c      ;; exit
  mov rdi, 0x00      ;; exit code is 0
  syscall            ;; make the system call
*/
Modules::instructions_list Modules::moduleExit() {
  instructions_list instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::EXIT])
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdi"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 0)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 60)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::SYSCALL
  ));

  return instructions;
}
