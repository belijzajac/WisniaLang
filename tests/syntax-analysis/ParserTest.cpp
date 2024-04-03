// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <gtest/gtest.h>
// Wisnia
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace std::literals;

TEST(ParserTest, NotAGlobalFunctionOrClass) {
  constexpr auto program = R"(int a;)"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};

  EXPECT_THROW(
      {
        Parser parser{lexer};
        const auto &root = parser.parse();
      },
      ParserError);
}

TEST(ParserTest, FunctionsMustHaveParametersList) {
  constexpr auto program = R"(fn main {})"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};

  EXPECT_THROW(
      {
        Parser parser{lexer};
        const auto &root = parser.parse();
      },
      ParserError);
}

TEST(ParserTest, FunctionNameMustBeAVariable) {
  constexpr auto program = R"(fn 555() {})"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};

  EXPECT_THROW(
      {
        Parser parser{lexer};
        const auto &root = parser.parse();
      },
      ParserError);
}

TEST(ParserTest, FunctionMustHaveASupportedReturnType) {
  constexpr auto program = R"(fn main() -> lol {})"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};

  EXPECT_THROW(
      {
        Parser parser{lexer};
        const auto &root = parser.parse();
      },
      ParserError);
}

TEST(ParserTest, Functions) {
  constexpr auto program = R"(
  fn empty() {}
  fn emptyReturn() { return; }
  fn main(argc: int, argv: string) -> int { return 5; }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 3);

  {
    // fn empty()
    auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
    EXPECT_NE(fn, nullptr);
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "empty");
    EXPECT_EQ(fn->getParameters().size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
    EXPECT_NE(fnVar, nullptr);
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
    // {}
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
    EXPECT_NE(stmtBlock, nullptr);
    EXPECT_EQ(stmtBlock->getStatements().size(), 0);
  }

  {
    // fn emptyReturn()
    auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[1]);
    EXPECT_NE(fn, nullptr);
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "emptyReturn");
    EXPECT_EQ(fn->getParameters().size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
    EXPECT_NE(fnVar, nullptr);
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
    // { return; }
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
    EXPECT_NE(stmtBlock, nullptr);
    EXPECT_EQ(stmtBlock->getStatements().size(), 1);
    auto returnStmt = dynamic_cast<AST::ReturnStmt *>(&*stmtBlock->getStatements()[0]);
    EXPECT_NE(returnStmt, nullptr);
    EXPECT_EQ(returnStmt->getReturnValue(), nullptr);
  }

  {
    // fn main(argc: int, argv: string) -> int
    auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[2]);
    EXPECT_NE(fn, nullptr);
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
    EXPECT_EQ(fn->getParameters().size(), 2);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
    EXPECT_NE(fnVar, nullptr);
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_INT);
    // argc: int
    auto param0Var = dynamic_cast<AST::VarExpr *>(&*fn->getParameters()[0]->getVariable());
    EXPECT_NE(param0Var, nullptr);
    EXPECT_STREQ(param0Var->getToken()->getValue<std::string>().c_str(), "argc");
    EXPECT_EQ(param0Var->getType()->getType(), TType::KW_INT);
    // argv: string
    auto param1Var = dynamic_cast<AST::VarExpr *>(&*fn->getParameters()[1]->getVariable());
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
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
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
  auto forLoopInitVar = dynamic_cast<AST::VarExpr *>(&*forLoopInit->getVariable());
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
  auto forLoopIncDecVar = dynamic_cast<AST::VarExpr *>(&*forLoopIncDec->getVariable());
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
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
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
  EXPECT_STREQ(ifCond->getToken()->getValueStr().c_str(), "true");
  // { f(5 - 1, 5); }
  auto ifBody = dynamic_cast<AST::StmtBlock *>(&*ifStmt->getBody());
  EXPECT_NE(ifBody, nullptr);
  EXPECT_EQ(ifBody->getStatements().size(), 1);
  // f
  auto ifExprStmt = dynamic_cast<AST::ExprStmt *>(&*ifBody->getStatements()[0]);
  EXPECT_NE(ifExprStmt, nullptr);
  auto ifFnCall = dynamic_cast<AST::FnCallExpr *>(&*ifExprStmt->getExpression());
  EXPECT_NE(ifFnCall, nullptr);
  EXPECT_EQ(ifFnCall->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(ifFnCall->getToken()->getValue<std::string>().c_str(), "f");
  EXPECT_EQ(ifFnCall->getArguments().size(), 2);
  // (5 - 1,
  auto ifFnCallSubArg = dynamic_cast<AST::SubExpr *>(&*ifFnCall->getArguments()[0]);
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
  auto ifFnCallIntArg = dynamic_cast<AST::IntExpr *>(&*ifFnCall->getArguments()[1]);
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
  auto elifFnCall = dynamic_cast<AST::FnCallExpr *>(&*elifExprStmt->getExpression());
  EXPECT_NE(elifFnCall, nullptr);
  EXPECT_EQ(elifFnCall->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(elifFnCall->getToken()->getValue<std::string>().c_str(), "f");
  EXPECT_EQ(elifFnCall->getArguments().size(), 1);
  // 0
  auto elifFnCallArg = dynamic_cast<AST::IntExpr *>(&*elifFnCall->getArguments()[0]);
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

TEST(ParserTest, LogicalOrExpression) {
  constexpr auto program = R"(
  fn main() {
    bool var1 = var2 || var3;
  }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);

  // fn main()
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
  EXPECT_EQ(stmtBlock->getStatements().size(), 1);
  // bool var1
  auto varDeclStmt = dynamic_cast<AST::VarDeclStmt *>(&*stmtBlock->getStatements()[0]);
  EXPECT_NE(varDeclStmt, nullptr);
  EXPECT_EQ(varDeclStmt->getVariable()->getToken()->getType(), TType::IDENT_BOOL);
  EXPECT_STREQ(varDeclStmt->getVariable()->getToken()->getValue<std::string>().c_str(), "var1");
  auto binaryExpr = dynamic_cast<AST::BinaryExpr *>(varDeclStmt->getValue().get());
  EXPECT_NE(binaryExpr, nullptr);
  // ||
  EXPECT_EQ(binaryExpr->getOperand(), TType::OP_OR);
  // var2
  EXPECT_NE(binaryExpr->lhs(), nullptr);
  EXPECT_EQ(binaryExpr->lhs()->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(binaryExpr->lhs()->getToken()->getValue<std::string>().c_str(), "var2");
  // var3
  EXPECT_NE(binaryExpr->rhs(), nullptr);
  EXPECT_EQ(binaryExpr->rhs()->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(binaryExpr->rhs()->getToken()->getValue<std::string>().c_str(), "var3");
}

TEST(ParserTest, UnaryExpression) {
  constexpr auto program = R"(
  fn main() {
    !var1;
    ++var2;
    --var3;
  }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);

  // fn main()
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
  EXPECT_EQ(stmtBlock->getStatements().size(), 3);
  // !
  auto negExpressionStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[0]);
  EXPECT_NE(negExpressionStmt, nullptr);
  auto negExpression = dynamic_cast<AST::UnaryExpr *>(negExpressionStmt->getExpression().get());
  EXPECT_NE(negExpression, nullptr);
  EXPECT_EQ(negExpression->getOperand(), TType::OP_UNEG);
  // var1
  EXPECT_NE(negExpression->lhs(), nullptr);
  EXPECT_EQ(negExpression->lhs()->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(negExpression->lhs()->getToken()->getValue<std::string>().c_str(), "var1");
  // ++
  auto ppExpressionStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[1]);
  EXPECT_NE(ppExpressionStmt, nullptr);
  auto ppExpression = dynamic_cast<AST::UnaryExpr *>(ppExpressionStmt->getExpression().get());
  EXPECT_NE(ppExpression, nullptr);
  EXPECT_EQ(ppExpression->getOperand(), TType::OP_UADD);
  // var2
  EXPECT_NE(ppExpression->lhs(), nullptr);
  EXPECT_EQ(ppExpression->lhs()->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(ppExpression->lhs()->getToken()->getValue<std::string>().c_str(), "var2");
  // --
  auto mmExpressionStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[2]);
  EXPECT_NE(mmExpressionStmt, nullptr);
  auto mmExpression = dynamic_cast<AST::UnaryExpr *>(mmExpressionStmt->getExpression().get());
  EXPECT_NE(mmExpression, nullptr);
  EXPECT_EQ(mmExpression->getOperand(), TType::OP_USUB);
  // var3
  EXPECT_NE(mmExpression->lhs(), nullptr);
  EXPECT_EQ(mmExpression->lhs()->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(mmExpression->lhs()->getToken()->getValue<std::string>().c_str(), "var3");
}

TEST(ParserTest, MultiplicationAndDivisionExpression) {
  constexpr auto program = R"(
  fn main() {
    var1 * var2;
    var3 / var4;
  }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);

  // fn main()
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
  EXPECT_EQ(stmtBlock->getStatements().size(), 2);
  // *
  auto multiplicationStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[0]);
  EXPECT_NE(multiplicationStmt, nullptr);
  EXPECT_EQ(multiplicationStmt->getExpression()->getToken()->getType(), TType::OP_MUL);
  // var1
  EXPECT_NE(multiplicationStmt->getExpression()->lhs(), nullptr);
  auto var1 = dynamic_cast<AST::VarExpr *>(multiplicationStmt->getExpression()->lhs().get());
  EXPECT_NE(var1, nullptr);
  EXPECT_EQ(var1->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(var1->getToken()->getValue<std::string>().c_str(), "var1");
  // var2
  EXPECT_NE(multiplicationStmt->getExpression()->rhs(), nullptr);
  auto var2 = dynamic_cast<AST::VarExpr *>(multiplicationStmt->getExpression()->rhs().get());
  EXPECT_NE(var2, nullptr);
  EXPECT_EQ(var2->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(var2->getToken()->getValue<std::string>().c_str(), "var2");
  // /
  auto divisionStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[1]);
  EXPECT_NE(divisionStmt, nullptr);
  EXPECT_EQ(divisionStmt->getExpression()->getToken()->getType(), TType::OP_DIV);
  // var3
  EXPECT_NE(divisionStmt->getExpression()->lhs(), nullptr);
  auto var3 = dynamic_cast<AST::VarExpr *>(divisionStmt->getExpression()->lhs().get());
  EXPECT_NE(var3, nullptr);
  EXPECT_EQ(var3->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(var3->getToken()->getValue<std::string>().c_str(), "var3");
  // var4
  EXPECT_NE(divisionStmt->getExpression()->rhs(), nullptr);
  auto var4 = dynamic_cast<AST::VarExpr *>(divisionStmt->getExpression()->rhs().get());
  EXPECT_NE(var4, nullptr);
  EXPECT_EQ(var4->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(var4->getToken()->getValue<std::string>().c_str(), "var4");
}

TEST(ParserTest, IOStatements) {
  constexpr auto program = R"(
  fn main() {
    print(var1, var2);
    read(var3, var4);
  }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);

  // fn main()
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
  EXPECT_EQ(stmtBlock->getStatements().size(), 2);
  // print(var1, var2)
  auto printStmt = dynamic_cast<AST::WriteStmt *>(&*stmtBlock->getStatements()[0]);
  EXPECT_NE(printStmt, nullptr);
  EXPECT_EQ(printStmt->getExpressions().size(), 2);
  constexpr std::array<std::string_view, 2> printVariables{"var1", "var2"};
  for (size_t i = 0; i < printStmt->getExpressions().size(); i++) {
    const auto &expr = printStmt->getExpressions()[i];
    EXPECT_EQ(expr->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(expr->getToken()->getValue<std::string>().c_str(), printVariables[i].data());
  }
  // read(var3, var4)
  auto readStmt = dynamic_cast<AST::ReadStmt *>(&*stmtBlock->getStatements()[1]);
  EXPECT_NE(readStmt, nullptr);
  EXPECT_EQ(readStmt->getVariableList().size(), 2);
  constexpr std::array<std::string_view, 2> readVariables{"var3", "var4"};
  for (size_t i = 0; i < readStmt->getVariableList().size(); i++) {
    const auto &var = readStmt->getVariableList()[i];
    EXPECT_EQ(var->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(var->getToken()->getValue<std::string>().c_str(), readVariables[i].data());
  }
}

TEST(ParserTest, FunctionAndMehtodCalls) {
  constexpr auto program = R"(
  fn main() {
    # function call
    foo(1, 2);
    bar{3, 4};
    # method call
    obj.foo(5, 6);
    obj->bar(7, 8);
  }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);

  // fn main()
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
  EXPECT_EQ(stmtBlock->getStatements().size(), 4);
  {
    auto fnCallStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[0]);
    EXPECT_NE(fnCallStmt, nullptr);
    auto fnCallExpr = dynamic_cast<AST::FnCallExpr *>(fnCallStmt->getExpression().get());
    EXPECT_NE(fnCallExpr, nullptr);
    // foo
    EXPECT_EQ(fnCallExpr->getVariable()->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(fnCallExpr->getVariable()->getToken()->getValue<std::string>().c_str(), "foo");
    EXPECT_EQ(fnCallExpr->getFunctionName()->getType(), TType::IDENT);
    EXPECT_STREQ(fnCallExpr->getFunctionName()->getValue<std::string>().c_str(), "foo");
    // (1, 2)
    constexpr std::array<size_t, 2> arguments{1, 2};
    for (size_t i = 0; i < fnCallExpr->getArguments().size(); i++) {
      const auto &arg = fnCallExpr->getArguments()[i];
      EXPECT_EQ(arg->getToken()->getType(), TType::LIT_INT);
      EXPECT_EQ(arg->getToken()->getValue<int>(), arguments[i]);
    }
  }
  {
    auto fnCallStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[1]);
    EXPECT_NE(fnCallStmt, nullptr);
    auto fnCallExpr = dynamic_cast<AST::FnCallExpr *>(fnCallStmt->getExpression().get());
    EXPECT_NE(fnCallExpr, nullptr);
    // bar
    EXPECT_EQ(fnCallExpr->getVariable()->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(fnCallExpr->getVariable()->getToken()->getValue<std::string>().c_str(), "bar");
    EXPECT_EQ(fnCallExpr->getFunctionName()->getType(), TType::IDENT);
    EXPECT_STREQ(fnCallExpr->getFunctionName()->getValue<std::string>().c_str(), "bar");
    // (3, 4)
    constexpr std::array<size_t, 2> arguments{3, 4};
    for (size_t i = 0; i < fnCallExpr->getArguments().size(); i++) {
      const auto &arg = fnCallExpr->getArguments()[i];
      EXPECT_EQ(arg->getToken()->getType(), TType::LIT_INT);
      EXPECT_EQ(arg->getToken()->getValue<int>(), arguments[i]);
    }
  }
  {
    auto methodCallStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[2]);
    EXPECT_NE(methodCallStmt, nullptr);
    auto methodCallExpr = dynamic_cast<AST::FnCallExpr *>(methodCallStmt->getExpression().get());
    EXPECT_NE(methodCallExpr, nullptr);
    // obj.foo
    EXPECT_EQ(methodCallExpr->getVariable()->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(methodCallExpr->getVariable()->getToken()->getValue<std::string>().c_str(), "obj::foo");
    EXPECT_EQ(methodCallExpr->getFunctionName()->getType(), TType::IDENT);
    EXPECT_STREQ(methodCallExpr->getFunctionName()->getValue<std::string>().c_str(), "obj::foo");
    EXPECT_EQ(methodCallExpr->getClassName()->getType(), TType::IDENT);
    EXPECT_STREQ(methodCallExpr->getClassName()->getValue<std::string>().c_str(), "obj");
    // (5, 6)
    constexpr std::array<size_t, 2> arguments{5, 6};
    for (size_t i = 0; i < methodCallExpr->getArguments().size(); i++) {
      const auto &arg = methodCallExpr->getArguments()[i];
      EXPECT_EQ(arg->getToken()->getType(), TType::LIT_INT);
      EXPECT_EQ(arg->getToken()->getValue<int>(), arguments[i]);
    }
  }
  {
    auto methodCallStmt = dynamic_cast<AST::ExprStmt *>(&*stmtBlock->getStatements()[3]);
    EXPECT_NE(methodCallStmt, nullptr);
    auto methodCallExpr = dynamic_cast<AST::FnCallExpr *>(methodCallStmt->getExpression().get());
    EXPECT_NE(methodCallExpr, nullptr);
    // obj->bar
    EXPECT_EQ(methodCallExpr->getVariable()->getToken()->getType(), TType::IDENT);
    EXPECT_STREQ(methodCallExpr->getVariable()->getToken()->getValue<std::string>().c_str(), "obj::bar");
    EXPECT_EQ(methodCallExpr->getFunctionName()->getType(), TType::IDENT);
    EXPECT_STREQ(methodCallExpr->getFunctionName()->getValue<std::string>().c_str(), "obj::bar");
    EXPECT_EQ(methodCallExpr->getClassName()->getType(), TType::IDENT);
    EXPECT_STREQ(methodCallExpr->getClassName()->getValue<std::string>().c_str(), "obj");
    // (7, 8)
    constexpr std::array<size_t, 2> arguments{7, 8};
    for (size_t i = 0; i < methodCallExpr->getArguments().size(); i++) {
      const auto &arg = methodCallExpr->getArguments()[i];
      EXPECT_EQ(arg->getToken()->getType(), TType::LIT_INT);
      EXPECT_EQ(arg->getToken()->getValue<int>(), arguments[i]);
    }
  }
}

TEST(ParserTest, FloatVariables) {
  constexpr auto program = R"(
  fn main() {
    float var1 = 123.45;
    float var2 {67.89};
    float var3;
  }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 1);

  // fn main()
  auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[0]);
  EXPECT_NE(fn, nullptr);
  EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
  EXPECT_EQ(fn->getParameters().size(), 0);
  auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVariable());
  EXPECT_NE(fnVar, nullptr);
  EXPECT_EQ(fnVar->getType()->getType(), TType::KW_VOID);
  auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->getBody());
  EXPECT_NE(stmtBlock, nullptr);
  EXPECT_EQ(stmtBlock->getStatements().size(), 3);
  {
    // float var1 = 123.45
    auto floatDeclStmt = dynamic_cast<AST::VarDeclStmt *>(&*stmtBlock->getStatements()[0]);
    EXPECT_NE(floatDeclStmt, nullptr);
    // float
    auto floatVar = dynamic_cast<AST::VarExpr *>(&*floatDeclStmt->getVariable());
    EXPECT_NE(floatVar, nullptr);
    EXPECT_EQ(floatVar->getType()->getType(), TType::KW_FLOAT);
    // var1
    EXPECT_EQ(floatVar->getToken()->getType(), TType::IDENT_FLOAT);
    EXPECT_STREQ(floatVar->getToken()->getValue<std::string>().c_str(), "var1");
    // 123.45
    auto floatVarConstExpr = dynamic_cast<AST::FloatExpr *>(&*floatDeclStmt->getValue());
    EXPECT_NE(floatVarConstExpr, nullptr);
    EXPECT_EQ(floatVarConstExpr->getToken()->getType(), TType::LIT_FLT);
    EXPECT_EQ(floatVarConstExpr->getToken()->getValue<float>(), 123.45f);
  }
  {
    // float var2 {67.89}
    auto floatDeclStmt = dynamic_cast<AST::VarDeclStmt *>(&*stmtBlock->getStatements()[1]);
    EXPECT_NE(floatDeclStmt, nullptr);
    // float
    auto floatVar = dynamic_cast<AST::VarExpr *>(&*floatDeclStmt->getVariable());
    EXPECT_NE(floatVar, nullptr);
    EXPECT_EQ(floatVar->getType()->getType(), TType::KW_FLOAT);
    // var2
    EXPECT_EQ(floatVar->getToken()->getType(), TType::IDENT_FLOAT);
    EXPECT_STREQ(floatVar->getToken()->getValue<std::string>().c_str(), "var2");
    // 67.89
    auto floatVarConstExpr = dynamic_cast<AST::FloatExpr *>(&*floatDeclStmt->getValue());
    EXPECT_NE(floatVarConstExpr, nullptr);
    EXPECT_EQ(floatVarConstExpr->getToken()->getType(), TType::LIT_FLT);
    EXPECT_EQ(floatVarConstExpr->getToken()->getValue<float>(), 67.89f);
  }
  {
    // float var3
    auto floatDeclStmt = dynamic_cast<AST::VarDeclStmt *>(&*stmtBlock->getStatements()[2]);
    EXPECT_NE(floatDeclStmt, nullptr);
    // float
    auto floatVar = dynamic_cast<AST::VarExpr *>(&*floatDeclStmt->getVariable());
    EXPECT_NE(floatVar, nullptr);
    EXPECT_EQ(floatVar->getType()->getType(), TType::KW_FLOAT);
    // var3
    EXPECT_EQ(floatVar->getToken()->getType(), TType::IDENT_FLOAT);
    EXPECT_STREQ(floatVar->getToken()->getValue<std::string>().c_str(), "var3");
    // 0.0
    auto floatVarConstExpr = dynamic_cast<AST::FloatExpr *>(&*floatDeclStmt->getValue());
    EXPECT_NE(floatVarConstExpr, nullptr);
    EXPECT_EQ(floatVarConstExpr->getToken()->getType(), TType::LIT_FLT);
    EXPECT_EQ(floatVarConstExpr->getToken()->getValue<float>(), 0.0f);
  }
}

TEST(ParserTest, ClassDefinition) {
  constexpr auto program = R"(
  class Main {
    def Main(var1: int) {}
    rem Main {}
    int m_var;
  }
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  Parser parser{lexer};
  const auto &root = parser.parse();

  EXPECT_EQ(root->getGlobalClasses().size(), 1);

  // class Main
  auto klass = dynamic_cast<AST::ClassDef *>(&*root->getGlobalClasses()[0]);
  EXPECT_NE(klass, nullptr);
  EXPECT_STREQ(klass->getToken()->getValue<std::string>().c_str(), "Main");
  auto klassVar = dynamic_cast<AST::VarExpr *>(&*klass->getVariable());
  EXPECT_NE(klassVar, nullptr);
  EXPECT_EQ(klassVar->getType()->getType(), TType::KW_CLASS);
  // def Main
  auto constructor = dynamic_cast<AST::CtorDef *>(klass->getConstructor().get());
  EXPECT_NE(constructor, nullptr);
  EXPECT_EQ(constructor->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(constructor->getToken()->getValue<std::string>().c_str(), "Main");
  // (var1: int)
  EXPECT_EQ(constructor->getParameters().size(), 1);
  auto param = dynamic_cast<AST::Param *>(constructor->getParameters()[0].get());
  EXPECT_NE(param, nullptr);
  EXPECT_EQ(param->getVariable()->getToken()->getType(), TType::IDENT_INT);
  EXPECT_STREQ(param->getVariable()->getToken()->getValue<std::string>().c_str(), "var1");
  // rem Main
  auto destructor = dynamic_cast<AST::DtorDef *>(klass->getDestructor().get());
  EXPECT_NE(destructor, nullptr);
  EXPECT_EQ(destructor->getToken()->getType(), TType::IDENT);
  EXPECT_STREQ(destructor->getToken()->getValue<std::string>().c_str(), "Main");
  // int m_var
  EXPECT_EQ(klass->getFields().size(), 1);
  auto field = dynamic_cast<AST::Field *>(klass->getFields()[0].get());
  EXPECT_NE(field, nullptr);
  EXPECT_EQ(field->getVariable()->getToken()->getType(), TType::IDENT_INT);
  EXPECT_STREQ(field->getVariable()->getToken()->getValue<std::string>().c_str(), "m_var");
  EXPECT_EQ(field->getValue(), nullptr);
}
