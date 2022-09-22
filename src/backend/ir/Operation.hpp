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

#ifndef WISNIALANG_OPERATION_HPP
#define WISNIALANG_OPERATION_HPP

#include <string>
#include <unordered_map>

namespace Wisnia {

enum class Operation {
  /* arithmetic (each for int and float) */
  IADD,  FADD,    // add
  ISUB,  FSUB,    // subtract
  IMUL,  FMUL,    // multiply
  IDIV,  FDIV,    // divide
  INC,            // increment
  /* comparison (each for int and float) */
  IEQ,   FEQ,     // equal
  ILT,   FLT,     // less than
  ILE,   FLE,     // less equal
  IGT,   FGT,     // greater than
  IGE,   FGE,     // greater equal
  INE,   FNE,     // not equal
  CMP_BYTE_ADDR,  // compare address to a single-byte memory location
  /* logical */
  NOT,
  AND,
  OR,
  /* jumps */
  JMP,            // unconditional jump
  JE,             // jump equal
  /* miscellaneous */
  MOV,            // move to/from register
  PUSH,           // push value on the stack
  POP,            // pop value from the stack
  CALL,           // function invocation
  SYSCALL,        // system call
  LABEL,          // label
  RET,            // function return
  NOP             // do nothing
};

static inline std::unordered_map<Operation, std::string> Operation2Str {
  // arithmetic (each for int and float)
  {Operation::IADD, "+" }, {Operation::FADD, "+"},
  {Operation::ISUB, "-" }, {Operation::FSUB, "-"},
  {Operation::IMUL, "*" }, {Operation::FMUL, "*"},
  {Operation::IDIV, "/" }, {Operation::FDIV, "/"},
  {Operation::INC,  "++"},
  // comparison (each for int and float)
  {Operation::IEQ, "=="}, {Operation::FEQ, "=="},
  {Operation::ILT, "<" }, {Operation::FLT, "<" },
  {Operation::ILE, "<="}, {Operation::FLE, "<="},
  {Operation::IGT, ">" }, {Operation::FGT, ">" },
  {Operation::IGE, ">="}, {Operation::FGE, ">="},
  {Operation::INE, "!="}, {Operation::FNE, "!="},
  {Operation::CMP_BYTE_ADDR, "cmp byte ptr"    },
  // logical
  {Operation::NOT, "!" },
  {Operation::AND, "&&"},
  {Operation::OR,  "||"},
  // jumps
  {Operation::JMP, "jmp"},
  {Operation::JE,  "je" },
  // miscellaneous
  {Operation::MOV,     "<-"      },
  {Operation::PUSH,    "push"    },
  {Operation::POP,     "pop"     },
  {Operation::CALL,    "call"    },
  {Operation::SYSCALL, "syscall" },
  {Operation::RET,     "ret"     },
  {Operation::NOP,     "nop"     },
};

}  // namespace Wisnia

#endif  // WISNIALANG_OPERATION_HPP
