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
    case Module::PRINT_UINT_NUMBER:
      return modulePrintUintNumber();
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
_print_uint_number_:
  push rcx                            ;; save register
  push rbx                            ;; save register
  mov rax, rdi                        ;; function argument
  mov rcx, 0xa                        ;; base 10
  mov rsi, rsp                        ;; move buffer on the stack
  sub rsp, 16                         ;; stack allocate space for a string
.print_uint_number_loop:              ;; do {
  xor edx, edx
  div rcx                             ;; eax /= 10, edx %= 10
  add edx, '0'                        ;; convert to ascii
  dec rsi                             ;; working backwards from the end of the string
  mov [rsi], dl                       ;; append character
  test rax, rax                       ;; } while(x);
  jnz .print_uint_number_loop

  mov rax, 1                          ;; write
  mov rdi, 1                          ;; stdout file descriptor
  lea edx, [rsp + 16]                 ;; length of the string
  sub edx, esi                        ;; rdx = length = end-start
  syscall

  add rsp, 16                         ;; undo the buffer reservation
  pop rbx                             ;; restore register
  pop rcx                             ;; restore register

  ret
*/
Modules::instructions_list Modules::modulePrintUintNumber() {
  instructions_list instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::PRINT_UINT_NUMBER])
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_uint_number_loop")
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_uint_number_loop")
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
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LEA,
    std::make_shared<Basic::Token>(TType::REGISTER, "edx"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 16)
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
    std::make_shared<Basic::Token>(TType::LIT_INT, 16)
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
  xor rdi, rdi       ;; exit code is 0
  mov rax, 0x3c      ;; exit
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
