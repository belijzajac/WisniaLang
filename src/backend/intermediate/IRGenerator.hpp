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

#ifndef WISNIALANG_IRGENERATOR_HPP
#define WISNIALANG_IRGENERATOR_HPP

#include <memory>
#include <vector>
#include <stack>
#include <variant>
// Wisnia
#include "IROptimization.hpp"
#include "IRPrintHelper.hpp"
#include "RegisterAllocator.hpp"
#include "TType.hpp"
#include "Visitor.hpp"

namespace Wisnia {
class Instruction;

class IRGenerator : public Visitor {
  using InstructionList = std::vector<std::shared_ptr<Instruction>>;
  using TemporaryVariableList = std::vector<std::shared_ptr<AST::VarExpr>>;

  template <typename T>
  std::vector<T> vec_slice(const std::vector<T> &vec, size_t start, size_t end) {
    const auto first = vec.begin() + start;
    const auto last  = vec.begin() + end;
    return std::vector<T>{first, last};
  }

 public:
  explicit IRGenerator(bool allocateRegisters = true)
      : m_allocateRegisters{allocateRegisters} {}

  const InstructionList &getInstructions() const {
    return m_instructions;
  }

  const InstructionList &getInstructionsAfterRegisterAllocation() const {
    return registerAllocator.getInstructions();
  }

  const InstructionList &getInstructionsAfterInstructionOptimization() const {
    return irOptimization.getInstructions();
  }

  const TemporaryVariableList &getTemporaryVars() const {
    return m_tempVars;
  }

  void printInstructions(std::ostream &output) const {
    IRPrintHelper::print(output, m_instructions);
  }

  void printInstructionsAfterRegisterAllocation(std::ostream &output) const {
    IRPrintHelper::print(output, registerAllocator.getInstructions());
  }

  void printInstructionsAfterInstructionOptimization(std::ostream &output) const {
    IRPrintHelper::print(output, irOptimization.getInstructions());
  }

 public: // TODO: why are these public???
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
  AST::Root *popNode();
  void genBinaryExpr(Basic::TType exprType);
  std::tuple<std::shared_ptr<Basic::Token>, Basic::TType> getExpression(AST::Root *node);

 private:
  std::stack<AST::Root *> m_stack;
  InstructionList m_instructions;
  TemporaryVariableList m_tempVars;
  bool m_allocateRegisters; // we want to skip register allocation in some unit tests
  RegisterAllocator registerAllocator{};
  IROptimization irOptimization{};
  size_t m_labelCount{0};
};

}  // namespace Wisnia

#endif  // WISNIALANG_IRGENERATOR_HPP
