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
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"

using namespace Wisnia;
using namespace std::literals;

class SemanticTestFixture : public testing::Test {
 protected:
  void SetUp(std::string_view program) {
    std::istringstream iss{program.data()};
    Lexer lexer{iss};
    Parser parser{lexer};
    m_root = parser.parse();
  }

 protected:
  std::unique_ptr<AST::Root> m_root;
};

using SemanticTest = SemanticTestFixture;

TEST_F(SemanticTest, VariableNotFound) {
  constexpr auto program = R"(
  fn main() {
    int foo = bar;
  }
  )"sv;
  SetUp(program.data());

  EXPECT_THROW(
      {
        SemanticAnalysis analysis;
        m_root->accept(analysis);
      },
      SemanticError);
}

TEST_F(SemanticTest, MainFunctionNotFound) {
  constexpr auto program = R"(fn foo() {})"sv;
  SetUp(program.data());

  EXPECT_THROW(
      {
        SemanticAnalysis analysis;
        m_root->accept(analysis);
      },
      SemanticError);
}

TEST_F(SemanticTest, NonVoidFunctionIsNotReturning) {
  constexpr auto program = R"(fn foo() -> int {})"sv;
  SetUp(program.data());

  EXPECT_THROW(
      {
        SemanticAnalysis analysis;
        m_root->accept(analysis);
      },
      SemanticError);
}

TEST_F(SemanticTest, MultipleDefinitionsForFunction) {
  constexpr auto program = R"(
  fn foo() {}
  fn foo() {}
  fn main() {}
  )"sv;
  SetUp(program.data());

  EXPECT_THROW(
      {
        SemanticAnalysis analysis;
        m_root->accept(analysis);
      },
      SemanticError);
}

TEST_F(SemanticTest, InsufficientArgumentsPassedToFunciton) {
  constexpr auto program = R"(
  fn bar(a: int, b: int) -> int {
    return 1;
  }
  fn main() {
    int foo = bar(34);
  }
  )"sv;
  SetUp(program.data());

  EXPECT_THROW(
      {
        SemanticAnalysis analysis;
        m_root->accept(analysis);
      },
      SemanticError);
}
