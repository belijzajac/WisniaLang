// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <gtest/gtest.h>
// Wisnia
#include "AST.hpp"
#include "IRGenerator.hpp"
#include "Instruction.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"

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
    root->accept(m_analysis);
    root->accept(m_generator);
  }

 protected:
  IRGenerator m_generator{};

 private:
  SemanticAnalysis m_analysis{};
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
    int p = 16; // expected to be spilled
    int r = 17; // expected to be spilled
    int sum = a + b + c + d + e + f + g + h + i + j + k + l + m + n + o + p + r;
  })"sv;
  SetUp(program.data());
  constexpr auto registers = RegisterAllocator::getAllocatableRegisters;
  const auto &instructions = m_generator.getInstructions(IRGenerator::Transformation::INSTRUCTION_OPTIMIZATION);

  // 16 assigned registers
  for (size_t i = 0; i < registers.size(); i++) {
    const auto &op  = instructions[i]->getOperation();
    const auto &var = instructions[i]->getTarget();
    const auto &arg = instructions[i]->getArg1();
    EXPECT_EQ(op, Operation::MOV);
    EXPECT_EQ(var->getType(), TType::REGISTER);
    EXPECT_EQ(var->getValue<Basic::register_t>(), registers[i]);
    EXPECT_EQ(arg->getType(), TType::LIT_INT);
    EXPECT_EQ(arg->getValue<int>(), i + 1);
  }

  // 2 spilled registers

  EXPECT_EQ(instructions[15]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[15]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[15]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::SPILLED);
  EXPECT_EQ(instructions[15]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[15]->getArg1()->getValue<int>(), 16);

  EXPECT_EQ(instructions[16]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[16]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[16]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::SPILLED);
  EXPECT_EQ(instructions[16]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[16]->getArg1()->getValue<int>(), 17);

  // then goes the last 3 instructions marking the end of the program

  // xor rdi, rdi
  EXPECT_EQ(instructions[instructions.size() - 3]->getOperation(), Operation::XOR);
  EXPECT_EQ(instructions[instructions.size() - 3]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[instructions.size() - 3]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[instructions.size() - 3]->getArg2()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[instructions.size() - 3]->getArg2()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 0x3c
  EXPECT_EQ(instructions[instructions.size() - 2]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[instructions.size() - 2]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[instructions.size() - 2]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[instructions.size() - 2]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[instructions.size() - 2]->getArg1()->getValue<int>(), 60);
  // syscall
  EXPECT_EQ(instructions[instructions.size() - 1]->getOperation(), Operation::SYSCALL);
}
