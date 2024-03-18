// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

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
  DEC,            // decrement
  /* comparison (each for int and float) */
  IEQ,   FEQ,     // equal
  ILT,   FLT,     // less than
  ILE,   FLE,     // less equal
  IGT,   FGT,     // greater than
  IGE,   FGE,     // greater equal
  INE,   FNE,     // not equal
  CMP,            // compare register against a number
  CMP_BYTE_PTR,   // compare address of a single-byte memory location
  /* logical */
  NOT,
  AND,
  OR,
  XOR,
  TEST,
  /* jumps */
  JMP,            // unconditional jump
  JE,             // jump equal
  JZ,             // jump zero
  JNE,            // jump not equal
  JNZ,            // jump not zero
  /* miscellaneous */
  LEA,
  MOV,            // copies the value from rhs register to lhs register
  MOV_MEMORY,     // copies the value from rhs register to memory address contained in lhs
  PUSH,           // push value on the stack
  POP,            // pop value from the stack
  CALL,           // function invocation
  SYSCALL,        // system call
  LABEL,          // label
  RET,            // function return
  NOP             // do nothing
};

static inline std::unordered_map<Operation, std::string_view> Operation2Str {
  // arithmetic (each for int and float)
  {Operation::IADD, "+" }, {Operation::FADD, "+"},
  {Operation::ISUB, "-" }, {Operation::FSUB, "-"},
  {Operation::IMUL, "*" }, {Operation::FMUL, "*"},
  {Operation::IDIV, "/" }, {Operation::FDIV, "/"},
  {Operation::INC,  "++"}, {Operation::DEC, "--"},
  // comparison (each for int and float)
  {Operation::IEQ, "==" }, {Operation::FEQ, "=="},
  {Operation::ILT, "<"  }, {Operation::FLT, "<" },
  {Operation::ILE, "<=" }, {Operation::FLE, "<="},
  {Operation::IGT, ">"  }, {Operation::FGT, ">" },
  {Operation::IGE, ">=" }, {Operation::FGE, ">="},
  {Operation::INE, "!=" }, {Operation::FNE, "!="},
  {Operation::CMP, "cmp"},
  {Operation::CMP_BYTE_PTR, "cmp byte ptr"      },
  // logical
  {Operation::NOT,  "!"   },
  {Operation::AND,  "&&"  },
  {Operation::OR,   "||"  },
  {Operation::XOR,  "xor" },
  {Operation::TEST, "test"},
  // jumps
  {Operation::JMP, "jmp"},
  {Operation::JE,  "je" },
  {Operation::JZ,  "jz" },
  {Operation::JNE, "jne"},
  {Operation::JNZ, "jnz"},
  // miscellaneous
  {Operation::LEA,        "lea"     },
  {Operation::MOV,        "<-"      },
  {Operation::MOV_MEMORY, "[] <-"   },
  {Operation::PUSH,       "push"    },
  {Operation::POP,        "pop"     },
  {Operation::CALL,       "call"    },
  {Operation::SYSCALL,    "syscall" },
  {Operation::LABEL,      "label"   },
  {Operation::RET,        "ret"     },
  {Operation::NOP,        "nop"     },
};

}  // namespace Wisnia

#endif  // WISNIALANG_OPERATION_HPP
