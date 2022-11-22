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
  virtual void visit(AST::Root &) = 0;
  virtual void visit(AST::PrimitiveType &) = 0;
  virtual void visit(AST::VarExpr &) = 0;
  virtual void visit(AST::BooleanExpr &) = 0;
  virtual void visit(AST::EqExpr &) = 0;
  virtual void visit(AST::CompExpr &) = 0;
  virtual void visit(AST::AddExpr &) = 0;
  virtual void visit(AST::SubExpr &) = 0;
  virtual void visit(AST::MultExpr &) = 0;
  virtual void visit(AST::DivExpr &) = 0;
  virtual void visit(AST::UnaryExpr &) = 0;
  virtual void visit(AST::FnCallExpr &) = 0;
  virtual void visit(AST::ClassInitExpr &) = 0;
  virtual void visit(AST::IntExpr &) = 0;
  virtual void visit(AST::FloatExpr &) = 0;
  virtual void visit(AST::BoolExpr &) = 0;
  virtual void visit(AST::StringExpr &) = 0;
  virtual void visit(AST::StmtBlock &) = 0;
  virtual void visit(AST::ReturnStmt &) = 0;
  virtual void visit(AST::BreakStmt &) = 0;
  virtual void visit(AST::ContinueStmt &) = 0;
  virtual void visit(AST::VarDeclStmt &) = 0;
  virtual void visit(AST::VarAssignStmt &) = 0;
  virtual void visit(AST::ExprStmt &) = 0;
  virtual void visit(AST::ReadStmt &) = 0;
  virtual void visit(AST::WriteStmt &) = 0;
  virtual void visit(AST::Param &) = 0;
  virtual void visit(AST::FnDef &) = 0;
  virtual void visit(AST::CtorDef &) = 0;
  virtual void visit(AST::DtorDef &) = 0;
  virtual void visit(AST::Field &) = 0;
  virtual void visit(AST::ClassDef &) = 0;
  virtual void visit(AST::WhileLoop &) = 0;
  virtual void visit(AST::ForLoop &) = 0;
  virtual void visit(AST::ForEachLoop &) = 0;
  virtual void visit(AST::IfStmt &) = 0;
  virtual void visit(AST::ElseStmt &) = 0;
  virtual void visit(AST::ElseIfStmt &) = 0;
};

}  // namespace Wisnia

#endif  // WISNIALANG_VISITOR_HPP
