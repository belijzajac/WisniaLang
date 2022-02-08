#include <fmt/format.h>
// Wisnia
#include "Instruction.hpp"
#include "Token.hpp"

void Wisnia::Instruction::print() const {
  fmt::print("{:^17}|{:^5}|{:^17}|{:^17}\n",
    m_target->getASTValueStr(), Operation2Str[m_operation],
    m_arg1->getASTValueStr(),
    m_arg2 ? m_arg2->getASTValueStr() : ""
  );
}
