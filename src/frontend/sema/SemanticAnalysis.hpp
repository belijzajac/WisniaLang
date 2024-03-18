// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_SEMANTIC_ANALYSIS_HPP
#define WISNIALANG_SEMANTIC_ANALYSIS_HPP

// Wisnia
#include "SymbolTable.hpp"
#include "Visitor.hpp"

namespace Wisnia {

class SemanticAnalysis : public Visitor {
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
  struct ProgramSemanticChecks {
    struct Function {
      struct Parameter {
        std::string name;
        Basic::TType type;
      };

      std::string name;
      std::vector<Parameter> parameters;
    };

    bool mainFunctionFound;
    std::vector<Function> functionDefinitions;
    std::vector<std::string> invokedFunctions;
  } m_programChecks;

  struct FunctionSemanticChecks {
    enum class ReturnType { NOT_FOUND, NONE, INT, FLOAT, STRING, BOOLEAN };

    void reset() {
      returnFound = false;
      returnType = ReturnType::NOT_FOUND;
    }

    bool returnFound;
    ReturnType returnType{ReturnType::NOT_FOUND};
  } m_functionChecks;

 private:
  SymbolTable m_table;
};

}  // namespace Wisnia

#endif  // WISNIALANG_SEMANTIC_ANALYSIS_HPP
