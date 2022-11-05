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
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace std::literals;

TEST(ParserTest, Functions) {
  constexpr auto program = R"(
  fn empty() -> void {}
  fn main(argc: int, argv: string) -> int { return 5; }
  )"sv;
  std::istringstream iss{program.data()};

  auto lexer = std::make_unique<Lexer>(iss);
  auto parser = std::make_unique<Parser>(*lexer);
  auto root = parser->parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 2);
  // fn empty() -> void {}
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0])) {
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "empty");
    EXPECT_EQ(fn->getParams().size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
    // {}
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
    EXPECT_EQ(stmtBlock->getStatements().size(), 0);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
  // fn main(argc: int, argv: string) -> int { return 5; }
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[1])) {
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
    EXPECT_EQ(fn->getParams().size(), 2);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_INT);
    // argc: int
    auto param0Var = dynamic_cast<AST::VarExpr *>(&*fn->getParams()[0]->getVar());
    EXPECT_STREQ(param0Var->getToken()->getValue<std::string>().c_str(), "argc");
    EXPECT_EQ(param0Var->getType()->getType(), TType::KW_INT);
    // argv: string
    auto param1Var = dynamic_cast<AST::VarExpr *>(&*fn->getParams()[1]->getVar());
    EXPECT_STREQ(param1Var->getToken()->getValue<std::string>().c_str(), "argv");
    EXPECT_EQ(param1Var->getType()->getType(), TType::KW_STRING);
    // { return 5; }
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
    EXPECT_EQ(stmtBlock->getStatements().size(), 1);
    auto returnStmt = dynamic_cast<AST::ReturnStmt *>(&*stmtBlock->getStatements()[0]);
    EXPECT_NE(returnStmt, nullptr);
    EXPECT_EQ(returnStmt->getReturnValue()->getToken()->getType(), TType::LIT_INT);
    EXPECT_EQ(returnStmt->getReturnValue()->getToken()->getValue<int>(), 5);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
