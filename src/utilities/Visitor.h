#ifndef VISITOR_H
#define VISITOR_H

namespace Wisnia {

namespace AST {
class Root;
class PrimitiveType;
class VarExpr;
class BooleanExpr;
class EqExpr;
class CompExpr;
class AddExpr;
class MultExpr;
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
  virtual void visit(AST::MultExpr *node) = 0;
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

#endif  // VISITOR_H
