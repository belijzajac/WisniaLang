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

#include <optional>
#include <set>
// Wisnia
#include "RegisterAllocator.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

template <typename T>
bool checkVariable(const std::shared_ptr<Basic::Token> &token, const T &variable) {
  using VariableType = std::decay_t<decltype(variable)>;
  if (!token) return false;
  return token->isIdentifierType() && token->getValue<VariableType>() == variable;
}

// Linear Scan algorithm (default for LLVM)
void RegisterAllocator::allocate(InstructionList &&instructions, bool allocateRegisters) {
  if (!allocateRegisters) {
    m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
    return;
  }

  // List of live intervals
  const auto intervalComparison = [](const auto &a, const auto &b) { return a.m_start < b.m_start; };
  std::set<Live, decltype(intervalComparison)> liveIntervals{};

  // Populate the list with each variable's starting and ending interval points
  for (size_t i = 0; i < instructions.size(); i++) {
    std::string var{};
    const auto &target = instructions[i]->getTarget();
    const auto &argOne = instructions[i]->getArg1();
    const auto &argTwo = instructions[i]->getArg2();

    if (target) {
      if (target->getType() == TType::REGISTER) continue;
      var = target->getValue<std::string>();
    } else if (argOne) {
      if (argOne->getType() == TType::REGISTER) continue;
      var = argOne->getValue<std::string>();
    } else if (argTwo) {
      if (argTwo->getType() == TType::REGISTER) continue;
      var = argTwo->getValue<std::string>();
    } else {
      continue;
    }

    const size_t start = i;
    size_t end = i;

    for (size_t j = i + 1; j < instructions.size(); j++) {
      if (checkVariable(instructions[j]->getTarget(), var) ||
          checkVariable(instructions[j]->getArg1(), var) ||
          checkVariable(instructions[j]->getArg2(), var)) {
        end = j;
      }
    }

    liveIntervals.emplace(Live{.m_variable = var, .m_start = start, .m_end = end});
  }

  // List of available registers
  Registers availableRegisters{};

  // List of the intervals that have been given a register and overlap with the current interval
  const auto overlapComparison = [](const auto &a, const auto &b) { return a.m_start > b.m_start; };
  std::set<Live, decltype(overlapComparison)> activeIntervals{};

  // Process each interval in the list in order
  for (const auto &interval : liveIntervals) {
    // Remove all expired intervals
    std::erase_if(activeIntervals, [&](const auto &active) {
      if (active.m_end <= interval.m_start) {
        auto &r = availableRegisters[active.m_register];
        r.m_assigned = false;
        return true;
      }
      return false;
    });
    // Search for an unassigned register
    const auto availableRegister = [&]() -> std::optional<Basic::register_t> {
      for (auto &r : availableRegisters.m_registers) {
        if (!r.m_assigned) {
          r.m_assigned = true;
          return r.m_register;
        }
      }
      return {};
    };
    if (const auto &reg = availableRegister(); reg.has_value()) {
      // Allocate the register to the current interval
      // and add the current interval to the active list
      auto node = liveIntervals.extract(interval);
      node.value().m_register = reg.value();
      liveIntervals.insert(std::move(node));
      activeIntervals.insert(interval);
    } else {
      // We ran out of registers - spill it
      auto node = liveIntervals.extract(interval);
      node.value().m_register = Basic::register_t::SPILLED;
      liveIntervals.insert(std::move(node));
    }
  }

  // Assign registers to instructions
  for (auto& instr : instructions) {
    for (const auto &live : liveIntervals) {
      if (const auto &var = instr->getTarget(); var && checkVariable(var, live.m_variable)) {
        var->setType(TType::REGISTER);
        var->setValue(live.m_register);
      }
      if (const auto &var = instr->getArg1(); var && checkVariable(var, live.m_variable)) {
        var->setType(TType::REGISTER);
        var->setValue(live.m_register);
      }
      if (const auto &var = instr->getArg2(); var && checkVariable(var, live.m_variable)) {
        var->setType(TType::REGISTER);
        var->setValue(live.m_register);
      }
    }
  }

  m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
}
