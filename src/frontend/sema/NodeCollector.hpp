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

 private:
  void visit(AST::Root &) override;
  void visit(AST::PrimitiveType &) override;
  void visit(AST::VarExpr &) override;
  void visit(AST::BooleanExpr &) override;
  void visit(AST::EqExpr &) override;
  void visit(AST::CompExpr &) override;
  void visit(AST::AddExpr &) override;
  void visit(AST::SubExpr &) override;
  void visit(AST::MultExpr &) override;
  void visit(AST::DivExpr &) override;
  void visit(AST::UnaryExpr &) override;
  void visit(AST::FnCallExpr &) override;
  void visit(AST::ClassInitExpr &) override;
  void visit(AST::IntExpr &) override;
  void visit(AST::FloatExpr &) override;
  void visit(AST::BoolExpr &) override;
  void visit(AST::StringExpr &) override;
  void visit(AST::StmtBlock &) override;
  void visit(AST::ReturnStmt &) override;
  void visit(AST::BreakStmt &) override;
  void visit(AST::ContinueStmt &) override;
  void visit(AST::VarDeclStmt &) override;
  void visit(AST::VarAssignStmt &) override;
  void visit(AST::ExprStmt &) override;
  void visit(AST::ReadStmt &) override;
  void visit(AST::WriteStmt &) override;
  void visit(AST::Param &) override;
  void visit(AST::FnDef &) override;
  void visit(AST::CtorDef &) override;
  void visit(AST::DtorDef &) override;
  void visit(AST::Field &) override;
  void visit(AST::ClassDef &) override;
  void visit(AST::WhileLoop &) override;
  void visit(AST::ForLoop &) override;
  void visit(AST::ForEachLoop &) override;
  void visit(AST::IfStmt &) override;
  void visit(AST::ElseStmt &) override;
  void visit(AST::ElseIfStmt &) override;

 private:
  std::vector<const T *> m_nodes;
};

#include "NodeCollector.tpp"

}  // namespace Wisnia

#endif  // WISNIALANG_NODE_COLLECTOR_HPP
