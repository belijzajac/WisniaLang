#include <gtest/gtest.h>
// Wisnia
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"

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
  auto root = parser->parse();

  EXPECT_EQ(root->getGlobalFunctions().size(), 2);
  // fn empty () -> void {}
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
  // fn main (argc : int, argv : string) -> int { return 5; }
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->getGlobalFunctions()[1])) {
    EXPECT_STREQ(fn->getToken()->getValue<std::string>().c_str(), "main");
    EXPECT_EQ(fn->getParams().size(), 2);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->getVar());
    EXPECT_EQ(fnVar->getType()->getType(), TType::KW_INT);
    // argc : int
    auto param0Var = dynamic_cast<AST::VarExpr *>(&*fn->getParams()[0]->getVar());
    EXPECT_STREQ(param0Var->getToken()->getValue<std::string>().c_str(), "argc");
    EXPECT_EQ(param0Var->getType()->getType(), TType::KW_INT);
    // argv : string
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
