#include <fmt/format.h>
// Wisnia
#include "NameResolver.h"
#include "AST.h"

using namespace Wisnia;
using namespace AST;

void NameResolver::visit(AST::Root *node) {
  fmt::print("{}\n", node->kind());
  for (const auto &fn : node->globalFnDefs_)
    fn->accept(this);
  for (const auto &klass : node->globalClassDefs_)
    klass->accept(this);
}

void NameResolver::visit(AST::PrimitiveType *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::VarExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::BinaryExpr *node) {
  fmt::print("{}\n", node->kind());
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void NameResolver::visit(AST::BooleanExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::EqExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::CompExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::AddExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::MultExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::UnaryExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::FnCallExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::ClassInitExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::IntExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::FloatExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::BoolExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::StringExpr *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::StmtBlock *node) {
  fmt::print("{}\n", node->kind());
  for (const auto &stmt : node->stmts_)
    stmt->accept(this);
}

void NameResolver::visit(AST::ReturnStmt *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::BreakStmt *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::VarDeclStmt *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::VarAssignStmt *node) {
  fmt::print("{}\n", node->kind());
  node->value_->accept(this);
}

void NameResolver::visit(AST::ExprStmt *node) {
  fmt::print("{}\n", node->kind());
  node->expr_->accept(this);
}

void NameResolver::visit(AST::ReadStmt *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::WriteStmt *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::Param *node) {
  fmt::print("{}\n", node->kind());
  node->var_->accept(this);
}

void NameResolver::visit(AST::FnDef *node) {
  fmt::print("{}\n", node->kind());
  for (const auto &param : node->params_)
    param->accept(this);
  node->body_->accept(this);
}

void NameResolver::visit(AST::CtorDef *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::DtorDef *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::Field *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::ClassDef *node) {
  fmt::print("{}\n", node->kind());
}

void NameResolver::visit(AST::WhileLoop *node) {
  fmt::print("{}\n", node->kind());
  node->cond_->accept(this);
  node->body_->accept(this);
}

void NameResolver::visit(AST::ForLoop *node) {
  fmt::print("{}\n", node->kind());
  node->init_->accept(this);
  node->cond_->accept(this);
  node->incdec_->accept(this);
  node->body_->accept(this);
}

void NameResolver::visit(AST::ForEachLoop *node) {
  fmt::print("{}\n", node->kind());
  node->elem_->accept(this);
  node->iterElem_->accept(this);
  node->body_->accept(this);
}

void NameResolver::visit(AST::IfStmt *node) {
  fmt::print("{}\n", node->kind());
  node->cond_->accept(this);
  node->body_->accept(this);
  for (const auto &elseBl : node->elseBlcks_)
    elseBl->accept(this);
}

void NameResolver::visit(AST::ElseStmt *node) {
  fmt::print("{}\n", node->kind());
  node->body_->accept(this);
}

void NameResolver::visit(AST::ElseIfStmt *node) {
  fmt::print("{}\n", node->kind());
  node->cond_->accept(this);
  node->body_->accept(this);
}
