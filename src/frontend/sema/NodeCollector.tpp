#include "NodeCollector.hpp"
#include "AST.hpp"

template <typename T>
void NodeCollector<T>::visit(AST::Root *node) {
  if constexpr (std::is_same_v<T, AST::Root>) {
    m_nodes.push_back(node);
  }
  for (const auto &klass : node->m_globalClasses) {
    klass->accept(this);
  }
  for (const auto &function : node->m_globalFunctions) {
    function->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::PrimitiveType *node) {
  if constexpr (std::is_same_v<T, AST::PrimitiveType>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::VarExpr *node) {
  if constexpr (std::is_same_v<T, AST::VarExpr>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::BooleanExpr *node) {
  if constexpr (std::is_same_v<T, AST::BooleanExpr>) {
    m_nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::EqExpr *node) {
  if constexpr (std::is_same_v<T, AST::EqExpr>) {
    m_nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::CompExpr *node) {
  if constexpr (std::is_same_v<T, AST::CompExpr>) {
    m_nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::AddExpr *node) {
  if constexpr (std::is_same_v<T, AST::AddExpr>) {
    m_nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::MultExpr *node) {
  if constexpr (std::is_same_v<T, AST::MultExpr>) {
    m_nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::UnaryExpr *node) {
  if constexpr (std::is_same_v<T, AST::UnaryExpr>) {
    m_nodes.push_back(node);
  }
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::FnCallExpr *node) {
  if constexpr (std::is_same_v<T, AST::FnCallExpr>) {
    m_nodes.push_back(node);
  }
  node->m_var->accept(this);
  for (const auto &arg : node->m_args) {
    arg->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ClassInitExpr *node) {
  if constexpr (std::is_same_v<T, AST::ClassInitExpr>) {
    m_nodes.push_back(node);
  }
  node->m_var->accept(this);
  for (const auto &arg : node->m_args) {
    arg->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::IntExpr *node) {
  if constexpr (std::is_same_v<T, AST::IntExpr>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::FloatExpr *node) {
  if constexpr (std::is_same_v<T, AST::FloatExpr>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::BoolExpr *node) {
  if constexpr (std::is_same_v<T, AST::BoolExpr>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::StringExpr *node) {
  if constexpr (std::is_same_v<T, AST::StringExpr>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::StmtBlock *node) {
  if constexpr (std::is_same_v<T, AST::StmtBlock>) {
    m_nodes.push_back(node);
  }
  for (const auto &stmt : node->m_statements) {
    stmt->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ReturnStmt *node) {
  if constexpr (std::is_same_v<T, AST::ReturnStmt>) {
    m_nodes.push_back(node);
  }
  node->m_returnValue->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::BreakStmt *node) {
  if constexpr (std::is_same_v<T, AST::BreakStmt>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ContinueStmt *node) {
  if constexpr (std::is_same_v<T, AST::ContinueStmt>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::VarDeclStmt *node) {
  if constexpr (std::is_same_v<T, AST::VarDeclStmt>) {
    m_nodes.push_back(node);
  }
  node->m_var->accept(this);
  node->m_value->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::VarAssignStmt *node) {
  if constexpr (std::is_same_v<T, AST::VarAssignStmt>) {
    m_nodes.push_back(node);
  }
  node->m_var->accept(this);
  node->m_val->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ExprStmt *node) {
  if constexpr (std::is_same_v<T, AST::ExprStmt>) {
    m_nodes.push_back(node);
  }
  node->m_expr->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ReadStmt *node) {
  if constexpr (std::is_same_v<T, AST::ReadStmt>) {
    m_nodes.push_back(node);
  }
  for (const auto &var : node->m_vars) {
    var->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::WriteStmt *node) {
  if constexpr (std::is_same_v<T, AST::WriteStmt>) {
    m_nodes.push_back(node);
  }
  for (const auto &expr : node->m_exprs) {
    expr->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::Param *node) {
  if constexpr (std::is_same_v<T, AST::Param>) {
    m_nodes.push_back(node);
  }
  node->m_var->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::FnDef *node) {
  if constexpr (std::is_same_v<T, AST::FnDef>) {
    m_nodes.push_back(node);
  }
  node->m_var->accept(this);
  for (const auto &param : node->m_params) {
    param->accept(this);
  }
  node->m_body->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::CtorDef *node) {
  if constexpr (std::is_same_v<T, AST::CtorDef>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::DtorDef *node) {
  if constexpr (std::is_same_v<T, AST::DtorDef>) {
    m_nodes.push_back(node);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::Field *node) {
  if constexpr (std::is_same_v<T, AST::Field>) {
    m_nodes.push_back(node);
  }
  node->m_var->accept(this);
  node->m_value->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ClassDef *node) {
  if constexpr (std::is_same_v<T, AST::ClassDef>) {
    m_nodes.push_back(node);
  }
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

template <typename T>
void NodeCollector<T>::visit(AST::WhileLoop *node) {
  if constexpr (std::is_same_v<T, AST::WhileLoop>) {
    m_nodes.push_back(node);
  }
  node->m_cond->accept(this);
  node->m_body->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ForLoop *node) {
  if constexpr (std::is_same_v<T, AST::ForLoop>) {
    m_nodes.push_back(node);
  }
  node->m_initialization->accept(this);
  node->m_condition->accept(this);
  node->m_increment->accept(this);
  node->m_body->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ForEachLoop *node) {
  if constexpr (std::is_same_v<T, AST::ForEachLoop>) {
    m_nodes.push_back(node);
  }
  node->m_element->accept(this);
  node->m_collection->accept(this);
  node->m_body->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::IfStmt *node) {
  if constexpr (std::is_same_v<T, AST::IfStmt>) {
    m_nodes.push_back(node);
  }
  node->m_condition->accept(this);
  node->m_body->accept(this);
  for (const auto &elseBl : node->m_elseStmts) {
    elseBl->accept(this);
  }
}

template <typename T>
void NodeCollector<T>::visit(AST::ElseStmt *node) {
  if constexpr (std::is_same_v<T, AST::ElseStmt>) {
    m_nodes.push_back(node);
  }
  node->m_body->accept(this);
}

template <typename T>
void NodeCollector<T>::visit(AST::ElseIfStmt *node) {
  if constexpr (std::is_same_v<T, AST::ElseIfStmt>) {
    m_nodes.push_back(node);
  }
  node->m_condition->accept(this);
  node->m_body->accept(this);
}
