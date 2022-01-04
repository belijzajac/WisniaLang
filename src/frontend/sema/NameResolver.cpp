#include <fmt/ostream.h>
#include <iostream>
// Wisnia
#include "NameResolver.h"
#include "AST.h"

using namespace Wisnia;
using namespace AST;

void NameResolver::visit(AST::Root *node) {
  for (const auto &klass : node->globalClassDefs_) {
    klass->accept(this);
  }
  for (const auto &fn : node->globalFnDefs_) {
    fn->accept(this);
  }
}

void NameResolver::visit(AST::PrimitiveType *node) {
  // nothing to do
}

void NameResolver::visit(AST::VarExpr *node) {
  try {
    auto foundVar = table.findSymbol(node->token_->getValue<std::string>()); // VarExpr
    node->type_ = std::make_unique<PrimitiveType>(foundVar->type_->token_);
  } catch (const Utils::SemanticError &ex) {
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
  node->var_->accept(this);
  for (const auto &arg : node->args_) {
    arg->accept(this);
  }
}

void NameResolver::visit(AST::ClassInitExpr *node) {
  node->var_->accept(this);
  for (const auto &arg : node->args_) {
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
  table.pushScope();
  for (const auto &stmt : node->stmts_) {
    stmt->accept(this);
  }
  table.popScope();
}

void NameResolver::visit(AST::ReturnStmt *node) {
  node->returnValue_->accept(this);
}

void NameResolver::visit(AST::BreakStmt *node) {
  // nothing to do
}

void NameResolver::visit(AST::ContinueStmt *node) {
  // nothing to do
}

void NameResolver::visit(AST::VarDeclStmt *node) {
  table.addSymbol(dynamic_cast<VarExpr *>(node->var_.get()));
  node->var_->accept(this);
  node->value_->accept(this);
}

void NameResolver::visit(AST::VarAssignStmt *node) {
  node->var_->accept(this);
  node->value_->accept(this);
}

void NameResolver::visit(AST::ExprStmt *node) {
  node->expr_->accept(this);
}

void NameResolver::visit(AST::ReadStmt *node) {
  for (const auto &var : node->vars_) {
    var->accept(this);
  }
}

void NameResolver::visit(AST::WriteStmt *node) {
  for (const auto &expr : node->exprs_) {
    expr->accept(this);
  }
}

void NameResolver::visit(AST::Param *node) {
  table.addSymbol(dynamic_cast<VarExpr *>(node->var_.get()));
  node->var_->accept(this);
}

void NameResolver::visit(AST::FnDef *node) {
  table.addSymbol(dynamic_cast<VarExpr *>(node->var_.get()));
  node->var_->accept(this);
  for (const auto &param : node->params_) {
    param->accept(this);
  }
  node->body_->accept(this);
}

void NameResolver::visit(AST::CtorDef *node) {
  throw Utils::NotImplementedError{"Constructors are not supported"};
}

void NameResolver::visit(AST::DtorDef *node) {
  throw Utils::NotImplementedError{"Destructors are not supported"};
}

void NameResolver::visit(AST::Field *node) {
  table.addSymbol(dynamic_cast<VarExpr *>(node->var_.get()));
  node->var_->accept(this);
  node->value_->accept(this);
}

void NameResolver::visit(AST::ClassDef *node) {
  table.addSymbol(dynamic_cast<VarExpr *>(node->var_.get()));
  node->var_->accept(this);
  for (const auto &field : node->fields_) {
    field->accept(this);
  }
  if (node->ctor_) node->ctor_->accept(this);
  if (node->dtor_) node->dtor_->accept(this);
  for (const auto &method : node->methods_) {
    method->accept(this);
  }
}

void NameResolver::visit(AST::WhileLoop *node) {
  node->cond_->accept(this);
  node->body_->accept(this);
}

void NameResolver::visit(AST::ForLoop *node) {
  node->init_->accept(this);
  node->cond_->accept(this);
  node->incdec_->accept(this);
  node->body_->accept(this);
}

void NameResolver::visit(AST::ForEachLoop *node) {
  node->elem_->accept(this);
  node->iterElem_->accept(this);
  node->body_->accept(this);
}

void NameResolver::visit(AST::IfStmt *node) {
  node->cond_->accept(this);
  node->body_->accept(this);
  for (const auto &elseBl : node->elseBlcks_) {
    elseBl->accept(this);
  }
}

void NameResolver::visit(AST::ElseStmt *node) {
  node->body_->accept(this);
}

void NameResolver::visit(AST::ElseIfStmt *node) {
  node->cond_->accept(this);
  node->body_->accept(this);
}
