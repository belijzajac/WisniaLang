#include "NodeCollector.h"
#include "AST.h"

template <typename T>
void NodeCollector<T>::visit(AST::Root *node) {
  if constexpr (std::is_same_v<T, AST::Root>) {
    nodes.push_back(node);
  }
  for (const auto &klass : node->globalClassDefs_) {
    klass->accept(this);
  }
  for (const auto &fn : node->globalFnDefs_) {
    fn->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::PrimitiveType *node) {
  if constexpr (std::is_same_v<T, AST::PrimitiveType>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::VarExpr *node) {
  if constexpr (std::is_same_v<T, AST::VarExpr>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::BooleanExpr *node) {
  if constexpr (std::is_same_v<T, AST::BooleanExpr>) {
    nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::EqExpr *node) {
  if constexpr (std::is_same_v<T, AST::EqExpr>) {
    nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::CompExpr *node) {
  if constexpr (std::is_same_v<T, AST::CompExpr>) {
    nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::AddExpr *node) {
  if constexpr (std::is_same_v<T, AST::AddExpr>) {
    nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::MultExpr *node) {
  if constexpr (std::is_same_v<T, AST::MultExpr>) {
    nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::UnaryExpr *node) {
  if constexpr (std::is_same_v<T, AST::UnaryExpr>) {
    nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::FnCallExpr *node) {
  if constexpr (std::is_same_v<T, AST::FnCallExpr>) {
    nodes.push_back(node);
  }
  node->var_->accept(this);
  for (const auto &arg : node->args_) {
    arg->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ClassInitExpr *node) {
  if constexpr (std::is_same_v<T, AST::ClassInitExpr>) {
    nodes.push_back(node);
  }
  node->var_->accept(this);
  for (const auto &arg : node->args_) {
    arg->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::IntExpr *node) {
  if constexpr (std::is_same_v<T, AST::IntExpr>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::FloatExpr *node) {
  if constexpr (std::is_same_v<T, AST::FloatExpr>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::BoolExpr *node) {
  if constexpr (std::is_same_v<T, AST::BoolExpr>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::StringExpr *node) {
  if constexpr (std::is_same_v<T, AST::StringExpr>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::StmtBlock *node) {
  if constexpr (std::is_same_v<T, AST::StmtBlock>) {
    nodes.push_back(node);
  }
  for (const auto &stmt : node->stmts_) {
    stmt->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ReturnStmt *node) {
  if constexpr (std::is_same_v<T, AST::ReturnStmt>) {
    nodes.push_back(node);
  }
  node->returnValue_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::BreakStmt *node) {
  if constexpr (std::is_same_v<T, AST::BreakStmt>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ContinueStmt *node) {
  if constexpr (std::is_same_v<T, AST::ContinueStmt>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::VarDeclStmt *node) {
  if constexpr (std::is_same_v<T, AST::VarDeclStmt>) {
    nodes.push_back(node);
  }
  node->var_->accept(this);
  node->value_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::VarAssignStmt *node) {
  if constexpr (std::is_same_v<T, AST::VarAssignStmt>) {
    nodes.push_back(node);
  }
  node->var_->accept(this);
  node->value_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ExprStmt *node) {
  if constexpr (std::is_same_v<T, AST::ExprStmt>) {
    nodes.push_back(node);
  }
  node->expr_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ReadStmt *node) {
  if constexpr (std::is_same_v<T, AST::ReadStmt>) {
    nodes.push_back(node);
  }
  for (const auto &var : node->vars_) {
    var->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::WriteStmt *node) {
  if constexpr (std::is_same_v<T, AST::WriteStmt>) {
    nodes.push_back(node);
  }
  for (const auto &expr : node->exprs_) {
    expr->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::Param *node) {
  if constexpr (std::is_same_v<T, AST::Param>) {
    nodes.push_back(node);
  }
  node->var_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::FnDef *node) {
  if constexpr (std::is_same_v<T, AST::FnDef>) {
    nodes.push_back(node);
  }
  node->var_->accept(this);
  for (const auto &param : node->params_) {
    param->accept(this);
  }
  node->body_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::CtorDef *node) {
  if constexpr (std::is_same_v<T, AST::CtorDef>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::DtorDef *node) {
  if constexpr (std::is_same_v<T, AST::DtorDef>) {
    nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::Field *node) {
  if constexpr (std::is_same_v<T, AST::Field>) {
    nodes.push_back(node);
  }
  node->var_->accept(this);
  node->value_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ClassDef *node) {
  if constexpr (std::is_same_v<T, AST::ClassDef>) {
    nodes.push_back(node);
  }
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

template <typename T>
void NodeCollector<T>::visit(AST::WhileLoop *node) {
  if constexpr (std::is_same_v<T, AST::WhileLoop>) {
    nodes.push_back(node);
  }
  node->cond_->accept(this);
  node->body_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ForLoop *node) {
  if constexpr (std::is_same_v<T, AST::ForLoop>) {
    nodes.push_back(node);
  }
  node->init_->accept(this);
  node->cond_->accept(this);
  node->incdec_->accept(this);
  node->body_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ForEachLoop *node) {
  if constexpr (std::is_same_v<T, AST::ForEachLoop>) {
    nodes.push_back(node);
  }
  node->elem_->accept(this);
  node->iterElem_->accept(this);
  node->body_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::IfStmt *node) {
  if constexpr (std::is_same_v<T, AST::IfStmt>) {
    nodes.push_back(node);
  }
  node->cond_->accept(this);
  node->body_->accept(this);
  for (const auto &elseBl : node->elseBlcks_) {
    elseBl->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ElseStmt *node) {
  if constexpr (std::is_same_v<T, AST::ElseStmt>) {
    nodes.push_back(node);
  }
  node->body_->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ElseIfStmt *node) {
  if constexpr (std::is_same_v<T, AST::ElseIfStmt>) {
    nodes.push_back(node);
  }
  node->cond_->accept(this);
  node->body_->accept(this);
}
