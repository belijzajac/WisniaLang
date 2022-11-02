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

class IRGeneratorTestFixture : public testing::Test {
 protected:
  void SetUp(const std::string &program) {
    std::istringstream iss{program};
    auto lexer = std::make_unique<Lexer>(iss);
    auto parser = std::make_unique<Parser>(*lexer);
    auto root = parser->parse();
    root->accept(&m_resolver);
    root->accept(&m_generator);
  }

 protected:
  IRGenerator m_generator{false};

 private:
  NameResolver m_resolver{};
};

using IRGeneratorTest = IRGeneratorTestFixture;

TEST_F(IRGeneratorTest, VarDeclStmt) {
  constexpr std::string_view program = R"(
  fn main () -> void {
    int aa = 5 + 2 * 10;
    int ba = 7 - 1;
    bool bb = 6 > 5 && 6 != ba;
  })";
  SetUp(program.data());

  EXPECT_EQ(m_generator.getTemporaryVars().size(), 6); // from _t0 to _t5
  const auto &instructions = m_generator.getInstructions();

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

  // <_t3, >, 6, 5>
  EXPECT_EQ(instructions[8]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[8]->getTarget()->getValue<std::string>().c_str(), "_t3");
  EXPECT_EQ(instructions[8]->getArg1()->getValue<int>(), 6);
  EXPECT_EQ(instructions[9]->getOperation(), Operation::IGT);
  EXPECT_STREQ(instructions[9]->getTarget()->getValue<std::string>().c_str(), "_t3");
  EXPECT_EQ(instructions[9]->getArg1()->getValue<int>(), 5);

  // <_t4, !=, 6, ba>
  EXPECT_EQ(instructions[10]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[10]->getTarget()->getValue<std::string>().c_str(), "_t4");
  EXPECT_EQ(instructions[10]->getArg1()->getValue<int>(), 6);
  EXPECT_EQ(instructions[11]->getOperation(), Operation::INE);
  EXPECT_STREQ(instructions[11]->getTarget()->getValue<std::string>().c_str(), "_t4");
  EXPECT_STREQ(instructions[11]->getArg1()->getValue<std::string>().c_str(), "ba");

  // <_t5, &&, _t3, _t4>
  EXPECT_EQ(instructions[12]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[12]->getTarget()->getValue<std::string>().c_str(), "_t5");
  EXPECT_STREQ(instructions[12]->getArg1()->getValue<std::string>().c_str(), "_t3");
  EXPECT_EQ(instructions[13]->getOperation(), Operation::AND);
  EXPECT_STREQ(instructions[13]->getTarget()->getValue<std::string>().c_str(), "_t5");
  EXPECT_STREQ(instructions[13]->getArg1()->getValue<std::string>().c_str(), "_t4");

  // <bb, <-, _t5, null>
  EXPECT_EQ(instructions[14]->getOperation(), Operation::MOV);
  EXPECT_STREQ(instructions[14]->getTarget()->getValue<std::string>().c_str(), "bb");
  EXPECT_STREQ(instructions[14]->getArg1()->getValue<std::string>().c_str(), "_t5");
}
