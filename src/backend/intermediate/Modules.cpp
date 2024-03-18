// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

// Wisnia
#include "Modules.hpp"
#include "Exceptions.hpp"
#include "Instruction.hpp"
#include "TType.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

std::tuple<Modules::InstructionList, bool> Modules::getModule(Module module) {
  switch (module) {
    case Module::CALCULATE_STRING_LENGTH:
      return {moduleCalculateStringLength(), m_isUsed[CALCULATE_STRING_LENGTH]};
    case Module::PRINT_NUMBER:
      return {modulePrintUintNumber(), m_isUsed[PRINT_NUMBER]};
    case Module::PRINT_BOOLEAN:
      return {modulePrintBoolean(), m_isUsed[PRINT_BOOLEAN]};
    case Module::EXIT:
      return {moduleExit(), m_isUsed[EXIT]};
    default:
      throw InstructionError{"Unknown module"};
  }
}

/*
_calculate_string_length_:
  push rsi                                          ; save the string
  xor rdx, rdx                                      ; zero out the length
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
Modules::InstructionList Modules::moduleCalculateStringLength() {
  InstructionList instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::CALCULATE_STRING_LENGTH].data())
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::XOR,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx"),
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx")
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
_print_number_:
  push rax                            ;; save registers
  push rcx
  push r11
  push rsi
  push rdx

  mov rax, rdi                        ;; function argument
  mov rcx, 0xa                        ;; base 10
  mov rsi, rsp                        ;; move buffer on the stack
  sub rsp, 16                         ;; stack allocate space for a string
.print_number_loop:                   ;; do {
  xor edx, edx                        ;; zero out the temporary character
  div rcx                             ;; eax /= 10, edx %= 10
  add edx, '0'                        ;; convert to ascii
  dec rsi                             ;; working backwards from the end of the string
  mov [rsi], dl                       ;; append character
  test rax, rax                       ;; } while(x);
  jnz .print_number_loop

  mov rax, 1                          ;; write
  mov rdi, 1                          ;; stdout file descriptor
  lea edx, [rsp + 16]                 ;; length of the string
  sub edx, esi                        ;; rdx = length = end-start
  syscall

  add rsp, 16                         ;; undo the buffer reservation
  pop rdx                             ;; restore registers
  pop rsi
  pop r11
  pop rcx
  pop rax

  ret
*/
Modules::InstructionList Modules::modulePrintUintNumber() {
  InstructionList instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::PRINT_NUMBER].data())
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "r11")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx")
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_number_loop")
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
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_number_loop")
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
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "r11")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::RET
  ));

  return instructions;
}

/*
_print_boolean_:
  push rax                            ;; save registers
  push rcx
  push r11
  push rdx
  push rsi

  cmp rdi, 0                          ;; false?
  jz .print_boolean_false             ;; if true (i.e. rdi == 0 -> false), jump to false
  mov rdx, 4                          ;; else just continue
  mov rsi, "true"
  jmp .print_boolean_skip
.print_boolean_false:
  mov rdx, 5
  mov rsi, "false"
.print_boolean_skip:
  mov rax, 1                          ;; write
  mov rdi, 1                          ;; stdout file descriptor
  syscall

  pop rsi                             ;; restore registers
  pop rdx
  pop r11
  pop rcx
  pop rax
  ret
 */
Modules::InstructionList Modules::modulePrintBoolean() {
  InstructionList instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::PRINT_BOOLEAN].data())
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "r11")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::CMP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdi"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 0)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JZ,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_boolean_false")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 4)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
    std::make_shared<Basic::Token>(TType::LIT_STR, "true")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JMP,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_boolean_skip")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_boolean_false")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 5)
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
    std::make_shared<Basic::Token>(TType::LIT_STR, "false")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, ".print_boolean_skip")
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
    Operation::SYSCALL
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rsi")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "r11")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rcx")
  ));
  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::POP,
    nullptr,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax")
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
Modules::InstructionList Modules::moduleExit() {
  InstructionList instructions{};

  instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::EXIT].data())
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
