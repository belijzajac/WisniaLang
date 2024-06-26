// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <optional>
#include <set>
#include <algorithm>
// Wisnia
#include "RegisterAllocator.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

using TokenPtr = std::shared_ptr<Token>;

template <typename T>
bool checkVariable(const TokenPtr &token, const T &variable) {
  using VariableType = std::decay_t<decltype(variable)>;
  if (!token) return false;
  return token->isIdentifierType() && token->getValue<VariableType>() == variable;
}

// Linear Scan algorithm (default for LLVM)
// https://pages.cs.wisc.edu/~horwitz/CS701-NOTES/5.REGISTER-ALLOCATION.html#linearScan
void RegisterAllocator::allocate(InstructionList &&instructions, const bool allocateRegisters) {
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
      auto it = std::find_if(availableRegisters.m_registers.begin(), availableRegisters.m_registers.end(),
        [](const Registers::RegisterState r) { return !r.m_assigned; }
      );
      if (it != availableRegisters.m_registers.end()) {
        it->m_assigned = true;
        return it->m_register;
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
      node.value().m_register = SPILLED;
      liveIntervals.insert(std::move(node));
    }
  }

  // Assign registers to instructions
  for (const auto &instr : instructions) {
    for (const auto &[liveVar, liveReg, ignored1, ignored2] : liveIntervals) {
      if (const auto &var = instr->getTarget(); var && checkVariable(var, liveVar)) {
        var->setType(TType::REGISTER);
        var->setValue(liveReg);
      }
      if (const auto &var = instr->getArg1(); var && checkVariable(var, liveVar)) {
        var->setType(TType::REGISTER);
        var->setValue(liveReg);
      }
      if (const auto &var = instr->getArg2(); var && checkVariable(var, liveVar)) {
        var->setType(TType::REGISTER);
        var->setValue(liveReg);
      }
    }
  }

  m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
}
