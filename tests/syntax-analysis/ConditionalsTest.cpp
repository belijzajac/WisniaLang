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
  const auto &root = parser->parse();

  EXPECT_EQ(root->globalFnDefs_.size(), 1);
  // fn empty () -> void {}
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->globalFnDefs_[0])) {
    EXPECT_STREQ(fn->token_->getValue<std::string>().c_str(), "conditionals");
    EXPECT_EQ(fn->params_.size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->var_);
    EXPECT_EQ(fnVar->type_->type_, TType::KW_VOID);
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->body_);
    EXPECT_EQ(stmtBlock->stmts_.size(), 1);
    // if (true)
    auto ifStmt = dynamic_cast<AST::IfStmt *>(&*stmtBlock->stmts_[0]);
    EXPECT_NE(ifStmt, nullptr);
    EXPECT_EQ(ifStmt->token_->getType(), TType::KW_IF);
    EXPECT_EQ(ifStmt->elseBlcks_.size(), 3);
    // true
    auto ifCond = dynamic_cast<AST::BoolExpr *>(&*ifStmt->cond_);
    EXPECT_NE(ifCond, nullptr);
    EXPECT_EQ(ifCond->token_->getType(), TType::KW_TRUE);
    EXPECT_EQ(ifCond->token_->getValue<bool>(), true);
    // { f(5 - 1, 5); }
    auto ifBody = dynamic_cast<AST::StmtBlock *>(&*ifStmt->body_);
    EXPECT_NE(ifBody, nullptr);
    EXPECT_EQ(ifBody->stmts_.size(), 1);
    // f
    auto ifExprStmt = dynamic_cast<AST::ExprStmt *>(&*ifBody->stmts_[0]);
    EXPECT_NE(ifExprStmt, nullptr);
    auto ifFnCall = dynamic_cast<AST::FnCallExpr *>(&*ifExprStmt->expr_);
    EXPECT_NE(ifFnCall, nullptr);
    EXPECT_EQ(ifFnCall->token_->getType(), TType::IDENT);
    EXPECT_STREQ(ifFnCall->token_->getValue<std::string>().c_str(), "f");
    EXPECT_EQ(ifFnCall->args_.size(), 2);
    // (5 - 1,
    auto ifFnCallSubArg = dynamic_cast<AST::AddExpr *>(&*ifFnCall->args_[0]);
    EXPECT_NE(ifFnCallSubArg, nullptr);
    EXPECT_EQ(ifFnCallSubArg->op_, TType::OP_SUB);
    // 5
    auto ifFnCallSubArgFive = dynamic_cast<AST::IntExpr *>(&*ifFnCallSubArg->lhs());
    EXPECT_NE(ifFnCallSubArgFive, nullptr);
    EXPECT_EQ(ifFnCallSubArgFive->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(ifFnCallSubArgFive->token_->getValue<int>(), 5);
    // 1
    auto ifFnCallSubArgOne = dynamic_cast<AST::IntExpr *>(&*ifFnCallSubArg->rhs());
    EXPECT_NE(ifFnCallSubArgOne, nullptr);
    EXPECT_EQ(ifFnCallSubArgOne->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(ifFnCallSubArgOne->token_->getValue<int>(), 1);
    // 5);
    auto ifFnCallIntArg = dynamic_cast<AST::IntExpr *>(&*ifFnCall->args_[1]);
    EXPECT_NE(ifFnCallIntArg, nullptr);
    EXPECT_EQ(ifFnCallIntArg->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(ifFnCallIntArg->token_->getValue<int>(), 5);
    // elif (3 == "3")
    auto elifStmt = dynamic_cast<AST::ElseIfStmt *>(&*ifStmt->elseBlcks_[0]);
    EXPECT_NE(elifStmt, nullptr);
    EXPECT_EQ(elifStmt->token_->getType(), TType::KW_ELIF);
    // (3 == "3")
    auto elifCondExpr = dynamic_cast<AST::EqExpr *>(&*elifStmt->cond_);
    EXPECT_NE(elifCondExpr, nullptr);
    EXPECT_EQ(elifCondExpr->op_, TType::OP_EQ);
    // 3
    auto elifCondExprThree = dynamic_cast<AST::IntExpr *>(&*elifCondExpr->lhs());
    EXPECT_NE(elifCondExprThree, nullptr);
    EXPECT_EQ(elifCondExprThree->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(elifCondExprThree->token_->getValue<int>(), 3);
    // "3"
    auto elifCondExprStrThree = dynamic_cast<AST::StringExpr *>(&*elifCondExpr->rhs());
    EXPECT_NE(elifCondExprStrThree, nullptr);
    EXPECT_EQ(elifCondExprStrThree->token_->getType(), TType::LIT_STR);
    EXPECT_STREQ(elifCondExprStrThree->token_->getValue<std::string>().c_str(), "3");
    // { f(0); }
    auto elifBody = dynamic_cast<AST::StmtBlock *>(&*elifStmt->body_);
    EXPECT_NE(elifBody, nullptr);
    EXPECT_EQ(elifBody->stmts_.size(), 1);
    // f
    auto elifExprStmt = dynamic_cast<AST::ExprStmt *>(&*elifBody->stmts_[0]);
    EXPECT_NE(elifExprStmt, nullptr);
    auto elifFnCall = dynamic_cast<AST::FnCallExpr *>(&*elifExprStmt->expr_);
    EXPECT_NE(elifFnCall, nullptr);
    EXPECT_EQ(elifFnCall->token_->getType(), TType::IDENT);
    EXPECT_STREQ(elifFnCall->token_->getValue<std::string>().c_str(), "f");
    EXPECT_EQ(elifFnCall->args_.size(), 1);
    // 0
    auto elifFnCallArg = dynamic_cast<AST::IntExpr *>(&*elifFnCall->args_[0]);
    EXPECT_NE(elifFnCallArg, nullptr);
    EXPECT_EQ(elifFnCallArg->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(elifFnCallArg->token_->getValue<int>(), 0);
    // elif (abc)
    auto elifSecondStmt = dynamic_cast<AST::ElseIfStmt *>(&*ifStmt->elseBlcks_[1]);
    EXPECT_NE(elifSecondStmt, nullptr);
    EXPECT_EQ(elifSecondStmt->token_->getType(), TType::KW_ELIF);
    // (abc)
    auto elifSecondCondExpr = dynamic_cast<AST::VarExpr *>(&*elifSecondStmt->cond_);
    EXPECT_NE(elifSecondCondExpr, nullptr);
    EXPECT_EQ(elifSecondCondExpr->token_->getType(), TType::IDENT);
    EXPECT_STREQ(elifSecondCondExpr->token_->getValue<std::string>().c_str(), "abc");
    // { continue; }
    auto elifSecondBody = dynamic_cast<AST::StmtBlock *>(&*elifSecondStmt->body_);
    EXPECT_NE(elifSecondBody, nullptr);
    EXPECT_EQ(elifSecondBody->stmts_.size(), 1);
    auto continueExprStmt = dynamic_cast<AST::BreakStmt *>(&*elifSecondBody->stmts_[0]);
    EXPECT_NE(continueExprStmt, nullptr);
    EXPECT_EQ(continueExprStmt->token_->getType(), TType::KW_CONTINUE);
    // else
    auto elseStmt = dynamic_cast<AST::ElseStmt *>(&*ifStmt->elseBlcks_[2]);
    EXPECT_NE(elseStmt, nullptr);
    EXPECT_EQ(elseStmt->token_->getType(), TType::KW_ELSE);
    // { break; }
    auto elseBody = dynamic_cast<AST::StmtBlock *>(&*elseStmt->body_);
    EXPECT_NE(elseBody, nullptr);
    EXPECT_EQ(elseBody->stmts_.size(), 1);
    auto breakExprStmt = dynamic_cast<AST::BreakStmt *>(&*elseBody->stmts_[0]);
    EXPECT_NE(breakExprStmt, nullptr);
    EXPECT_EQ(breakExprStmt->token_->getType(), TType::KW_BREAK);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
