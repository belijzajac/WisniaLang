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
  fn empty() {}
  fn main(argc: int, argv: string) -> int { return 5; }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 2);

  {
    // fn empty()
    auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
    EXPECT_NE(fn, nullptr);
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "empty");
    EXPECT_EQ(fn->getParams().size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
    EXPECT_NE(fnVar, nullptr);
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
    // {}
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
    EXPECT_NE(stmtBlock, nullptr);
    EXPECT_EQ(stmtBlock->getStatements().size(), 0);
  }

  {
    // fn main(argc: int, argv: string) -> int
    auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[1]);
    EXPECT_NE(fn, nullptr);
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
    EXPECT_EQ(fn->getParams().size(), 2);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
    EXPECT_NE(fnVar, nullptr);
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_INT);
    // argc: int
    auto param0Var = dynamic_cast<AST::VarExpr *>(&*fn->getParams()[0]->getVar());
    EXPECT_NE(param0Var, nullptr);
    EXPECT_STREQ(param0Var->getToken()->getValue<std::string>().c_str(), "argc");
    EXPECT_EQ(param0Var->getType()->getType(), TType::KW_INT);
    // argv: string
    auto param1Var = dynamic_cast<AST::VarExpr *>(&*fn->getParams()[1]->getVar());
    EXPECT_NE(param1Var, nullptr);
    EXPECT_STREQ(param1Var->getToken()->getValue<std::string>().c_str(), "argv");
    EXPECT_EQ(param1Var->getType()->getType(), TType::KW_STRING);
    // { return 5; }
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
    EXPECT_NE(stmtBlock, nullptr);
    EXPECT_EQ(stmtBlock->getStatements().size(), 1);
    auto returnStmt = dynamic_cast<AST::ReturnStmt *>(&*stmtBlock->getStatements()[0]);
    EXPECT_NE(returnStmt, nullptr);
    EXPECT_EQ(returnStmt->getReturnValue()->getToken()->getType(), TType::LIT_INT);
    EXPECT_EQ(returnStmt->getReturnValue()->getToken()->getValue<int>(), 5);
  }
}

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
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "loops");
  EXPECT_EQ(fn->getParams().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
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
  EXPECT_NE(whileLoopStmtBlock, nullptr);
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
  EXPECT_NE(forLoopInitVar, nullptr);
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
  EXPECT_NE(forLoopIncDecVar, nullptr);
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
  EXPECT_NE(forLoopStmtBlock, nullptr);
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
  EXPECT_NE(forEachLoopStmtBlock, nullptr);
  EXPECT_EQ(forEachLoopStmtBlock->getStatements().size(), 0);
}

TEST(ParserTest, Conditionals) {
  constexpr auto program = R"(
  fn conditionals() {
    if (true) {
      f(5 - 1, 5);
    }
    elif (3 == "3") {
      f(0);
    }
    elif (abc) {
      continue;
    }
    else {
      break;
    }
  })"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);

  // fn conditionals()
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "conditionals");
  EXPECT_EQ(fn->getParams().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
  EXPECT_EQ(stmtBlock->getStatements().size(), 1);
  // if (true)
  auto ifStmt = dynamic_cast<AST::IfStmt *>(&*stmtBlock->getStatements()[0]);
  EXPECT_NE(ifStmt, nullptr);
  EXPECT_EQ(ifStmt->getToken()->getType(), TType::KW_IF);
  EXPECT_EQ(ifStmt->getElseStatements().size(), 3);
  // true
  auto ifCond = dynamic_cast<AST::BoolExpr *>(&*ifStmt->getCondition());
  EXPECT_NE(ifCond, nullptr);
  EXPECT_EQ(ifCond->getToken()->getType(), TType::KW_TRUE);
  EXPECT_EQ(ifCond->getToken()->getValue<bool>(), true);
  // { f(5 - 1, 5); }
  auto ifBody = dynamic_cast<AST::StmtBlock *>(&*ifStmt->getBody());
  EXPECT_NE(ifBody, nullptr);
  EXPECT_EQ(ifBody->getStatements().size(), 1);
  // f
  auto ifExprStmt = dynamic_cast<AST::ExprStmt *>(&*ifBody->getStatements()[0]);
  EXPECT_NE(ifExprStmt, nullptr);
  auto ifFnCall = dynamic_cast<AST::FnCallExpr *>(&*ifExprStmt->getExpr());
  EXPECT_NE(ifFnCall, nullptr);
  EXPECT_EQ(ifFnCall->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(ifFnCall->getToken()->getValue<std::string>().c_str(), "f");
  EXPECT_EQ(ifFnCall->getArgs().size(), 2);
  // (5 - 1,
  auto ifFnCallSubArg = dynamic_cast<AST::SubExpr *>(&*ifFnCall->getArgs()[0]);
  EXPECT_NE(ifFnCallSubArg, nullptr);
  EXPECT_EQ(ifFnCallSubArg->getOperand(), TType::OP_SUB);
  // 5
  auto ifFnCallSubArgFive = dynamic_cast<AST::IntExpr *>(&*ifFnCallSubArg->lhs());
  EXPECT_NE(ifFnCallSubArgFive, nullptr);
  EXPECT_EQ(ifFnCallSubArgFive->getToken()->getType(), TType::LIT_INT);
  EXPECT_EQ(ifFnCallSubArgFive->getToken()->getValue<int>(), 5);
  // 1
  auto ifFnCallSubArgOne = dynamic_cast<AST::IntExpr *>(&*ifFnCallSubArg->rhs());
  EXPECT_NE(ifFnCallSubArgOne, nullptr);
  EXPECT_EQ(ifFnCallSubArgOne->getToken()->getType(), TType::LIT_INT);
  EXPECT_EQ(ifFnCallSubArgOne->getToken()->getValue<int>(), 1);
  // 5);
  auto ifFnCallIntArg = dynamic_cast<AST::IntExpr *>(&*ifFnCall->getArgs()[1]);
  EXPECT_NE(ifFnCallIntArg, nullptr);
  EXPECT_EQ(ifFnCallIntArg->getToken()->getType(), TType::LIT_INT);
  EXPECT_EQ(ifFnCallIntArg->getToken()->getValue<int>(), 5);
  // elif (3 == "3")
  auto elifStmt = dynamic_cast<AST::ElseIfStmt *>(&*ifStmt->getElseStatements()[0]);
  EXPECT_NE(elifStmt, nullptr);
  EXPECT_EQ(elifStmt->getToken()->getType(), TType::KW_ELIF);
  // (3 == "3")
  auto elifCondExpr = dynamic_cast<AST::EqExpr *>(&*elifStmt->getCondition());
  EXPECT_NE(elifCondExpr, nullptr);
  EXPECT_EQ(elifCondExpr->getOperand(), TType::OP_EQ);
  // 3
  auto elifCondExprThree = dynamic_cast<AST::IntExpr *>(&*elifCondExpr->lhs());
  EXPECT_NE(elifCondExprThree, nullptr);
  EXPECT_EQ(elifCondExprThree->getToken()->getType(), TType::LIT_INT);
  EXPECT_EQ(elifCondExprThree->getToken()->getValue<int>(), 3);
  // "3"
  auto elifCondExprStrThree = dynamic_cast<AST::StringExpr *>(&*elifCondExpr->rhs());
  EXPECT_NE(elifCondExprStrThree, nullptr);
  EXPECT_EQ(elifCondExprStrThree->getToken()->getType(), TType::LIT_STR);
  EXPECT_STREQ(elifCondExprStrThree->getToken()->getValue<std::string>().c_str(), "3");
  // { f(0); }
  auto elifBody = dynamic_cast<AST::StmtBlock *>(&*elifStmt->getBody());
  EXPECT_NE(elifBody, nullptr);
  EXPECT_EQ(elifBody->getStatements().size(), 1);
  // f
  auto elifExprStmt = dynamic_cast<AST::ExprStmt *>(&*elifBody->getStatements()[0]);
  EXPECT_NE(elifExprStmt, nullptr);
  auto elifFnCall = dynamic_cast<AST::FnCallExpr *>(&*elifExprStmt->getExpr());
  EXPECT_NE(elifFnCall, nullptr);
  EXPECT_EQ(elifFnCall->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(elifFnCall->getToken()->getValue<std::string>().c_str(), "f");
  EXPECT_EQ(elifFnCall->getArgs().size(), 1);
  // 0
  auto elifFnCallArg = dynamic_cast<AST::IntExpr *>(&*elifFnCall->getArgs()[0]);
  EXPECT_NE(elifFnCallArg, nullptr);
  EXPECT_EQ(elifFnCallArg->getToken()->getType(), TType::LIT_INT);
  EXPECT_EQ(elifFnCallArg->getToken()->getValue<int>(), 0);
  // elif (abc)
  auto elifSecondStmt = dynamic_cast<AST::ElseIfStmt *>(&*ifStmt->getElseStatements()[1]);
  EXPECT_NE(elifSecondStmt, nullptr);
  EXPECT_EQ(elifSecondStmt->getToken()->getType(), TType::KW_ELIF);
  // (abc)
  auto elifSecondCondExpr = dynamic_cast<AST::VarExpr *>(&*elifSecondStmt->getCondition());
  EXPECT_NE(elifSecondCondExpr, nullptr);
  EXPECT_EQ(elifSecondCondExpr->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(elifSecondCondExpr->getToken()->getValue<std::string>().c_str(), "abc");
  // { continue; }
  auto elifSecondBody = dynamic_cast<AST::StmtBlock *>(&*elifSecondStmt->getBody());
  EXPECT_NE(elifSecondBody, nullptr);
  EXPECT_EQ(elifSecondBody->getStatements().size(), 1);
  auto continueExprStmt = dynamic_cast<AST::ContinueStmt *>(&*elifSecondBody->getStatements()[0]);
  EXPECT_NE(continueExprStmt, nullptr);
  EXPECT_EQ(continueExprStmt->getToken()->getType(), TType::KW_CONTINUE);
  // else
  auto elseStmt = dynamic_cast<AST::ElseStmt *>(&*ifStmt->getElseStatements()[2]);
  EXPECT_NE(elseStmt, nullptr);
  EXPECT_EQ(elseStmt->getToken()->getType(), TType::KW_ELSE);
  // { break; }
  auto elseBody = dynamic_cast<AST::StmtBlock *>(&*elseStmt->getBody());
  EXPECT_NE(elseBody, nullptr);
  EXPECT_EQ(elseBody->getStatements().size(), 1);
  auto breakExprStmt = dynamic_cast<AST::BreakStmt *>(&*elseBody->getStatements()[0]);
  EXPECT_NE(breakExprStmt, nullptr);
  EXPECT_EQ(breakExprStmt->getToken()->getType(), TType::KW_BREAK);
}
