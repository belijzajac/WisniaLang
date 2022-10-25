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

#ifndef WISNIALANG_VISITOR_HPP
#define WISNIALANG_VISITOR_HPP

namespace Wisnia {

namespace AST {
class Root;
class PrimitiveType;
class VarExpr;
class BooleanExpr;
class EqExpr;
class CompExpr;
class AddExpr;
class SubExpr;
class MultExpr;
class DivExpr;
class UnaryExpr;
class FnCallExpr;
class ClassInitExpr;
class IntExpr;
class FloatExpr;
class BoolExpr;
class StringExpr;
class StmtBlock;
class ReturnStmt;
class ContinueStmt;
class BreakStmt;
class VarDeclStmt;
class VarAssignStmt;
class ExprStmt;
class ReadStmt;
class WriteStmt;
class Param;
class FnDef;
class CtorDef;
class DtorDef;
class Field;
class ClassDef;
class WhileLoop;
class ForLoop;
class ForEachLoop;
class IfStmt;
class ElseStmt;
class ElseIfStmt;
}  // namespace AST

class Visitor {
 public:
  virtual void visit(AST::Root *node) = 0;
  virtual void visit(AST::PrimitiveType *node) = 0;
  virtual void visit(AST::VarExpr *node) = 0;
  virtual void visit(AST::BooleanExpr *node) = 0;
  virtual void visit(AST::EqExpr *node) = 0;
  virtual void visit(AST::CompExpr *node) = 0;
  virtual void visit(AST::AddExpr *node) = 0;
  virtual void visit(AST::SubExpr *node) = 0;
  virtual void visit(AST::MultExpr *node) = 0;
  virtual void visit(AST::DivExpr *node) = 0;
  virtual void visit(AST::UnaryExpr *node) = 0;
  virtual void visit(AST::FnCallExpr *node) = 0;
  virtual void visit(AST::ClassInitExpr *node) = 0;
  virtual void visit(AST::IntExpr *node) = 0;
  virtual void visit(AST::FloatExpr *node) = 0;
  virtual void visit(AST::BoolExpr *node) = 0;
  virtual void visit(AST::StringExpr *node) = 0;
  virtual void visit(AST::StmtBlock *node) = 0;
  virtual void visit(AST::ReturnStmt *node) = 0;
  virtual void visit(AST::BreakStmt *node) = 0;
  virtual void visit(AST::ContinueStmt *node) = 0;
  virtual void visit(AST::VarDeclStmt *node) = 0;
  virtual void visit(AST::VarAssignStmt *node) = 0;
  virtual void visit(AST::ExprStmt *node) = 0;
  virtual void visit(AST::ReadStmt *node) = 0;
  virtual void visit(AST::WriteStmt *node) = 0;
  virtual void visit(AST::Param *node) = 0;
  virtual void visit(AST::FnDef *node) = 0;
  virtual void visit(AST::CtorDef *node) = 0;
  virtual void visit(AST::DtorDef *node) = 0;
  virtual void visit(AST::Field *node) = 0;
  virtual void visit(AST::ClassDef *node) = 0;
  virtual void visit(AST::WhileLoop *node) = 0;
  virtual void visit(AST::ForLoop *node) = 0;
  virtual void visit(AST::ForEachLoop *node) = 0;
  virtual void visit(AST::IfStmt *node) = 0;
  virtual void visit(AST::ElseStmt *node) = 0;
  virtual void visit(AST::ElseIfStmt *node) = 0;
};

}  // namespace Wisnia

#endif  // WISNIALANG_VISITOR_HPP
