// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_IR_GENERATOR_HPP
#define WISNIALANG_IR_GENERATOR_HPP

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
  using TokenPtr = std::shared_ptr<Basic::Token>;
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

  enum class Transformation {
    NONE,
    REGISTER_ALLOCATION,
    INSTRUCTION_OPTIMIZATION
  };

  const InstructionList &getInstructions(Transformation transform) const {
    switch (transform) {
      case Transformation::NONE:
        return m_instructions;
      case Transformation::REGISTER_ALLOCATION:
        return registerAllocator.getInstructions();
      case Transformation::INSTRUCTION_OPTIMIZATION:
        return irOptimization.getInstructions();
      default:
        assert(0 && "Unknown transformation type");
    }
  }

  const TemporaryVariableList &getTemporaryVars() const {
    return m_tempVars;
  }

  void printInstructions(std::ostream &output, Transformation transform) const {
    switch (transform) {
      case Transformation::NONE:
        IRPrintHelper::print(output, m_instructions);
        break;
      case Transformation::REGISTER_ALLOCATION:
        IRPrintHelper::print(output, registerAllocator.getInstructions());
        break;
      case Transformation::INSTRUCTION_OPTIMIZATION:
        IRPrintHelper::print(output, irOptimization.getInstructions());
        break;
      default:
        assert(0 && "Unknown transformation type");
    }
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
  AST::Root &popNode();
  void createBinaryExpression(Basic::TType expressionType);

  // In general, we want literal types to be associated with variables, but in the case of
  // "AST::WriteStmt", we perform a compile-time optimization to prevent loading modules for
  // printing variables whose values we already know at compile time
  std::tuple<TokenPtr, Basic::TType> getExpression(AST::Root &node, bool createVariableForLiteral = true);

 private:
  std::stack<AST::Root *> m_stack;
  InstructionList m_instructions;
  TemporaryVariableList m_tempVars;
  bool m_allocateRegisters; // We wish to skip register allocation in some unit tests
  RegisterAllocator registerAllocator{};
  IROptimization irOptimization{};
  size_t m_labelCount{0};
};

}  // namespace Wisnia

#endif  // WISNIALANG_IR_GENERATOR_HPP
