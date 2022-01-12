#include <gtest/gtest.h>
// Wisnia
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"

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
  auto root = parser->parse();

  EXPECT_EQ(root->m_globalFunctions.size(), 1);
  // fn loops () -> void
  if (auto fn = dynamic_cast<AST::FnDef *>(&*root->m_globalFunctions[0])) {
    EXPECT_STREQ(fn->m_token->getValue<std::string>().c_str(), "loops");
    EXPECT_EQ(fn->m_params.size(), 0);
    auto fnVar = dynamic_cast<AST::VarExpr *>(&*fn->m_var);
    EXPECT_EQ(fnVar->m_type->m_type, TType::KW_VOID);
    auto stmtBlock = dynamic_cast<AST::StmtBlock *>(&*fn->m_body);
    EXPECT_EQ(stmtBlock->m_statements.size(), 3);
    // while (i < 5) {}
    auto whileLoopStmt = dynamic_cast<AST::WhileLoop *>(&*stmtBlock->m_statements[0]);
    EXPECT_NE(whileLoopStmt, nullptr);
    EXPECT_EQ(whileLoopStmt->m_token->getType(), TType::KW_WHILE);
    // i < 5
    auto whileLoopCondition = dynamic_cast<AST::CompExpr *>(&*whileLoopStmt->m_cond);
    EXPECT_NE(whileLoopCondition, nullptr);
    EXPECT_EQ(whileLoopCondition->m_operand, TType::OP_L);
    auto whileLoopConditionVar = dynamic_cast<AST::VarExpr *>(&*whileLoopCondition->lhs());
    EXPECT_NE(whileLoopConditionVar, nullptr);
    EXPECT_EQ(whileLoopConditionVar->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(whileLoopConditionVar->m_token->getValue<std::string>().c_str(), "i");
    // 5
    auto whileLoopConditionIntConst = dynamic_cast<AST::IntExpr *>(&*whileLoopCondition->rhs());
    EXPECT_NE(whileLoopConditionIntConst, nullptr);
    EXPECT_EQ(whileLoopConditionIntConst->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(whileLoopConditionIntConst->m_token->getValue<int>(), 5);
    // {}
    auto whileLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*whileLoopStmt->m_body);
    EXPECT_EQ(whileLoopStmtBlock->m_statements.size(), 0);
    // for (int i = 0; i < 5; i = i + 1) {}
    auto forLoopStmt = dynamic_cast<AST::ForLoop *>(&*stmtBlock->m_statements[1]);
    EXPECT_NE(forLoopStmt, nullptr);
    EXPECT_EQ(forLoopStmt->m_token->getType(), TType::KW_FOR);
    // int i = 0;
    auto forLoopInit = dynamic_cast<AST::VarDeclStmt *>(&*forLoopStmt->m_initialization);
    EXPECT_NE(forLoopInit, nullptr);
    // int
    auto forLoopInitVar = dynamic_cast<AST::VarExpr *>(&*forLoopInit->m_var);
    EXPECT_EQ(forLoopInitVar->m_type->m_type, TType::KW_INT);
    // i
    EXPECT_EQ(forLoopInitVar->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopInitVar->m_token->getValue<std::string>().c_str(), "i");
    // 0
    auto forLoopInitConstExpr = dynamic_cast<AST::IntExpr *>(&*forLoopInit->m_value);
    EXPECT_NE(forLoopInitConstExpr, nullptr);
    EXPECT_EQ(forLoopInitConstExpr->m_token->getType(), TType::LIT_INT);
    EXPECT_EQ(forLoopInitConstExpr->m_token->getValue<int>(), 0);
    // six <= 6.59;
    auto forLoopComparisonExpr = dynamic_cast<AST::CompExpr *>(&*forLoopStmt->m_condition);
    EXPECT_NE(forLoopComparisonExpr, nullptr);
    EXPECT_EQ(forLoopComparisonExpr->m_operand, TType::OP_LE);
    auto forLoopComparisonVar = dynamic_cast<AST::VarExpr *>(&*forLoopComparisonExpr->lhs());
    EXPECT_NE(forLoopComparisonVar, nullptr);
    EXPECT_EQ(forLoopComparisonVar->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopComparisonVar->m_token->getValue<std::string>().c_str(), "six");
    // 6.59
    auto forLoopComparisonFltConst = dynamic_cast<AST::FloatExpr *>(&*forLoopComparisonExpr->rhs());
    EXPECT_NE(forLoopComparisonFltConst, nullptr);
    EXPECT_EQ(forLoopComparisonFltConst->m_token->getType(), TType::LIT_FLT);
    EXPECT_EQ(forLoopComparisonFltConst->m_token->getValue<float>(), 6.59f);
    // i = i + "1"
    auto forLoopIncDec = dynamic_cast<AST::VarAssignStmt *>(&*forLoopStmt->m_increment);
    EXPECT_NE(forLoopIncDec, nullptr);
    // i
    auto forLoopIncDecVar = dynamic_cast<AST::VarExpr *>(&*forLoopIncDec->m_var);
    EXPECT_EQ(forLoopIncDecVar->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopIncDecVar->m_token->getValue<std::string>().c_str(), "i");
    // i + "1"
    auto forLoopIncDecAddExpr = dynamic_cast<AST::AddExpr *>(&*forLoopIncDec->m_val);
    EXPECT_NE(forLoopIncDecAddExpr, nullptr);
    EXPECT_EQ(forLoopIncDecAddExpr->m_operand, TType::OP_ADD);
    auto forLoopIncDecLhs = dynamic_cast<AST::VarExpr *>(&*forLoopIncDecAddExpr->lhs());
    EXPECT_NE(forLoopIncDecLhs, nullptr);
    EXPECT_EQ(forLoopIncDecLhs->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(forLoopIncDecLhs->m_token->getValue<std::string>().c_str(), "i");
    // "1"
    auto forLoopIncDecRhs = dynamic_cast<AST::StringExpr *>(&*forLoopIncDecAddExpr->rhs());
    EXPECT_NE(forLoopIncDecRhs, nullptr);
    EXPECT_EQ(forLoopIncDecRhs->m_token->getType(), TType::LIT_STR);
    EXPECT_EQ(forLoopIncDecRhs->m_token->getValue<std::string>(), "1");
    // {}
    auto forLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*forLoopStmt->m_body);
    EXPECT_EQ(forLoopStmtBlock->m_statements.size(), 0);
    // for_each (elem in elems) {}
    auto forEachLoopStmt = dynamic_cast<AST::ForEachLoop *>(&*stmtBlock->m_statements[2]);
    EXPECT_NE(forEachLoopStmt, nullptr);
    EXPECT_EQ(forEachLoopStmt->m_token->getType(), TType::KW_FOREACH);
    // elem
    auto forEachElem = dynamic_cast<AST::VarExpr *>(&*forEachLoopStmt->m_element);
    EXPECT_NE(forEachElem, nullptr);
    EXPECT_EQ(forEachElem->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(forEachElem->m_token->getValue<std::string>().c_str(), "elem");
    // elems
    auto forEachIterElem = dynamic_cast<AST::VarExpr *>(&*forEachLoopStmt->m_collection);
    EXPECT_NE(forEachIterElem, nullptr);
    EXPECT_EQ(forEachIterElem->m_token->getType(), TType::IDENT);
    EXPECT_STREQ(forEachIterElem->m_token->getValue<std::string>().c_str(), "elems");
    // {}
    auto forEachLoopStmtBlock = dynamic_cast<AST::StmtBlock *>(&*forEachLoopStmt->m_body);
    EXPECT_EQ(forEachLoopStmtBlock->m_statements.size(), 0);
  } else {
    FAIL() << "Invalid cast to type AST::FnDef";
  }
}
