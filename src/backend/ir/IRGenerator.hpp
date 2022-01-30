#ifndef WISNIALANG_IRGENERATOR_HPP
#define WISNIALANG_IRGENERATOR_HPP

#include <memory>
#include <vector>
#include <stack>
#include <variant>
// Wisnia
#include "Visitor.hpp"
#include "Instruction.hpp"

namespace Wisnia {

class IRGenerator : public Visitor {
 public:
  const std::vector<std::unique_ptr<Instruction>> &getInstructions() const {
    return m_instructions;
  }

  void printInstructions() const;

 public: // TODO: why are these public???
  void visit(AST::Root *node) override;
  void visit(AST::PrimitiveType *node) override;
  void visit(AST::VarExpr *node) override;
  void visit(AST::BooleanExpr *node) override;
  void visit(AST::EqExpr *node) override;
  void visit(AST::CompExpr *node) override;
  void visit(AST::AddExpr *node) override;
  void visit(AST::MultExpr *node) override;
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
  using node_t = std::variant<
    AST::VarExpr *,
    AST::IntExpr *,
    AST::FloatExpr *,
    AST::BoolExpr *,
    AST::StringExpr *
  >;
  std::stack<node_t> m_stack;
  std::vector<std::unique_ptr<Instruction>> m_instructions;
  std::vector<std::unique_ptr<AST::VarExpr>> m_tempVars;
};

}  // namespace Wisnia

#endif  // WISNIALANG_IRGENERATOR_HPP
