#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

using namespace Wisnia;
using namespace Basic;

TEST(ParserTest, Conditionals) {
  std::string program = R"(
  fn conditionals () -> void {
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
  }
  )";
  std::istringstream iss{program};

  auto lexer = std::make_unique<Lexer>(iss);
  auto parser = std::make_unique<Parser>(*lexer);
  auto root = parser->parse();

  EXPECT_EQ(root->m_globalFunctions.size(), 1);
  // fn empty () -> void {}
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->m_globalFunctions[0])) {
    EXPECT_STREQ(fn->m_token->getValue<std::string>().c_str(), "conditionals");
    EXPECT_EQ(fn->m_params.size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->m_var);
    EXPECT_EQ(fnVar->m_type->m_type, TType::KW_VOID);
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->m_body);
    EXPECT_EQ(stmtBlock->m_statements.size(), 1);
    // if (true)
    auto ifStmt = dynamic_cast<AST::IfStmt *>(&*stmtBlock->m_statements[0]);
    EXPECT_NE(ifStmt, nullptr);
    EXPECT_EQ(ifStmt->m_token->getType(), TType::KW_IF);
    EXPECT_EQ(ifStmt->m_elseStmts.size(), 3);
    // true
    auto ifCond = dynamic_cast<AST::BoolExpr *>(&*ifStmt->m_condition);
    EXPECT_NE(ifCond, nullptr);
    EXPECT_EQ(ifCond->m_token->getType(), TType::KW_TRUE);
    EXPECT_EQ(ifCond->m_token->getValue<bool>(), true);
    // { f(5 - 1, 5); }
    auto ifBody = dynamic_cast<AST::StmtBlock *>(&*ifStmt->m_body);
    EXPECT_NE(ifBody, nullptr);
    EXPECT_EQ(ifBody->m_statements.size(), 1);
    // f
    auto ifExprStmt = dynamic_cast<AST::ExprStmt *>(&*ifBody->m_statements[0]);
    EXPECT_NE(ifExprStmt, nullptr);
    auto ifFnCall = dynamic_cast<AST::FnCallExpr *>(&*ifExprStmt->m_expr);
    EXPECT_NE(ifFnCall, nullptr);
    EXPECT_EQ(ifFnCall->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(ifFnCall->m_token->getValue<std::string>().c_str(), "f");
    EXPECT_EQ(ifFnCall->m_args.size(), 2);
    // (5 - 1,
    auto ifFnCallSubArg = dynamic_cast<AST::AddExpr *>(&*ifFnCall->m_args[0]);
    EXPECT_NE(ifFnCallSubArg, nullptr);
    EXPECT_EQ(ifFnCallSubArg->m_operand, TType::OP_SUB);
    // 5
    auto ifFnCallSubArgFive = dynamic_cast<AST::IntExpr *>(&*ifFnCallSubArg->lhs());
    EXPECT_NE(ifFnCallSubArgFive, nullptr);
    EXPECT_EQ(ifFnCallSubArgFive->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(ifFnCallSubArgFive->m_token->getValue<int>(), 5);
    // 1
    auto ifFnCallSubArgOne = dynamic_cast<AST::IntExpr *>(&*ifFnCallSubArg->rhs());
    EXPECT_NE(ifFnCallSubArgOne, nullptr);
    EXPECT_EQ(ifFnCallSubArgOne->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(ifFnCallSubArgOne->m_token->getValue<int>(), 1);
    // 5);
    auto ifFnCallIntArg = dynamic_cast<AST::IntExpr *>(&*ifFnCall->m_args[1]);
    EXPECT_NE(ifFnCallIntArg, nullptr);
    EXPECT_EQ(ifFnCallIntArg->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(ifFnCallIntArg->m_token->getValue<int>(), 5);
    // elif (3 == "3")
    auto elifStmt = dynamic_cast<AST::ElseIfStmt *>(&*ifStmt->m_elseStmts[0]);
    EXPECT_NE(elifStmt, nullptr);
    EXPECT_EQ(elifStmt->m_token->getType(), TType::KW_ELIF);
    // (3 == "3")
    auto elifCondExpr = dynamic_cast<AST::EqExpr *>(&*elifStmt->m_condition);
    EXPECT_NE(elifCondExpr, nullptr);
    EXPECT_EQ(elifCondExpr->m_operand, TType::OP_EQ);
    // 3
    auto elifCondExprThree = dynamic_cast<AST::IntExpr *>(&*elifCondExpr->lhs());
    EXPECT_NE(elifCondExprThree, nullptr);
    EXPECT_EQ(elifCondExprThree->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(elifCondExprThree->m_token->getValue<int>(), 3);
    // "3"
    auto elifCondExprStrThree = dynamic_cast<AST::StringExpr *>(&*elifCondExpr->rhs());
    EXPECT_NE(elifCondExprStrThree, nullptr);
    EXPECT_EQ(elifCondExprStrThree->m_token->getType(), TType::LIT_STR);
    EXPECT_STREQ(elifCondExprStrThree->m_token->getValue<std::string>().c_str(), "3");
    // { f(0); }
    auto elifBody = dynamic_cast<AST::StmtBlock *>(&*elifStmt->m_body);
    EXPECT_NE(elifBody, nullptr);
    EXPECT_EQ(elifBody->m_statements.size(), 1);
    // f
    auto elifExprStmt = dynamic_cast<AST::ExprStmt *>(&*elifBody->m_statements[0]);
    EXPECT_NE(elifExprStmt, nullptr);
    auto elifFnCall = dynamic_cast<AST::FnCallExpr *>(&*elifExprStmt->m_expr);
    EXPECT_NE(elifFnCall, nullptr);
    EXPECT_EQ(elifFnCall->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(elifFnCall->m_token->getValue<std::string>().c_str(), "f");
    EXPECT_EQ(elifFnCall->m_args.size(), 1);
    // 0
    auto elifFnCallArg = dynamic_cast<AST::IntExpr *>(&*elifFnCall->m_args[0]);
    EXPECT_NE(elifFnCallArg, nullptr);
    EXPECT_EQ(elifFnCallArg->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(elifFnCallArg->m_token->getValue<int>(), 0);
    // elif (abc)
    auto elifSecondStmt = dynamic_cast<AST::ElseIfStmt *>(&*ifStmt->m_elseStmts[1]);
    EXPECT_NE(elifSecondStmt, nullptr);
    EXPECT_EQ(elifSecondStmt->m_token->getType(), TType::KW_ELIF);
    // (abc)
    auto elifSecondCondExpr = dynamic_cast<AST::VarExpr *>(&*elifSecondStmt->m_condition);
    EXPECT_NE(elifSecondCondExpr, nullptr);
    EXPECT_EQ(elifSecondCondExpr->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(elifSecondCondExpr->m_token->getValue<std::string>().c_str(), "abc");
    // { continue; }
    auto elifSecondBody = dynamic_cast<AST::StmtBlock *>(&*elifSecondStmt->m_body);
    EXPECT_NE(elifSecondBody, nullptr);
    EXPECT_EQ(elifSecondBody->m_statements.size(), 1);
    auto continueExprStmt = dynamic_cast<AST::ContinueStmt *>(&*elifSecondBody->m_statements[0]);
    EXPECT_NE(continueExprStmt, nullptr);
    EXPECT_EQ(continueExprStmt->m_token->getType(), TType::KW_CONTINUE);
    // else
    auto elseStmt = dynamic_cast<AST::ElseStmt *>(&*ifStmt->m_elseStmts[2]);
    EXPECT_NE(elseStmt, nullptr);
    EXPECT_EQ(elseStmt->m_token->getType(), TType::KW_ELSE);
    // { break; }
    auto elseBody = dynamic_cast<AST::StmtBlock *>(&*elseStmt->m_body);
    EXPECT_NE(elseBody, nullptr);
    EXPECT_EQ(elseBody->m_statements.size(), 1);
    auto breakExprStmt = dynamic_cast<AST::BreakStmt *>(&*elseBody->m_statements[0]);
    EXPECT_NE(breakExprStmt, nullptr);
    EXPECT_EQ(breakExprStmt->m_token->getType(), TType::KW_BREAK);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
