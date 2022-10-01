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
    case Module::UINT_TO_STRING:
      return moduleUintToString();
    case Module::EXIT:
      return moduleExit();
    default:
      throw InstructionError{"Unknown module"};
  }
}

/*
_calculate_string_length_:
  xor rdx, rdx                                      ; zero the length
  push rsi                                          ; save the string
.calculate_string_length_loop:
  cmp byte ptr [rsi], 0x00                          ; is it null-terminated yet?
  je .calculate_string_length_exit_loop             ; if it is, break the loop
  inc rdx                                           ; increment length
  inc rsi                                           ; look at the next character
  jmp .calculate_string_length_loop                 ; loop again
.calculate_string_length_exit_loop:
  pop rsi                                           ; restore the string
  ret                                               ; return
*/
Modules::instructions_list Modules::moduleCalculateStringLength() {
  instructions_list instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::CALCULATE_STRING_LENGTH])
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::XOR,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx"),
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".calculate_string_length_loop")
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".calculate_string_length_exit_loop")
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".calculate_string_length_loop")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".calculate_string_length_exit_loop")
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
_uint_to_string_:
  mov rax, rsi
  mov rsi, rsp       ;; move buffer on the stack
  sub rsp, 40        ;; stack allocate space for a string
  dec rsi
  mov r8, 10
.uint_to_string_loop:
  xor rdx, rdx
  div r8
  or dl, 0x30
  dec rsi
  mov [rsi], dl
  test rax, rax
  jnz .uint_to_string_loop
  ret
*/
Modules::instructions_list Modules::moduleUintToString() {
  instructions_list instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::UINT_TO_STRING])
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rbx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax"),
    std::make_shared<Basic::Token>(TType::REGISTER, "rdi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 10)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
    std::make_shared<Basic::Token>(TType::REGISTER, "rsp")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::ISUB,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsp"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 16)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".uint_to_string_loop")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::XOR,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "edx"),
    std::make_shared<Basic::Token>(TType::REGISTER, "edx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::IDIV,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::IADD,
    std::make_shared<Basic::Token>(TType::REGISTER, "edx"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 48) // '0'
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::DEC,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV_MEMORY,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
    std::make_shared<Basic::Token>(TType::REGISTER, "dl")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::TEST,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax"),
    std::make_shared<Basic::Token>(TType::REGISTER, "rax")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JNZ,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".uint_to_string_loop")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 1)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdi"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 1)
  ));
  //lea edx, [rsp + 16]
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LEA
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::ISUB,
    std::make_shared<Basic::Token>(TType::REGISTER, "edx"),
    std::make_shared<Basic::Token>(TType::REGISTER, "esi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::SYSCALL
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::IADD,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsp"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 24 - 8)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rbx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::RET
  ));

  return instructions;
}

/*
_exit_:
  mov rax, 0x3c      ;; exit
  xor rdi, rdi       ;; exit code is 0
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
    Operation::XOR,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdi"),
    std::make_shared<Basic::Token>(TType::REGISTER, "rdi")
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
