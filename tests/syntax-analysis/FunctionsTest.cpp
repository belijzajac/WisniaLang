#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

using namespace Wisnia;
using namespace Basic;

TEST(ParserTest, Functions) {
  std::string program = R"(
  fn empty () -> void {}
  fn main (argc : int, argv : string) -> int { return 5; }
  )";
  std::istringstream iss{program};

  auto lexer = std::make_unique<Lexer>(iss);
  auto parser = std::make_unique<Parser>(*lexer);
  const auto &root = parser->parse();

  EXPECT_EQ(root->globalFnDefs_.size(), 2);
  // fn empty () -> void {}
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->globalFnDefs_[0])) {
    EXPECT_STREQ(fn->token_->getValue<std::string>().c_str(), "empty");
    EXPECT_EQ(fn->params_.size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->var_);
    EXPECT_EQ(fnVar->type_->type_, TType::KW_VOID);
    // {}
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->body_);
    EXPECT_EQ(stmtBlock->stmts_.size(), 0);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
  // fn main (argc : int, argv : string) -> int { return 5; }
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->globalFnDefs_[1])) {
    EXPECT_STREQ(fn->token_->getValue<std::string>().c_str(), "main");
    EXPECT_EQ(fn->params_.size(), 2);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->var_);
    EXPECT_EQ(fnVar->type_->type_, TType::KW_INT);
    // argc : int
    auto param0Var = dynamic_cast<AST::VarExpr *>(&*fn->params_[0]->var_);
    EXPECT_STREQ(param0Var->token_->getValue<std::string>().c_str(), "argc");
    EXPECT_EQ(param0Var->type_->type_, TType::KW_INT);
    // argv : string
    auto param1Var = dynamic_cast<AST::VarExpr *>(&*fn->params_[1]->var_);
    EXPECT_STREQ(param1Var->token_->getValue<std::string>().c_str(), "argv");
    EXPECT_EQ(param1Var->type_->type_, TType::KW_STRING);
    // { return 5; }
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->body_);
    EXPECT_EQ(stmtBlock->stmts_.size(), 1);
    auto returnStmt = dynamic_cast<AST::ReturnStmt *>(&*stmtBlock->stmts_[0]);
    EXPECT_NE(returnStmt, nullptr);
    EXPECT_EQ(returnStmt->returnValue_->token_->getType(), TType::LIT_INT);
    EXPECT_EQ(returnStmt->returnValue_->token_->getValue<int>(), 5);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
