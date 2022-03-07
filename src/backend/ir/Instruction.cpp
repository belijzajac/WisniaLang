#include <fmt/format.h>
// Wisnia
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

void Wisnia::Instruction::print() const {
  fmt::print("{:^15} %% {:<15}|{:^8}|{:^15} %% {:<15}|{:^15} %% {:<15}\n",
    // target
    m_target->getASTValueStr(),
    TokenType2Str[m_target->getType()],
    // operation
    Operation2Str[m_operation],
    // arg1
    m_arg1->getASTValueStr(),
    TokenType2Str[m_arg1->getType()],
    // arg2
    m_arg2 ? m_arg2->getASTValueStr() : "",
    m_arg2 ? TokenType2Str[m_arg2->getType()] : ""
  );
}
