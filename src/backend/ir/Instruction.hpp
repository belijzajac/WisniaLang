#ifndef WISNIALANG_INSTRUCTION_HPP
#define WISNIALANG_INSTRUCTION_HPP

#include <memory>
#include <string>
#include <unordered_map>

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

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
  MOV,  // move to/from register
  JMP,  // unconditional jump
  BR,   // conditional branch
  CALL, // function invocation
  RET,  // function return
  PRNT, // output values to the console
  NOP   // do nothing
};

static inline std::unordered_map<Operation, std::string> Operator2Str {
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
  {Operation::MOV,  "<-"   },
  {Operation::JMP,  "jmp"  },
  {Operation::BR,   "br"   },
  {Operation::CALL, "call" },
  {Operation::RET,  "ret"  },
  {Operation::PRNT, "print"},
  {Operation::NOP,  "nop"  },
};

class Instruction {
 public:
  Instruction(
    Operation op,
    const std::shared_ptr<Basic::Token> &target,
    const std::shared_ptr<Basic::Token> &arg1,
    const std::shared_ptr<Basic::Token> &arg2 = nullptr
  ) : m_operator{op}, m_target{target}, m_arg1{arg1}, m_arg2{arg2} {}

  void print() const;

 private:
  Operation m_operator;
  std::shared_ptr<Basic::Token> m_target;
  std::shared_ptr<Basic::Token> m_arg1;
  std::shared_ptr<Basic::Token> m_arg2;
};

}  // namespace Wisnia

#endif  // WISNIALANG_INSTRUCTION_HPP
