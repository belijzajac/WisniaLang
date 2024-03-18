// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

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
