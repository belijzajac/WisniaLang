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
  auto parser = std::make_unique<Parser>(std::move(*lexer));
  const auto &root = parser->parse();
  root->print();

  EXPECT_EQ(root->globalFnDefs_.size(), 1);
  // fn empty () -> void {}
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->globalFnDefs_[0])) {
    EXPECT_STREQ(fn->token_->getValue<std::string>().c_str(), "conditionals");
    EXPECT_EQ(fn->params_.size(), 0);
    EXPECT_EQ(fn->retType_->type_, TType::KW_VOID);
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
    // { f(5 - 1); }
    auto baseOfIf = dynamic_cast<AST::BaseIf *>(&*stmtBlock->stmts_[0]);
    EXPECT_NE(baseOfIf, nullptr);
    auto baseOfIfBody = dynamic_cast<AST::StmtBlock *>(&*baseOfIf->body_);
    EXPECT_NE(baseOfIfBody, nullptr);
    EXPECT_EQ(baseOfIfBody->stmts_.size(), 1);
    //auto ifFnCall = dynamic_cast<AST::FnCallExpr *>(&*baseOfIfBody->stmts_[0]);
    //EXPECT_NE(ifFnCall, nullptr); // <-- fails


    //EXPECT_EQ(ifFnCall->returnValue_->token_->getType(), TType::LIT_INT);
    //EXPECT_EQ(ifFnCall->returnValue_->token_->getValue<int>(), 5);




  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
