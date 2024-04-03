// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include "NodeCollector.hpp"
#include "AST.hpp"

template <typename T>
void NodeCollector<T>::visit(AST::Root &node) {
  if constexpr (std::is_same_v<T, AST::Root>) {
    m_nodes.push_back(&node);
  }
  for (const auto &klass : node.getGlobalClasses()) {
    klass->accept(*this);
  }
  for (const auto &function : node.getGlobalFunctions()) {
    function->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::PrimitiveType &node) {
  if constexpr (std::is_same_v<T, AST::PrimitiveType>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::VarExpr &node) {
  if constexpr (std::is_same_v<T, AST::VarExpr>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::BooleanExpr &node) {
  if constexpr (std::is_same_v<T, AST::BooleanExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::EqExpr &node) {
  if constexpr (std::is_same_v<T, AST::EqExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::CompExpr &node) {
  if constexpr (std::is_same_v<T, AST::CompExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::AddExpr &node) {
  if constexpr (std::is_same_v<T, AST::AddExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::SubExpr &node) {
  if constexpr (std::is_same_v<T, AST::SubExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::MultExpr &node) {
  if constexpr (std::is_same_v<T, AST::MultExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::DivExpr &node) {
  if constexpr (std::is_same_v<T, AST::DivExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::UnaryExpr &node) {
  if constexpr (std::is_same_v<T, AST::UnaryExpr>) {
    m_nodes.push_back(&node);
  }
  node.lhs()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::FnCallExpr &node) {
  if constexpr (std::is_same_v<T, AST::FnCallExpr>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
  for (const auto &arg : node.getArguments()) {
    arg->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ClassInitExpr &node) {
  if constexpr (std::is_same_v<T, AST::ClassInitExpr>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
  for (const auto &arg : node.getArguments()) {
    arg->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::IntExpr &node) {
  if constexpr (std::is_same_v<T, AST::IntExpr>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::FloatExpr &node) {
  if constexpr (std::is_same_v<T, AST::FloatExpr>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::BoolExpr &node) {
  if constexpr (std::is_same_v<T, AST::BoolExpr>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::StringExpr &node) {
  if constexpr (std::is_same_v<T, AST::StringExpr>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::StmtBlock &node) {
  if constexpr (std::is_same_v<T, AST::StmtBlock>) {
    m_nodes.push_back(&node);
  }
  for (const auto &stmt : node.getStatements()) {
    stmt->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ReturnStmt &node) {
  if constexpr (std::is_same_v<T, AST::ReturnStmt>) {
    m_nodes.push_back(&node);
  }
  node.getReturnValue()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::BreakStmt &node) {
  if constexpr (std::is_same_v<T, AST::BreakStmt>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ContinueStmt &node) {
  if constexpr (std::is_same_v<T, AST::ContinueStmt>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::VarDeclStmt &node) {
  if constexpr (std::is_same_v<T, AST::VarDeclStmt>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::VarAssignStmt &node) {
  if constexpr (std::is_same_v<T, AST::VarAssignStmt>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ExprStmt &node) {
  if constexpr (std::is_same_v<T, AST::ExprStmt>) {
    m_nodes.push_back(&node);
  }
  node.getExpression()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ReadStmt &node) {
  if constexpr (std::is_same_v<T, AST::ReadStmt>) {
    m_nodes.push_back(&node);
  }
  for (const auto &var : node.getVariableList()) {
    var->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::WriteStmt &node) {
  if constexpr (std::is_same_v<T, AST::WriteStmt>) {
    m_nodes.push_back(&node);
  }
  for (const auto &expr : node.getExpressions()) {
    expr->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::Param &node) {
  if constexpr (std::is_same_v<T, AST::Param>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::FnDef &node) {
  if constexpr (std::is_same_v<T, AST::FnDef>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
  for (const auto &param : node.getParameters()) {
    param->accept(*this);
  }
  node.getBody()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::CtorDef &node) {
  if constexpr (std::is_same_v<T, AST::CtorDef>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::DtorDef &node) {
  if constexpr (std::is_same_v<T, AST::DtorDef>) {
    m_nodes.push_back(&node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::Field &node) {
  if constexpr (std::is_same_v<T, AST::Field>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ClassDef &node) {
  if constexpr (std::is_same_v<T, AST::ClassDef>) {
    m_nodes.push_back(&node);
  }
  node.getVariable()->accept(*this);
  for (const auto &field : node.getFields()) {
    field->accept(*this);
  }
  if (node.getConstructor()) node.getConstructor()->accept(*this);
  if (node.getDestructor()) node.getDestructor()->accept(*this);
  for (const auto &method : node.getMethods()) {
    method->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::WhileLoop &node) {
  if constexpr (std::is_same_v<T, AST::WhileLoop>) {
    m_nodes.push_back(&node);
  }
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ForLoop &node) {
  if constexpr (std::is_same_v<T, AST::ForLoop>) {
    m_nodes.push_back(&node);
  }
  node.getInitial()->accept(*this);
  node.getCondition()->accept(*this);
  node.getIncrement()->accept(*this);
  node.getBody()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ForEachLoop &node) {
  if constexpr (std::is_same_v<T, AST::ForEachLoop>) {
    m_nodes.push_back(&node);
  }
  node.getElement()->accept(*this);
  node.getCollection()->accept(*this);
  node.getBody()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::IfStmt &node) {
  if constexpr (std::is_same_v<T, AST::IfStmt>) {
    m_nodes.push_back(&node);
  }
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
  for (const auto &elseBl : node.getElseStatements()) {
    elseBl->accept(*this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ElseStmt &node) {
  if constexpr (std::is_same_v<T, AST::ElseStmt>) {
    m_nodes.push_back(&node);
  }
  node.getBody()->accept(*this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ElseIfStmt &node) {
  if constexpr (std::is_same_v<T, AST::ElseIfStmt>) {
    m_nodes.push_back(&node);
  }
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
}
