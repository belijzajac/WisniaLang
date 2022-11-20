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

#include <gtest/gtest.h>
// Wisnia
#include "AST.hpp"
#include "IRGenerator.hpp"
#include "Instruction.hpp"
#include "Lexer.hpp"
#include "NameResolver.hpp"
#include "Parser.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace std::literals;

class RegisterAllocatorTestFixture : public testing::Test {
 protected:
  void SetUp(std::string_view program) {
    std::istringstream iss{program.data()};
    Lexer lexer{iss};
    Parser parser{lexer};
    const auto &root = parser.parse();
    root->accept(&m_resolver);
    root->accept(&m_generator);
  }

 protected:
  IRGenerator m_generator{};

 private:
  NameResolver m_resolver{};
};

using RegisterAllocatorTest = RegisterAllocatorTestFixture;

TEST_F(RegisterAllocatorTest, RegisterForEachVariable) {
  constexpr auto program = R"(
  fn main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int e = 5;
    int f = 6;
    int g = 7;
    int h = 8;
    int i = 9;
    int j = 10;
    int k = 11;
    int l = 12;
    int m = 13;
    int n = 14;
    int o = 15;
    int p = 16; # expected to be spilled
    int r = 17; # expected to be spilled
    int sum = a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + r;
  })"sv;
  SetUp(program.data());
  constexpr auto registers = RegisterAllocator::getAllRegisters();
  const auto &instructions = m_generator.getInstructionsAfterInstructionSimplification();

  // 16 assigned registers
  for (size_t i = 0; i < registers.size(); i++) {
    const auto &op  = instructions[i]->getOperation();
    const auto &var = instructions[i]->getTarget();
    const auto &arg = instructions[i]->getArg1();
    EXPECT_EQ(op, Operation::MOV);
    EXPECT_EQ(var->getType(), TType::REGISTER);
    EXPECT_STREQ(var->getValue<std::string>().c_str(), registers[i].data());
    EXPECT_EQ(arg->getType(), TType::LIT_INT);
    EXPECT_EQ(arg->getValue<int>(), i + 1);
  }

  // 2 spilled registers

  EXPECT_EQ(instructions[15]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[15]->getTarget()->getType(), TType::REGISTER);
  EXPECT_STREQ(instructions[15]->getTarget()->getValue<std::string>().c_str(), "[spill]");
  EXPECT_EQ(instructions[15]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[15]->getArg1()->getValue<int>(), 16);

  EXPECT_EQ(instructions[16]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[16]->getTarget()->getType(), TType::REGISTER);
  EXPECT_STREQ(instructions[16]->getTarget()->getValue<std::string>().c_str(), "[spill]");
  EXPECT_EQ(instructions[16]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[16]->getArg1()->getValue<int>(), 17);

  // then goes the last 3 instructions marking the end of the program

  // xor rdi, rdi
  EXPECT_EQ(instructions[instructions.size() - 3]->getOperation(), Operation::XOR);
  EXPECT_EQ(instructions[instructions.size() - 3]->getArg1()->getType(), TType::REGISTER);
  EXPECT_STREQ(instructions[instructions.size() - 3]->getArg1()->getValue<std::string>().c_str(), "rdi");
  EXPECT_EQ(instructions[instructions.size() - 3]->getArg2()->getType(), TType::REGISTER);
  EXPECT_STREQ(instructions[instructions.size() - 3]->getArg2()->getValue<std::string>().c_str(), "rdi");
  // mov rax, 0x3c
  EXPECT_EQ(instructions[instructions.size() - 2]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[instructions.size() - 2]->getTarget()->getType(), TType::REGISTER);
  EXPECT_STREQ(instructions[instructions.size() - 2]->getTarget()->getValue<std::string>().c_str(), "rax");
  EXPECT_EQ(instructions[instructions.size() - 2]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[instructions.size() - 2]->getArg1()->getValue<int>(), 60);
  // syscall
  EXPECT_EQ(instructions[instructions.size() - 1]->getOperation(), Operation::SYSCALL);
}
