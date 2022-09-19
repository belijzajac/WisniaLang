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
  // arithmetic (each for int and float)
  IADD,  FADD,
  ISUB,  FSUB,
  IMUL,  FMUL,
  IDIV,  FDIV,
  // comparison (each for int and float)
  IEQ,   FEQ,
  ILT,   FLT,
  ILE,   FLE,
  IGT,   FGT,
  IGE,   FGE,
  INE,   FNE,
  // logical
  NOT,
  AND,
  OR,
  // miscellaneous
  MOV,     // move to/from register
  PUSH,    // push value on the stack
  POP,     // pop value from the stack
  JMP,     // unconditional jump
  BR,      // conditional branch
  CALL,    // function invocation
  SYSCALL, // system call
  RET,     // function return
  NOP      // do nothing
};

static inline std::unordered_map<Operation, std::string> Operation2Str {
  // arithmetic (each for int and float)
  {Operation::IADD, "+"}, {Operation::FADD, "+"},
  {Operation::ISUB, "-"}, {Operation::FSUB, "-"},
  {Operation::IMUL, "*"}, {Operation::FMUL, "*"},
  {Operation::IDIV, "/"}, {Operation::FDIV, "/"},
  // comparison (each for int and float)
  {Operation::IEQ, "=="}, {Operation::FEQ, "=="},
  {Operation::ILT, "<" }, {Operation::FLT, "<" },
  {Operation::ILE, "<="}, {Operation::FLE, "<="},
  {Operation::IGT, ">" }, {Operation::FGT, ">" },
  {Operation::IGE, ">="}, {Operation::FGE, ">="},
  {Operation::INE, "!="}, {Operation::FNE, "!="},
  // logical
  {Operation::NOT, "!" },
  {Operation::AND, "&&"},
  {Operation::OR,  "||"},
  // miscellaneous
  {Operation::MOV,     "<-"      },
  {Operation::PUSH,    "PUSH"    },
  {Operation::POP,     "POP"     },
  {Operation::JMP,     "jmp"     },
  {Operation::BR,      "br"      },
  {Operation::CALL,    "call"    },
  {Operation::SYSCALL, "syscall" },
  {Operation::RET,     "ret"     },
  {Operation::NOP,     "nop"     },
};

}  // namespace Wisnia

#endif  // WISNIALANG_OPERATION_HPP
