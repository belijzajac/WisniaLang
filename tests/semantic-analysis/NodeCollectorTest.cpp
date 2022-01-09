// Wisnia
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "NodeCollector.h"
#include "SemanticTestFixture.h"

using namespace Wisnia;
using NodeCollectorTest = SemanticTestFixture;

TEST_F(NodeCollectorTest, ClassDef) {
  NodeCollector<AST::ClassDef> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, FnDef) {
  NodeCollector<AST::FnDef> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, Param) {
  NodeCollector<AST::Param> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, VarExpr) {
  NodeCollector<AST::VarExpr> collector{};
  root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 44);
  constexpr std::array<const char *, 44> expectedValues{
      "Foo", "is_fifteen", "number", "digit", "simple_operations", "a", "number", "i", "i", "i",
      "i", "a", "a", "a", "is_fifteen", "digit", "a", "a", "digit", "digit", "a", "digit",
      "output_hello", "do_output", "do_output", "main", "argc", "argv", "correct", "argc", "answer",
      "output_hello", "correct", "fooPtr", "Foo", "correct", "yes", "answer", "output_hello", "yes",
      "argc", "answer", "output_hello", "argc"};
  for (size_t i = 0; i < 44; i++) {
    EXPECT_TRUE(collectedNodes[i]->token_->getValue<std::string>() == expectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, VarAssignStmt) {
  NodeCollector<AST::VarAssignStmt> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 7);
}

TEST_F(NodeCollectorTest, CompExpr) {
  NodeCollector<AST::CompExpr> collector{};
  root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 4);
  constexpr std::array<char, 4> expectedValues{'<', '<', '<', '>'};
  for (size_t i = 0; i < 4; i++) {
    EXPECT_TRUE(collectedNodes[i]->opStr_ == std::string{expectedValues[i]});
  }
}

TEST_F(NodeCollectorTest, AddExpr) {
  NodeCollector<AST::AddExpr> collector{};
  root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 4);
  constexpr std::array<char, 4> expectedValues{'+', '+', '+', '-'};
  for (size_t i = 0; i < 4; i++) {
    EXPECT_TRUE(collectedNodes[i]->opStr_ == std::string{expectedValues[i]});
  }
}

TEST_F(NodeCollectorTest, MultExpr) {
  NodeCollector<AST::MultExpr> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, EqExpr) {
  NodeCollector<AST::EqExpr> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, BoolExpr) {
  NodeCollector<AST::BoolExpr> collector{};
  root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 5);
  constexpr std::array<bool, 5> expectedValues{false, true, true, false, false};
  for (size_t i = 0; i < 5; i++) {
    EXPECT_EQ(collectedNodes[i]->token_->getValue<bool>(), expectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, StringExpr) {
  NodeCollector<AST::StringExpr> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
  EXPECT_TRUE(collector.getNodes()[0]->token_->getValue<std::string>() == "hello");
  EXPECT_TRUE(collector.getNodes()[1]->token_->getValue<std::string>() == "bye");
}

TEST_F(NodeCollectorTest, IntExpr) {
  NodeCollector<AST::IntExpr> collector{};
  root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 10);
  constexpr std::array<int, 10> expectedValues{123456, 0, 5, 1, 150000, 2, 5, 5, 7, 5};
  for (size_t i = 0; i < 10; i++) {
    EXPECT_EQ(collectedNodes[i]->token_->getValue<int>(), expectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, FloatExpr) {
  NodeCollector<AST::FloatExpr> collector{};
  root->accept(&collector);
  auto collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 5);
  constexpr std::array<float, 5> expectedValues{5.0, 7.75, 1.45, 15.0, 1.33};
  for (size_t i = 0; i < 5; i++) {
    EXPECT_EQ(collectedNodes[i]->token_->getValue<float>(), expectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, BreakStmt) {
  NodeCollector<AST::BreakStmt> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ReturnStmt) {
  NodeCollector<AST::ReturnStmt> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, WriteStmt) {
  NodeCollector<AST::WriteStmt> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ForLoop) {
  NodeCollector<AST::ForLoop> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, WhileLoop) {
  NodeCollector<AST::WhileLoop> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, IfStmt) {
  NodeCollector<AST::IfStmt> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, ElseIfStmt) {
  NodeCollector<AST::ElseIfStmt> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ElseStmt) {
  NodeCollector<AST::ElseStmt> collector{};
  root->accept(&collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}