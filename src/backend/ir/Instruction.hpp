#ifndef WISNIALANG_INSTRUCTION_HPP
#define WISNIALANG_INSTRUCTION_HPP

#include <memory>
#include <string>
#include <unordered_map>

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

enum class Operator {
  MOV,
  ADD,
  SUB,
  MUL,
};

static inline std::unordered_map<Operator, std::string> Operator2Str = {
  {Operator::MOV, "<-"},
  {Operator::ADD, "+"},
  {Operator::SUB, "-"},
  {Operator::MUL, "*"},
};

class Instruction {
 public:
  Instruction(
    Operator op,
    const std::shared_ptr<Basic::Token> &target,
    const std::shared_ptr<Basic::Token> &arg1,
    const std::shared_ptr<Basic::Token> &arg2
  ) : m_operator{op}, m_target{target}, m_arg1{arg1}, m_arg2{arg2} {}

  void print() const;

 private:
  Operator m_operator;
  std::shared_ptr<Basic::Token> m_target;
  std::shared_ptr<Basic::Token> m_arg1;
  std::shared_ptr<Basic::Token> m_arg2;
};

}  // namespace Wisnia

#endif  // WISNIALANG_INSTRUCTION_HPP
