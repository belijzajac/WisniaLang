#include <fmt/ostream.h>
#include <iostream>
// Wisnia
#include "NameResolver.h"
#include "AST.h"

using namespace Wisnia;
using namespace Utils;
using namespace AST;

void NameResolver::visit(AST::Root *node) {
  for (const auto &klass : node->m_globalClasses) {
    klass->accept(this);
  }
  for (const auto &function : node->m_globalFunctions) {
    function->accept(this);
  }
}

void NameResolver::visit(AST::PrimitiveType *node) {
  // nothing to do
}

void NameResolver::visit(AST::VarExpr *node) {
  try {
    auto foundVar = m_table.findSymbol(node->m_token->getValue<std::string>()); // VarExpr
    node->m_type = std::make_unique<PrimitiveType>(foundVar->m_type->m_token);
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

void NameResolver::visit(AST::MultExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::UnaryExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::FnCallExpr *node) {
  node->m_var->accept(this);
  for (const auto &arg : node->m_args) {
    arg->accept(this);
  }
}

void NameResolver::visit(AST::ClassInitExpr *node) {
  node->m_var->accept(this);
  for (const auto &arg : node->m_args) {
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
  for (const auto &stmt : node->m_statements) {
    stmt->accept(this);
  }
  m_table.popScope();
}

void NameResolver::visit(AST::ReturnStmt *node) {
  node->m_returnValue->accept(this);
}

void NameResolver::visit(AST::BreakStmt *node) {
  // nothing to do
}

void NameResolver::visit(AST::ContinueStmt *node) {
  // nothing to do
}

void NameResolver::visit(AST::VarDeclStmt *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->m_var.get()));
  node->m_var->accept(this);
  node->m_value->accept(this);
}

void NameResolver::visit(AST::VarAssignStmt *node) {
  node->m_var->accept(this);
  node->m_val->accept(this);
}

void NameResolver::visit(AST::ExprStmt *node) {
  node->m_expr->accept(this);
}

void NameResolver::visit(AST::ReadStmt *node) {
  for (const auto &var : node->m_vars) {
    var->accept(this);
  }
}

void NameResolver::visit(AST::WriteStmt *node) {
  for (const auto &expr : node->m_exprs) {
    expr->accept(this);
  }
}

void NameResolver::visit(AST::Param *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->m_var.get()));
  node->m_var->accept(this);
}

void NameResolver::visit(AST::FnDef *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->m_var.get()));
  node->m_var->accept(this);
  for (const auto &param : node->m_params) {
    param->accept(this);
  }
  node->m_body->accept(this);
}

void NameResolver::visit(AST::CtorDef *node) {
  throw NotImplementedError{"Constructors are not supported"};
}

void NameResolver::visit(AST::DtorDef *node) {
  throw NotImplementedError{"Destructors are not supported"};
}

void NameResolver::visit(AST::Field *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->m_var.get()));
  node->m_var->accept(this);
  node->m_value->accept(this);
}

void NameResolver::visit(AST::ClassDef *node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node->m_var.get()));
  node->m_var->accept(this);
  for (const auto &field : node->m_fields) {
    field->accept(this);
  }
  if (node->m_ctor) node->m_ctor->accept(this);
  if (node->m_dtor) node->m_dtor->accept(this);
  for (const auto &method : node->m_methods) {
    method->accept(this);
  }
}

void NameResolver::visit(AST::WhileLoop *node) {
  node->m_cond->accept(this);
  node->m_body->accept(this);
}

void NameResolver::visit(AST::ForLoop *node) {
  node->m_initialization->accept(this);
  node->m_condition->accept(this);
  node->m_increment->accept(this);
  node->m_body->accept(this);
}

void NameResolver::visit(AST::ForEachLoop *node) {
  node->m_element->accept(this);
  node->m_collection->accept(this);
  node->m_body->accept(this);
}

void NameResolver::visit(AST::IfStmt *node) {
  node->m_condition->accept(this);
  node->m_body->accept(this);
  for (const auto &elseBl : node->m_elseStmts) {
    elseBl->accept(this);
  }
}

void NameResolver::visit(AST::ElseStmt *node) {
  node->m_body->accept(this);
}

void NameResolver::visit(AST::ElseIfStmt *node) {
  node->m_condition->accept(this);
  node->m_body->accept(this);
}
