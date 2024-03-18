// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <fmt/format.h>
// Wisnia
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

void Wisnia::Instruction::print(std::ostream &output) const {
  output << fmt::format("{:^{}} %% {:<15}|{:^14}|{:^{}} %% {:<15}|{:^15} %% {:<15}\n",
    // target
    m_target ? m_target->getASTValueStr() : "", sPrintTargetWidth + 2,
    m_target ? TokenType2Str[m_target->getType()] : "",
    // operation
    Operation2Str[m_operation],
    // arg1
    m_arg1 ? m_arg1->getASTValueStr() : "", sPrintArgOneWidth + 2,
    m_arg1 ? TokenType2Str[m_arg1->getType()] : "",
    // arg2
    m_arg2 ? m_arg2->getASTValueStr() : "",
    m_arg2 ? TokenType2Str[m_arg2->getType()] : ""
  );
}
