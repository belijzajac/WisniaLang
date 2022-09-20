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

#include <fmt/format.h>

#include <optional>
#include <set>
#include <vector>
// Wisnia
#include "RegisterAllocator.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

template<class T>
struct is_shared_ptr : std::false_type {};

template<class T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <class T>
concept TokenPointer = requires(T t) {
  requires is_shared_ptr<T>::value;
};

template <TokenPointer TToken, typename TVariable>
bool checkVariable(const TToken token, const TVariable &variable) {
  using VariableType = std::decay_t<decltype(variable)>;
  if (!token) return false;
  const auto type = token->getType();
  const auto satisfiesType{type == TType::IDENT_INT || type == TType::IDENT_FLOAT ||
                           type == TType::IDENT_STRING || type == TType::IDENT_BOOL};
  return satisfiesType && token->template getValue<VariableType>() == variable;
}

void RegisterAllocator::allocateRegisters(instructions_list &&instructions) {
  // List of live intervals
  const auto cmp_1 = [](const auto &a, const auto &b) { return a.m_start < b.m_start; };
  std::set<Live, decltype(cmp_1)> liveIntervals{};

  // Populate the list with each variable's starting and ending interval points
  for (size_t i = 0; i < instructions.size(); i++) {
    if (!instructions[i]->getTarget()) continue;

    const auto var{instructions[i]->getTarget()->getValue<std::string>()};
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
  const auto cmp_2 = [](const auto &a, const auto &b) { return a.m_start > b.m_start; };
  std::set<Live, decltype(cmp_2)> activeIntervals{};

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
    // Search for the unassigned register
    const auto availableRegister = [&]() -> std::optional<std::string> {
      for (auto &r : availableRegisters.m_registers) {
        if (!r.m_assigned) {
          r.m_assigned = true;
          return r.m_name;
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
      node.value().m_register = "[spill]"; // todo: push/pop
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

#if 0
  fmt::print("-------\n");
  fmt::print("{:^10}{:^10}{:^6}{:^6}\n", "var", "reg", "start", "end\n");
  for (const auto &live : liveIntervals) {
    fmt::print("{:^10}{:^10}{:^6}{:^6}\n", live.m_variable, live.m_register, live.m_start, live.m_end);
  }
#endif

  m_instructions.insert(m_instructions.end(), instructions.begin(), instructions.end());
}
