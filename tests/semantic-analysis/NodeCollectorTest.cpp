// Wisnia
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "NodeCollector.hpp"
#include "SemanticTestFixture.hpp"

using namespace Wisnia;
using NodeCollectorTest = SemanticTestFixture;

TEST_F(NodeCollectorTest, ClassDef) {
  NodeCollector<AST::ClassDef> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, FnDef) {
  NodeCollector<AST::FnDef> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, Param) {
  NodeCollector<AST::Param> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, VarExpr) {
  NodeCollector<AST::VarExpr> collector;
  m_root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 44);
  constexpr std::array<const char *, 44> kExpectedValues{
      "Foo", "is_fifteen", "number", "digit", "simple_operations", "a", "number", "i", "i", "i",
      "i", "a", "a", "a", "is_fifteen", "digit", "a", "a", "digit", "digit", "a", "digit",
      "output_hello", "do_output", "do_output", "main", "argc", "argv", "correct", "argc", "answer",
      "output_hello", "correct", "fooPtr", "Foo", "correct", "yes", "answer", "output_hello", "yes",
      "argc", "answer", "output_hello", "argc"};
  for (size_t i = 0; i < 44; i++) {
    EXPECT_TRUE(collectedNodes[i]->m_token->getValue<std::string>() == kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, VarAssignStmt) {
  NodeCollector<AST::VarAssignStmt> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 7);
}

TEST_F(NodeCollectorTest, CompExpr) {
  NodeCollector<AST::CompExpr> collector;
  m_root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 4);
  constexpr std::array<char, 4> kExpectedValues{'<', '<', '<', '>'};
  for (size_t i = 0; i < 4; i++) {
    EXPECT_TRUE(collectedNodes[i]->m_strOperand == std::string{kExpectedValues[i]});
  }
}

TEST_F(NodeCollectorTest, AddExpr) {
  NodeCollector<AST::AddExpr> collector;
  m_root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 4);
  constexpr std::array<char, 4> kExpectedValues{'+', '+', '+', '-'};
  for (size_t i = 0; i < 4; i++) {
    EXPECT_TRUE(collectedNodes[i]->m_strOperand == std::string{kExpectedValues[i]});
  }
}

TEST_F(NodeCollectorTest, MultExpr) {
  NodeCollector<AST::MultExpr> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, EqExpr) {
  NodeCollector<AST::EqExpr> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, BoolExpr) {
  NodeCollector<AST::BoolExpr> collector;
  m_root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 5);
  constexpr std::array<bool, 5> kExpectedValues{false, true, true, false, false};
  for (size_t i = 0; i < 5; i++) {
    EXPECT_EQ(collectedNodes[i]->m_token->getValue<bool>(), kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, StringExpr) {
  NodeCollector<AST::StringExpr> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
  EXPECT_TRUE(collector.getNodes()[0]->m_token->getValue<std::string>() == "hello");
  EXPECT_TRUE(collector.getNodes()[1]->m_token->getValue<std::string>() == "bye");
}

TEST_F(NodeCollectorTest, IntExpr) {
  NodeCollector<AST::IntExpr> collector;
  m_root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 10);
  constexpr std::array<int, 10> kExpectedValues{123456, 0, 5, 1, 150000, 2, 5, 5, 7, 5};
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(collectedNodes[i]->m_token->getValue<int>(), kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, FloatExpr) {
  NodeCollector<AST::FloatExpr> collector;
  m_root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 5);
  constexpr std::array<float, 5> kExpectedValues{5.0, 7.75, 1.45, 15.0, 1.33};
  for (size_t i = 0; i < 5; i++) {
    EXPECT_EQ(collectedNodes[i]->m_token->getValue<float>(), kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, BreakStmt) {
  NodeCollector<AST::BreakStmt> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ReturnStmt) {
  NodeCollector<AST::ReturnStmt> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, WriteStmt) {
  NodeCollector<AST::WriteStmt> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ForLoop) {
  NodeCollector<AST::ForLoop> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, WhileLoop) {
  NodeCollector<AST::WhileLoop> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, IfStmt) {
  NodeCollector<AST::IfStmt> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, ElseIfStmt) {
  NodeCollector<AST::ElseIfStmt> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ElseStmt) {
  NodeCollector<AST::ElseStmt> collector;
  m_root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}
