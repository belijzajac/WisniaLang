#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

using namespace Wisnia;
using namespace Basic;

TEST(ParserTest, Loops) {
  std::string program = R"(
  fn loops () -> void {
    while (i < 5) {}
    for (int i = 0; six <= 6.59; i = i + "1") {}
    for_each (elem in elems) {}
  }
  )";
  std::istringstream iss{program};

  auto lexer = std::make_unique<Lexer>(iss);
  auto parser = std::make_unique<Parser>(*lexer);
  const auto &root = parser->parse();

  EXPECT_EQ(root->globalFnDefs_.size(), 1);
  // fn loops () -> void
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->globalFnDefs_[0])) {
    EXPECT_STREQ(fn->token_->getValue<std::string>().c_str(), "loops");
    EXPECT_EQ(fn->params_.size(), 0);
    EXPECT_EQ(fn->retType_->type_, TType::KW_VOID);
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->body_);
    EXPECT_EQ(stmtBlock->stmts_.size(), 3);
    // while (i < 5) {}
    auto whileLoopStmt = dynamic_cast<AST::WhileLoop *>(&*stmtBlock->stmts_[0]);
    EXPECT_NE(whileLoopStmt, nullptr);
    EXPECT_EQ(whileLoopStmt->token_->getType(), TType::KW_WHILE);
    // i < 5
    auto whileLoopCondition = dynamic_cast<AST::CompExpr *>(&*whileLoopStmt->cond_);
    EXPECT_NE(whileLoopCondition, nullptr);
    EXPECT_EQ(whileLoopCondition->op_, TType::OP_L);
    auto whileLoopConditionVar = dynamic_cast<AST::VarExpr *>(&*whileLoopCondition->lhs());
    EXPECT_NE(whileLoopConditionVar, nullptr);
    EXPECT_EQ(whileLoopConditionVar->token_->getType(), TType::IDENT);
    EXPECT_STREQ(whileLoopConditionVar->token_->getValue<std::string>().c_str(), "i");
    // 5
    auto whileLoopConditionIntConst = dynamic_cast<AST::IntExpr *>(&*whileLoopCondition->rhs());
    EXPECT_NE(whileLoopConditionIntConst, nullptr);
    EXPECT_EQ(whileLoopConditionIntConst->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(whileLoopConditionIntConst->token_->getValue<int>(), 5);
    // {}
    auto whileLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*whileLoopStmt->body_);
    EXPECT_EQ(whileLoopStmtBlock->stmts_.size(), 0);
    // for (int i = 0; i < 5; i = i + 1) {}
    auto forLoopStmt = dynamic_cast<AST::ForLoop *>(&*stmtBlock->stmts_[1]);
    EXPECT_NE(forLoopStmt, nullptr);
    EXPECT_EQ(forLoopStmt->token_->getType(), TType::KW_FOR);
    // int i = 0;
    auto forLoopInit = dynamic_cast<AST::VarDeclStmt *>(&*forLoopStmt->init_);
    EXPECT_NE(forLoopInit, nullptr);
    // int
    EXPECT_EQ(forLoopInit->type_->type_, TType::KW_INT);
    // i
    EXPECT_EQ(forLoopInit->name_->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopInit->name_->getValue<std::string>().c_str(), "i");
    // 0
    auto forLoopInitConstExpr = dynamic_cast<AST::IntExpr *>(&*forLoopInit->value_);
    EXPECT_NE(forLoopInitConstExpr, nullptr);
    EXPECT_EQ(forLoopInitConstExpr->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(forLoopInitConstExpr->token_->getValue<int>(), 0);
    // six <= 6.59;
    auto forLoopComparisonExpr = dynamic_cast<AST::CompExpr *>(&*forLoopStmt->cond_);
    EXPECT_NE(forLoopComparisonExpr, nullptr);
    EXPECT_EQ(forLoopComparisonExpr->op_, TType::OP_LE);
    auto forLoopComparisonVar = dynamic_cast<AST::VarExpr *>(&*forLoopComparisonExpr->lhs());
    EXPECT_NE(forLoopComparisonVar, nullptr);
    EXPECT_EQ(forLoopComparisonVar->token_->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopComparisonVar->token_->getValue<std::string>().c_str(), "six");
    // 6.59
    auto forLoopComparisonFltConst = dynamic_cast<AST::FloatExpr *>(&*forLoopComparisonExpr->rhs());
    EXPECT_NE(forLoopComparisonFltConst, nullptr);
    EXPECT_EQ(forLoopComparisonFltConst->token_->getType(), TType::LIT_FLT);
    EXPECT_EQ(forLoopComparisonFltConst->token_->getValue<float>(), 6.59f);
    // i = i + "1"
    auto forLoopIncDec = dynamic_cast<AST::VarAssignStmt *>(&*forLoopStmt->incdec_);
    EXPECT_NE(forLoopIncDec, nullptr);
    // i
    EXPECT_EQ(forLoopIncDec->name_->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopIncDec->name_->getValue<std::string>().c_str(), "i");
    // i + "1"
    auto forLoopIncDecAddExpr = dynamic_cast<AST::AddExpr *>(&*forLoopIncDec->value_);
    EXPECT_NE(forLoopIncDecAddExpr, nullptr);
    EXPECT_EQ(forLoopIncDecAddExpr->op_, TType::OP_ADD);
    auto forLoopIncDecLhs = dynamic_cast<AST::VarExpr *>(&*forLoopIncDecAddExpr->lhs());
    EXPECT_NE(forLoopIncDecLhs, nullptr);
    EXPECT_EQ(forLoopIncDecLhs->token_->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopIncDecLhs->token_->getValue<std::string>().c_str(), "i");
    // "1"
    auto forLoopIncDecRhs = dynamic_cast<AST::StringExpr *>(&*forLoopIncDecAddExpr->rhs());
    EXPECT_NE(forLoopIncDecRhs, nullptr);
    EXPECT_EQ(forLoopIncDecRhs->token_->getType(), TType::LIT_STR);
    EXPECT_EQ(forLoopIncDecRhs->token_->getValue<std::string>(), "1");
    // {}
    auto forLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*forLoopStmt->body_);
    EXPECT_EQ(forLoopStmtBlock->stmts_.size(), 0);
    // for_each (elem in elems) {}
    auto forEachLoopStmt = dynamic_cast<AST::ForEachLoop *>(&*stmtBlock->stmts_[2]);
    EXPECT_NE(forEachLoopStmt, nullptr);
    EXPECT_EQ(forEachLoopStmt->token_->getType(), TType::KW_FOREACH);
    // elem
    auto forEachElem = dynamic_cast<AST::VarExpr *>(&*forEachLoopStmt->elem_);
    EXPECT_NE(forEachElem, nullptr);
    EXPECT_EQ(forEachElem->token_->getType(), TType::IDENT);
    EXPECT_STREQ(forEachElem->token_->getValue<std::string>().c_str(), "elem");
    // elems
    auto forEachIterElem = dynamic_cast<AST::VarExpr *>(&*forEachLoopStmt->iterElem_);
    EXPECT_NE(forEachIterElem, nullptr);
    EXPECT_EQ(forEachIterElem->token_->getType(), TType::IDENT);
    EXPECT_STREQ(forEachIterElem->token_->getValue<std::string>().c_str(), "elems");
    // {}
    auto forEachLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*forEachLoopStmt->body_);
    EXPECT_EQ(forEachLoopStmtBlock->stmts_.size(), 0);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
