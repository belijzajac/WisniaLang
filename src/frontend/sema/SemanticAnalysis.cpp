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

#include <fmt/ostream.h>

#include <algorithm>
#include <iostream>
#include <map>
// Wisnia
#include "AST.hpp"
#include "SemanticAnalysis.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace AST;

void SemanticAnalysis::visit(Root &node) {
  for (const auto &klass : node.getGlobalClasses()) {
    klass->accept(*this);
  }
  for (const auto &function : node.getGlobalFunctions()) {
    m_functionChecks.reset();
    function->accept(*this);
  }

  if (!m_programChecks.mainFunctionFound) {
    throw SemanticError{"Function `main` not found"};
  }

  for (const auto &function : m_programChecks.functionDefinitions) {
    auto found = std::find(m_programChecks.invokedFunctions.begin(), m_programChecks.invokedFunctions.end(), function.name);
    if (function.name != "main" && found == m_programChecks.invokedFunctions.end()) {
      std::cout << fmt::format("[Warning] Function '{}' is defined but never used\n", function.name);
    }
  }

  std::map<std::string, size_t> functionOccurrenceMap;
  for (const auto &function : m_programChecks.functionDefinitions) {
    functionOccurrenceMap[function.name]++;
  }

  for (const auto &[function, count] : functionOccurrenceMap) {
    if (count > 1) {
      throw SemanticError{fmt::format("Found multiple definitions for `{}` function", function)};
    }
  }
}

void SemanticAnalysis::visit(AST::PrimitiveType &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::VarExpr &node) {
  auto foundVar = m_table.findSymbol(node.getToken()->getValue<std::string>()); // VarExpr
  node.addType(std::make_unique<PrimitiveType>(foundVar->getType()->getToken()));
}

void SemanticAnalysis::visit(AST::BooleanExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::EqExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::CompExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::AddExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::SubExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::MultExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::DivExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::UnaryExpr &node) {
  node.lhs()->accept(*this);
}

void SemanticAnalysis::visit(AST::FnCallExpr &node) {
  const auto functionName = node.getVar()->getToken()->getValue<std::string>();
  m_programChecks.invokedFunctions.emplace_back(functionName);

  const auto fnDefinition = std::find_if(
    m_programChecks.functionDefinitions.begin(), m_programChecks.functionDefinitions.end(),
    [&](const auto &function) { return function.name == functionName; });
  if (fnDefinition == m_programChecks.functionDefinitions.end()) {
    throw SemanticError{fmt::format("Failed to find function '{}' definition", functionName)};
  }

  if (node.getArgs().size() != fnDefinition->parameters.size()) {
    throw SemanticError{fmt::format("Function '{}' expects {} arguments but only {} were provided",
                                    fnDefinition->name,
                                    fnDefinition->parameters.size(),
                                    node.getArgs().size())};
  }

  node.getVar()->accept(*this);
  for (const auto &arg : node.getArgs()) {
    arg->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::ClassInitExpr &node) {
  node.getVar()->accept(*this);
  for (const auto &arg : node.getArgs()) {
    arg->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::IntExpr &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::FloatExpr &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::BoolExpr &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::StringExpr &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::StmtBlock &node) {
  m_table.pushScope();
  for (const auto &stmt : node.getStatements()) {
    stmt->accept(*this);
  }
  m_table.popScope();
}

void SemanticAnalysis::visit(AST::ReturnStmt &node) {
  m_functionChecks.returnFound = true;
  node.getReturnValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::BreakStmt &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::ContinueStmt &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::VarDeclStmt &node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVar().get()));
  node.getVar()->accept(*this);
  node.getValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::VarAssignStmt &node) {
  node.getVar()->accept(*this);
  node.getValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::ExprStmt &node) {
  node.getExpr()->accept(*this);
}

void SemanticAnalysis::visit(AST::ReadStmt &node) {
  for (const auto &var : node.getVars()) {
    var->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::WriteStmt &node) {
  for (const auto &expr : node.getExprs()) {
    expr->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::Param &node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVar().get()));
  node.getVar()->accept(*this);
}

void SemanticAnalysis::visit(AST::FnDef &node) {
  const auto functionName = node.getVar()->getToken()->getValue<std::string>();
  if (functionName == "main") {
    m_programChecks.mainFunctionFound = true;
  }

  std::vector<ProgramSemanticChecks::Function::Parameter> parameters;
  for (const auto &param : node.getParams()) {
    parameters.push_back({
      param->getToken()->getValue<std::string>(),
      param->getToken()->getType()
    });
  }

  m_programChecks.functionDefinitions.emplace_back(ProgramSemanticChecks::Function{
    .name = functionName,
    .parameters = std::move(parameters)
  });

  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVar().get()));
  node.getVar()->accept(*this);
  for (const auto &param : node.getParams()) {
    param->accept(*this);
  }
  node.getBody()->accept(*this);

  if (node.getVar()->getToken()->getType() != TType::IDENT_VOID && !m_functionChecks.returnFound) {
    throw SemanticError{fmt::format("Non-void function '{}' is not returning", functionName)};
  }
}

void SemanticAnalysis::visit(AST::CtorDef &) {
  throw NotImplementedError{"Constructors are not supported"};
}

void SemanticAnalysis::visit(AST::DtorDef &) {
  throw NotImplementedError{"Destructors are not supported"};
}

void SemanticAnalysis::visit(AST::Field &node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVar().get()));
  node.getVar()->accept(*this);
  node.getValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::ClassDef &node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVar().get()));
  node.getVar()->accept(*this);
  for (const auto &field : node.getFields()) {
    field->accept(*this);
  }
  if (node.getCtor()) node.getCtor()->accept(*this);
  if (node.getDtor()) node.getDtor()->accept(*this);
  for (const auto &method : node.getMethods()) {
    method->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::WhileLoop &node) {
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
}

void SemanticAnalysis::visit(AST::ForLoop &node) {
  node.getInitial()->accept(*this);
  node.getCondition()->accept(*this);
  node.getIncrement()->accept(*this);
  node.getBody()->accept(*this);
}

void SemanticAnalysis::visit(AST::ForEachLoop &node) {
  node.getElement()->accept(*this);
  node.getCollection()->accept(*this);
  node.getBody()->accept(*this);
}

void SemanticAnalysis::visit(AST::IfStmt &node) {
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
  for (const auto &elseBl : node.getElseStatements()) {
    elseBl->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::ElseStmt &node) {
  node.getBody()->accept(*this);
}

void SemanticAnalysis::visit(AST::ElseIfStmt &node) {
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
}
