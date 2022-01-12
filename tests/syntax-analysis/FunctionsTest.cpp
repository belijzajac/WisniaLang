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

  EXPECT_EQ(root->m_globalFunctions.size(), 2);
  // fn empty () -> void {}
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->m_globalFunctions[0])) {
    EXPECT_STREQ(fn->m_token->getValue<std::string>().c_str(), "empty");
    EXPECT_EQ(fn->m_params.size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->m_var);
    EXPECT_EQ(fnVar->m_type->m_type, TType::KW_VOID);
    // {}
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->m_body);
    EXPECT_EQ(stmtBlock->m_statements.size(), 0);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
  // fn main (argc : int, argv : string) -> int { return 5; }
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->m_globalFunctions[1])) {
    EXPECT_STREQ(fn->m_token->getValue<std::string>().c_str(), "main");
    EXPECT_EQ(fn->m_params.size(), 2);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->m_var);
    EXPECT_EQ(fnVar->m_type->m_type, TType::KW_INT);
    // argc : int
    auto param0Var = dynamic_cast<AST::VarExpr *>(&*fn->m_params[0]->m_var);
    EXPECT_STREQ(param0Var->m_token->getValue<std::string>().c_str(), "argc");
    EXPECT_EQ(param0Var->m_type->m_type, TType::KW_INT);
    // argv : string
    auto param1Var = dynamic_cast<AST::VarExpr *>(&*fn->m_params[1]->m_var);
    EXPECT_STREQ(param1Var->m_token->getValue<std::string>().c_str(), "argv");
    EXPECT_EQ(param1Var->m_type->m_type, TType::KW_STRING);
    // { return 5; }
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->m_body);
    EXPECT_EQ(stmtBlock->m_statements.size(), 1);
    auto returnStmt = dynamic_cast<AST::ReturnStmt *>(&*stmtBlock->m_statements[0]);
    EXPECT_NE(returnStmt, nullptr);
    EXPECT_EQ(returnStmt->m_returnValue->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(returnStmt->m_returnValue->m_token->getValue<int>(), 5);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
