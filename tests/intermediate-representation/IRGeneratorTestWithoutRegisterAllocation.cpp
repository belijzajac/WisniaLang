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
using namespace std::literals;

class IRGeneratorTestWithoutRegisterAllocationFixture : public testing::Test {
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
  IRGenerator m_generator{false};

 private:
  SemanticAnalysis m_analysis{};
};

using IRGeneratorTestWithoutRegisterAllocation = IRGeneratorTestWithoutRegisterAllocationFixture;

TEST_F(IRGeneratorTestWithoutRegisterAllocation, VariableDeclarationStatements) {
  constexpr auto program = R"(
  fn main() {
    int aa = 5 + 2 * 10;
    int ba = 7 - 1;
    int bb = aa - ba;
  })"sv;
  SetUp(program.data());
  const auto &temporaries = m_generator.getTemporaryVars();
  const auto &instructions = m_generator.getInstructions(IRGenerator::Transformation::NONE);

  EXPECT_EQ(temporaries.size(), 4); // from _t0 to _t3

  // _tx = a <op> b rewritten as
  //    _tx = a
  //    _tx = _tx <op> b

  // <_t0, *, 2, 10> = <target, op, arg1, arg2>
  EXPECT_EQ(instructions[0]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[0]->getTarget()->getValue<std::string>().c_str(), "_t0");
  EXPECT_EQ(instructions[0]->getArg1()->getValue<int>(), 2);
  EXPECT_EQ(instructions[1]->getOperation(), Operation::IMUL);
  EXPECT_STREQ(instructions[1]->getTarget()->getValue<std::string>().c_str(), "_t0");
  EXPECT_EQ(instructions[1]->getArg1()->getValue<int>(), 10);

  // <_t1, +, 5, _t0>
  EXPECT_EQ(instructions[2]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[2]->getTarget()->getValue<std::string>().c_str(), "_t1");
  EXPECT_EQ(instructions[2]->getArg1()->getValue<int>(), 5);
  EXPECT_EQ(instructions[3]->getOperation(), Operation::IADD);
  EXPECT_STREQ(instructions[3]->getTarget()->getValue<std::string>().c_str(), "_t1");
  EXPECT_STREQ(instructions[3]->getArg1()->getValue<std::string>().c_str(), "_t0");

  // <aa, <-, _t1, null>
  EXPECT_EQ(instructions[4]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[4]->getTarget()->getValue<std::string>().c_str(), "aa");
  EXPECT_STREQ(instructions[4]->getArg1()->getValue<std::string>().c_str(), "_t1");

  // <_t2, -, 7, 1>
  EXPECT_EQ(instructions[5]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[5]->getTarget()->getValue<std::string>().c_str(), "_t2");
  EXPECT_EQ(instructions[5]->getArg1()->getValue<int>(), 7);
  EXPECT_EQ(instructions[6]->getOperation(), Operation::ISUB);
  EXPECT_STREQ(instructions[6]->getTarget()->getValue<std::string>().c_str(), "_t2");
  EXPECT_EQ(instructions[6]->getArg1()->getValue<int>(), 1);

  // <ba, <-, _t2, null>
  EXPECT_EQ(instructions[7]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[7]->getTarget()->getValue<std::string>().c_str(), "ba");
  EXPECT_STREQ(instructions[7]->getArg1()->getValue<std::string>().c_str(), "_t2");

  // <_t3, <-, aa, null>
  EXPECT_EQ(instructions[8]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[8]->getTarget()->getValue<std::string>().c_str(), "_t3");
  EXPECT_STREQ(instructions[8]->getArg1()->getValue<std::string>().c_str(), "aa");

  // <_t3, -, ba, null>
  EXPECT_EQ(instructions[9]->getOperation(), Operation::ISUB);
  EXPECT_STREQ(instructions[9]->getTarget()->getValue<std::string>().c_str(), "_t3");
  EXPECT_STREQ(instructions[9]->getArg1()->getValue<std::string>().c_str(), "ba");

  // <bb, <-, _t3, null>
  EXPECT_EQ(instructions[10]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[10]->getTarget()->getValue<std::string>().c_str(), "bb");
  EXPECT_STREQ(instructions[10]->getArg1()->getValue<std::string>().c_str(), "_t3");
}

TEST_F(IRGeneratorTestWithoutRegisterAllocation, PrintIRForVariableDeclarationStatements) {
  constexpr auto program = R"(
  fn main() {
    int aa = 5 + 2 * 10;
    int ba = 7 - 1;
    int bb = aa - ba;
  })"sv;
  SetUp(program.data());
  std::stringstream ss;
  m_generator.printInstructions(ss, IRGenerator::Transformation::NONE);

  EXPECT_STREQ(ss.str().c_str(),
    "              Target               |      Op      |          Arg1          |               Arg2               \n"
    "-----------------------------------+--------------+------------------------+----------------------------------\n"
    "      _t0        %% IDENT_INT      |      <-      |  2   %% LIT_INT        |                %%                \n"
    "      _t0        %% IDENT_INT      |      *       | 10   %% LIT_INT        |                %%                \n"
    "      _t1        %% IDENT_INT      |      <-      |  5   %% LIT_INT        |                %%                \n"
    "      _t1        %% IDENT_INT      |      +       | _t0  %% IDENT_INT      |                %%                \n"
    "       aa        %% IDENT_INT      |      <-      | _t1  %% IDENT_INT      |                %%                \n"
    "      _t2        %% IDENT_INT      |      <-      |  7   %% LIT_INT        |                %%                \n"
    "      _t2        %% IDENT_INT      |      -       |  1   %% LIT_INT        |                %%                \n"
    "       ba        %% IDENT_INT      |      <-      | _t2  %% IDENT_INT      |                %%                \n"
    "      _t3        %% IDENT_INT      |      <-      | aa   %% IDENT_INT      |                %%                \n"
    "      _t3        %% IDENT_INT      |      -       | ba   %% IDENT_INT      |                %%                \n"
    "       bb        %% IDENT_INT      |      <-      | _t3  %% IDENT_INT      |                %%                \n"
    " __builtin_exit  %% IDENT_VOID     |     call     |      %%                |                %%                \n"
  );
}
