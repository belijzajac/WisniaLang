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
    EXPECT_EQ(fn->retType_->type_, TType::KW_VOID);
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
    EXPECT_EQ(fn->retType_->type_, TType::KW_INT);
    // argc : int
    EXPECT_STREQ(fn->params_[0]->value_->token_->getValue<std::string>().c_str(), "argc");
    EXPECT_EQ(fn->params_[0]->type_->token_->getType(), TType::KW_INT);
    // argv : string
    EXPECT_STREQ(fn->params_[1]->value_->token_->getValue<std::string>().c_str(), "argv");
    EXPECT_EQ(fn->params_[1]->type_->token_->getType(), TType::KW_STRING);
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
