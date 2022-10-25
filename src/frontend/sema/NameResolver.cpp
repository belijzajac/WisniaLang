/***

  WisniaLang - A Compiler for an Experimental Programming Language
  Copyright (C) 2022 Tautvydas Povilaitis (belijzajac) and contributors

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

***/

#include <fmt/ostream.h>
#include <iostream>
// Wisnia
#include "NameResolver.hpp"
#include "AST.hpp"

using namespace Wisnia;
using namespace AST;

void NameResolver::visit(AST::Root *node) {
  for (const auto &klass : node->getGlobalClasses()) {
    klass->accept(this);
  }
  for (const auto &function : node->getGlobalFunctions()) {
    function->accept(this);
  }
}

void NameResolver::visit(AST::PrimitiveType *node) {
  // nothing to do
}

void NameResolver::visit(AST::VarExpr *node) {
  try {
    auto foundVar = m_table.findSymbol(node->getToken()->getValue<std::string>()); // VarExpr
    node->addType(std::make_unique<PrimitiveType>(foundVar->getType()->getToken()));
  } catch (const SemanticError &ex) {
    fmt::print(std::cerr, "{}\n", ex.what());
  }
}

void NameResolver::visit(AST::BooleanExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::EqExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::CompExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::AddExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::SubExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::MultExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::DivExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::UnaryExpr *node) {
  node->lhs()->accept(this);
}

void NameResolver::visit(AST::FnCallExpr *node) {
  node->getVar()->accept(this);
  for (const auto &arg : node->getArgs()) {
    arg->accept(this);
  }
}

void NameResolver::visit(AST::ClassInitExpr *node) {
  node->getVar()->accept(this);
  for (const auto &arg : node->getArgs()) {
    arg->accept(this);
  }
}

void NameResolver::visit(AST::IntExpr *node) {
  // nothing to do
}

void NameResolver::visit(AST::FloatExpr *node) {
  // nothing to do
}

void NameResolver::visit(AST::BoolExpr *node) {
  // nothing to do
}

void NameResolver::visit(AST::StringExpr *node) {
  // nothing to do
}

void NameResolver::visit(AST::StmtBlock *node) {
  m_table.pushScope();
  for (const auto &stmt : node->getStatements()) {
    stmt->accept(this);
  }
  m_table.popScope();
}

void NameResolver::visit(AST::ReturnStmt *node) {
  node->getReturnValue()->accept(this);
}

void NameResolver::visit(AST::BreakStmt *node) {
  // nothing to do
}

void NameResolver::visit(AST::ContinueStmt *node) {
  // nothing to do
}

void NameResolver::visit(AST::VarDeclStmt *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->getVar().get()));
  node->getVar()->accept(this);
  node->getValue()->accept(this);
}

void NameResolver::visit(AST::VarAssignStmt *node) {
  node->getVar()->accept(this);
  node->getValue()->accept(this);
}

void NameResolver::visit(AST::ExprStmt *node) {
  node->getExpr()->accept(this);
}

void NameResolver::visit(AST::ReadStmt *node) {
  for (const auto &var : node->getVars()) {
    var->accept(this);
  }
}

void NameResolver::visit(AST::WriteStmt *node) {
  for (const auto &expr : node->getExprs()) {
    expr->accept(this);
  }
}

void NameResolver::visit(AST::Param *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->getVar().get()));
  node->getVar()->accept(this);
}

void NameResolver::visit(AST::FnDef *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->getVar().get()));
  node->getVar()->accept(this);
  for (const auto &param : node->getParams()) {
    param->accept(this);
  }
  node->getBody()->accept(this);
}

void NameResolver::visit(AST::CtorDef *node) {
  throw NotImplementedError{"Constructors are not supported"};
}

void NameResolver::visit(AST::DtorDef *node) {
  throw NotImplementedError{"Destructors are not supported"};
}

void NameResolver::visit(AST::Field *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->getVar().get()));
  node->getVar()->accept(this);
  node->getValue()->accept(this);
}

void NameResolver::visit(AST::ClassDef *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->getVar().get()));
  node->getVar()->accept(this);
  for (const auto &field : node->getFields()) {
    field->accept(this);
  }
  if (node->getCtor()) node->getCtor()->accept(this);
  if (node->getDtor()) node->getDtor()->accept(this);
  for (const auto &method : node->getMethods()) {
    method->accept(this);
  }
}

void NameResolver::visit(AST::WhileLoop *node) {
  node->getCondition()->accept(this);
  node->getBody()->accept(this);
}

void NameResolver::visit(AST::ForLoop *node) {
  node->getInitial()->accept(this);
  node->getCondition()->accept(this);
  node->getIncrement()->accept(this);
  node->getBody()->accept(this);
}

void NameResolver::visit(AST::ForEachLoop *node) {
  node->getElement()->accept(this);
  node->getCollection()->accept(this);
  node->getBody()->accept(this);
}

void NameResolver::visit(AST::IfStmt *node) {
  node->getCondition()->accept(this);
  node->getBody()->accept(this);
  for (const auto &elseBl : node->getElseStatements()) {
    elseBl->accept(this);
  }
}

void NameResolver::visit(AST::ElseStmt *node) {
  node->getBody()->accept(this);
}

void NameResolver::visit(AST::ElseIfStmt *node) {
  node->getCondition()->accept(this);
  node->getBody()->accept(this);
}
