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

TEST(ParserTest, Loops) {
  constexpr auto program = R"(
  fn loops() {
    while (i < 5) {}
    for (int i = 0; six <= 6.59; i = i + "1") {}
    for_each (elem in elems) {}
  })"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);
  // fn loops()
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0])) {
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "loops");
    EXPECT_EQ(fn->getParams().size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
    EXPECT_EQ(stmtBlock->getStatements().size(), 3);
    // while (i < 5) {}
    auto whileLoopStmt = dynamic_cast<AST::WhileLoop *>(&*stmtBlock->getStatements()[0]);
    EXPECT_NE(whileLoopStmt, nullptr);
    EXPECT_EQ(whileLoopStmt->getToken()->getType(), TType::KW_WHILE);
    // i < 5
    auto whileLoopCondition = dynamic_cast<AST::CompExpr *>(&*whileLoopStmt->getCondition());
    EXPECT_NE(whileLoopCondition, nullptr);
    EXPECT_EQ(whileLoopCondition->getOperand(), TType::OP_L);
    auto whileLoopConditionVar = dynamic_cast<AST::VarExpr *>(&*whileLoopCondition->lhs());
    EXPECT_NE(whileLoopConditionVar, nullptr);
    EXPECT_EQ(whileLoopConditionVar->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(whileLoopConditionVar->getToken()->getValue<std::string>().c_str(), "i");
    // 5
    auto whileLoopConditionIntConst = dynamic_cast<AST::IntExpr *>(&*whileLoopCondition->rhs());
    EXPECT_NE(whileLoopConditionIntConst, nullptr);
    EXPECT_EQ(whileLoopConditionIntConst->getToken()->getType(), TType::LIT_INT);
    EXPECT_EQ(whileLoopConditionIntConst->getToken()->getValue<int>(), 5);
    // {}
    auto whileLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*whileLoopStmt->getBody());
    EXPECT_EQ(whileLoopStmtBlock->getStatements().size(), 0);
    // for (int i = 0; i < 5; i = i + 1) {}
    auto forLoopStmt = dynamic_cast<AST::ForLoop *>(&*stmtBlock->getStatements()[1]);
    EXPECT_NE(forLoopStmt, nullptr);
    EXPECT_EQ(forLoopStmt->getToken()->getType(), TType::KW_FOR);
    // int i = 0;
    auto forLoopInit = dynamic_cast<AST::VarDeclStmt *>(&*forLoopStmt->getInitial());
    EXPECT_NE(forLoopInit, nullptr);
    // int
    auto forLoopInitVar = dynamic_cast<AST::VarExpr *>(&*forLoopInit->getVar());
    EXPECT_EQ(forLoopInitVar->getType()->getType(), TType::KW_INT);
    // i
    EXPECT_EQ(forLoopInitVar->getToken()->getType(), TType::IDENT_INT);
    EXPECT_STREQ(forLoopInitVar->getToken()->getValue<std::string>().c_str(), "i");
    // 0
    auto forLoopInitConstExpr = dynamic_cast<AST::IntExpr *>(&*forLoopInit->getValue());
    EXPECT_NE(forLoopInitConstExpr, nullptr);
    EXPECT_EQ(forLoopInitConstExpr->getToken()->getType(), TType::LIT_INT);
    EXPECT_EQ(forLoopInitConstExpr->getToken()->getValue<int>(), 0);
    // six <= 6.59;
    auto forLoopComparisonExpr = dynamic_cast<AST::CompExpr *>(&*forLoopStmt->getCondition());
    EXPECT_NE(forLoopComparisonExpr, nullptr);
    EXPECT_EQ(forLoopComparisonExpr->getOperand(), TType::OP_LE);
    auto forLoopComparisonVar = dynamic_cast<AST::VarExpr *>(&*forLoopComparisonExpr->lhs());
    EXPECT_NE(forLoopComparisonVar, nullptr);
    EXPECT_EQ(forLoopComparisonVar->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopComparisonVar->getToken()->getValue<std::string>().c_str(), "six");
    // 6.59
    auto forLoopComparisonFltConst = dynamic_cast<AST::FloatExpr *>(&*forLoopComparisonExpr->rhs());
    EXPECT_NE(forLoopComparisonFltConst, nullptr);
    EXPECT_EQ(forLoopComparisonFltConst->getToken()->getType(), TType::LIT_FLT);
    EXPECT_EQ(forLoopComparisonFltConst->getToken()->getValue<float>(), 6.59f);
    // i = i + "1"
    auto forLoopIncDec = dynamic_cast<AST::VarAssignStmt *>(&*forLoopStmt->getIncrement());
    EXPECT_NE(forLoopIncDec, nullptr);
    // i
    auto forLoopIncDecVar = dynamic_cast<AST::VarExpr *>(&*forLoopIncDec->getVar());
    EXPECT_EQ(forLoopIncDecVar->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopIncDecVar->getToken()->getValue<std::string>().c_str(), "i");
    // i + "1"
    auto forLoopIncDecAddExpr = dynamic_cast<AST::AddExpr *>(&*forLoopIncDec->getValue());
    EXPECT_NE(forLoopIncDecAddExpr, nullptr);
    EXPECT_EQ(forLoopIncDecAddExpr->getOperand(), TType::OP_ADD);
    auto forLoopIncDecLhs = dynamic_cast<AST::VarExpr *>(&*forLoopIncDecAddExpr->lhs());
    EXPECT_NE(forLoopIncDecLhs, nullptr);
    EXPECT_EQ(forLoopIncDecLhs->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopIncDecLhs->getToken()->getValue<std::string>().c_str(), "i");
    // "1"
    auto forLoopIncDecRhs = dynamic_cast<AST::StringExpr *>(&*forLoopIncDecAddExpr->rhs());
    EXPECT_NE(forLoopIncDecRhs, nullptr);
    EXPECT_EQ(forLoopIncDecRhs->getToken()->getType(), TType::LIT_STR);
    EXPECT_STREQ(forLoopIncDecRhs->getToken()->getValue<std::string>().c_str(), "1");
    // {}
    auto forLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*forLoopStmt->getBody());
    EXPECT_EQ(forLoopStmtBlock->getStatements().size(), 0);
    // for_each (elem in elems) {}
    auto forEachLoopStmt = dynamic_cast<AST::ForEachLoop *>(&*stmtBlock->getStatements()[2]);
    EXPECT_NE(forEachLoopStmt, nullptr);
    EXPECT_EQ(forEachLoopStmt->getToken()->getType(), TType::KW_FOREACH);
    // elem
    auto forEachElem = dynamic_cast<AST::VarExpr *>(&*forEachLoopStmt->getElement());
    EXPECT_NE(forEachElem, nullptr);
    EXPECT_EQ(forEachElem->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(forEachElem->getToken()->getValue<std::string>().c_str(), "elem");
    // elems
    auto forEachIterElem = dynamic_cast<AST::VarExpr *>(&*forEachLoopStmt->getCollection());
    EXPECT_NE(forEachIterElem, nullptr);
    EXPECT_EQ(forEachIterElem->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(forEachIterElem->getToken()->getValue<std::string>().c_str(), "elems");
    // {}
    auto forEachLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*forEachLoopStmt->getBody());
    EXPECT_EQ(forEachLoopStmtBlock->getStatements().size(), 0);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
