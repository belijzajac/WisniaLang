// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

// Wisnia
#include "Lexer.hpp"
#include "AST.hpp"
#include "NodeCollector.hpp"
#include "SemanticTestFixture.hpp"

using namespace Wisnia;
using NodeCollectorTest = SemanticTestFixture;

TEST_F(NodeCollectorTest, ClassDef) {
  NodeCollector<AST::ClassDef> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, FnDef) {
  NodeCollector<AST::FnDef> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, Param) {
  NodeCollector<AST::Param> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, VarExpr) {
  NodeCollector<AST::VarExpr> collector;
  m_root->accept(collector);
  const auto &collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 44);
  constexpr std::array kExpectedValues{
    "Foo", "is_fifteen", "number", "digit", "simple_operations", "a", "number", "i", "i", "i",
    "i", "a", "a", "a", "is_fifteen", "digit", "a", "a", "digit", "digit", "a", "digit",
    "output_hello", "do_output", "do_output", "main", "argc", "argv", "correct", "argc", "answer",
    "output_hello", "correct", "fooPtr", "Foo", "correct", "yes", "answer", "output_hello", "yes",
    "argc", "answer", "output_hello", "argc"};
  for (size_t i = 0; i < kExpectedValues.size(); i++) {
    EXPECT_TRUE(collectedNodes[i]->getToken()->getValue<std::string>() == kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, VarAssignStmt) {
  NodeCollector<AST::VarAssignStmt> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 7);
}

TEST_F(NodeCollectorTest, CompExpr) {
  NodeCollector<AST::CompExpr> collector;
  m_root->accept(collector);
  const auto &collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 4);
  constexpr std::array kExpectedValues{'<', '<', '<', '>'};
  for (size_t i = 0; i < kExpectedValues.size(); i++) {
    EXPECT_TRUE(collectedNodes[i]->getStringOperand() == std::string{kExpectedValues[i]});
  }
}

TEST_F(NodeCollectorTest, AddExpr) {
  NodeCollector<AST::AddExpr> collector;
  m_root->accept(collector);
  const auto &collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 3);
  constexpr std::array kExpectedValues{'+', '+', '+'};
  for (size_t i = 0; i < kExpectedValues.size(); i++) {
    EXPECT_TRUE(collectedNodes[i]->getStringOperand() == std::string{kExpectedValues[i]});
  }
}

TEST_F(NodeCollectorTest, MultExpr) {
  NodeCollector<AST::MultExpr> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, EqExpr) {
  NodeCollector<AST::EqExpr> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, BoolExpr) {
  NodeCollector<AST::BoolExpr> collector;
  m_root->accept(collector);
  const auto &collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 5);
  constexpr std::array kExpectedValues{false, true, true, false, false};
  for (size_t i = 0; i < kExpectedValues.size(); i++) {
    EXPECT_EQ(collectedNodes[i]->getToken()->getValue<bool>(), kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, StringExpr) {
  NodeCollector<AST::StringExpr> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
  EXPECT_STREQ(collector.getNodes()[0]->getToken()->getValue<std::string>().c_str(), "hello");
  EXPECT_STREQ(collector.getNodes()[1]->getToken()->getValue<std::string>().c_str(), "bye");
}

TEST_F(NodeCollectorTest, IntExpr) {
  NodeCollector<AST::IntExpr> collector;
  m_root->accept(collector);
  const auto &collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 10);
  constexpr std::array kExpectedValues{123456, 0, 5, 1, 150000, 2, 5, 5, 7, 5};
  for (size_t i = 0; i < kExpectedValues.size(); i++) {
    EXPECT_EQ(collectedNodes[i]->getToken()->getValue<int>(), kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, FloatExpr) {
  NodeCollector<AST::FloatExpr> collector;
  m_root->accept(collector);
  const auto &collectedNodes = collector.getNodes();
  EXPECT_EQ(collector.getNodes().size(), 5);
  constexpr std::array<float, 5> kExpectedValues{5.0, 7.75, 1.45, 15.0, 1.33};
  for (size_t i = 0; i < kExpectedValues.size(); i++) {
    EXPECT_EQ(collectedNodes[i]->getToken()->getValue<float>(), kExpectedValues[i]);
  }
}

TEST_F(NodeCollectorTest, BreakStmt) {
  NodeCollector<AST::BreakStmt> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ReturnStmt) {
  NodeCollector<AST::ReturnStmt> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, WriteStmt) {
  NodeCollector<AST::WriteStmt> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, ForLoop) {
  NodeCollector<AST::ForLoop> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}

TEST_F(NodeCollectorTest, WhileLoop) {
  NodeCollector<AST::WhileLoop> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, IfStmt) {
  NodeCollector<AST::IfStmt> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 3);
}

TEST_F(NodeCollectorTest, ElseIfStmt) {
  NodeCollector<AST::ElseIfStmt> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 2);
}

TEST_F(NodeCollectorTest, ElseStmt) {
  NodeCollector<AST::ElseStmt> collector;
  m_root->accept(collector);
  EXPECT_EQ(collector.getNodes().size(), 1);
}
