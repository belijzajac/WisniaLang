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

#ifndef WISNIALANG_NODE_COLLECTOR_HPP
#define WISNIALANG_NODE_COLLECTOR_HPP

#include <vector>
#include <type_traits>
// Wisnia
#include "Visitor.hpp"

namespace Wisnia {

template <typename T>
class NodeCollector : public Visitor {
 public:
  NodeCollector() {
    static_assert(std::is_base_of<AST::Root, T>::value, "Type parameter of this class must derive from AST::Root");
  }

  std::vector<const T *> getNodes() const {
    return m_nodes;
  }

 public:
  void visit(AST::Root *node) override;
  void visit(AST::PrimitiveType *node) override;
  void visit(AST::VarExpr *node) override;
  void visit(AST::BooleanExpr *node) override;
  void visit(AST::EqExpr *node) override;
  void visit(AST::CompExpr *node) override;
  void visit(AST::AddExpr *node) override;
  void visit(AST::SubExpr *node) override;
  void visit(AST::MultExpr *node) override;
  void visit(AST::DivExpr *node) override;
  void visit(AST::UnaryExpr *node) override;
  void visit(AST::FnCallExpr *node) override;
  void visit(AST::ClassInitExpr *node) override;
  void visit(AST::IntExpr *node) override;
  void visit(AST::FloatExpr *node) override;
  void visit(AST::BoolExpr *node) override;
  void visit(AST::StringExpr *node) override;
  void visit(AST::StmtBlock *node) override;
  void visit(AST::ReturnStmt *node) override;
  void visit(AST::BreakStmt *node) override;
  void visit(AST::ContinueStmt *node) override;
  void visit(AST::VarDeclStmt *node) override;
  void visit(AST::VarAssignStmt *node) override;
  void visit(AST::ExprStmt *node) override;
  void visit(AST::ReadStmt *node) override;
  void visit(AST::WriteStmt *node) override;
  void visit(AST::Param *node) override;
  void visit(AST::FnDef *node) override;
  void visit(AST::CtorDef *node) override;
  void visit(AST::DtorDef *node) override;
  void visit(AST::Field *node) override;
  void visit(AST::ClassDef *node) override;
  void visit(AST::WhileLoop *node) override;
  void visit(AST::ForLoop *node) override;
  void visit(AST::ForEachLoop *node) override;
  void visit(AST::IfStmt *node) override;
  void visit(AST::ElseStmt *node) override;
  void visit(AST::ElseIfStmt *node) override;

 private:
  std::vector<const T *> m_nodes;
};

#include "NodeCollector.tpp"

}  // namespace Wisnia

#endif  // WISNIALANG_NODE_COLLECTOR_HPP
