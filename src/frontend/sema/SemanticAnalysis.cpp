// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

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

struct Parameter {
  std::string name;
  TType type;
};

struct Function {
  std::string name;
  std::vector<Parameter> parameters;
};

struct ProgramSemanticChecks {
  bool mainFunctionFound{false};
  std::vector<Function> functionDefinitions;
  std::vector<std::string> invokedFunctions;

  void clear() {
    mainFunctionFound = false;
    functionDefinitions.clear();
    invokedFunctions.clear();
  }
} programChecks;

struct FunctionSemanticChecks {
  enum class ReturnType {
    NOT_FOUND,
    NONE,
    INT,
    FLOAT,
    STRING,
    BOOLEAN
  };

  void clear() {
    returnFound = false;
    returnType = ReturnType::NOT_FOUND;
  }

  bool returnFound{false};
  ReturnType returnType{ReturnType::NOT_FOUND};
} functionChecks;

void SemanticAnalysis::visit(Root &node) {
  programChecks.clear();

  for (const auto &klass : node.getGlobalClasses()) {
    klass->accept(*this);
  }
  for (const auto &function : node.getGlobalFunctions()) {
    functionChecks.clear();
    function->accept(*this);
  }

  if (!programChecks.mainFunctionFound) {
    throw SemanticError{"Function `main` not found"};
  }

  for (const auto &function : programChecks.functionDefinitions) {
    auto found = std::find(programChecks.invokedFunctions.begin(), programChecks.invokedFunctions.end(), function.name);
    if (function.name != "main" && found == programChecks.invokedFunctions.end()) {
      std::cout << fmt::format("[Warning] Function '{}' is defined but never used\n", function.name);
    }
  }

  std::map<std::string, size_t> functionOccurrenceMap;
  for (const auto &function : programChecks.functionDefinitions) {
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
  const auto functionName = node.getVariable()->getToken()->getValue<std::string>();
  programChecks.invokedFunctions.emplace_back(functionName);

  const auto fnDefinition = std::find_if(
    programChecks.functionDefinitions.begin(), programChecks.functionDefinitions.end(),
    [&](const auto &function) { return function.name == functionName; });
  if (fnDefinition == programChecks.functionDefinitions.end()) {
    throw SemanticError{fmt::format("Failed to find function '{}' definition", functionName)};
  }

  if (node.getArguments().size() != fnDefinition->parameters.size()) {
    throw SemanticError{fmt::format("Function '{}' expects {} arguments but only {} were provided",
                                    fnDefinition->name,
                                    fnDefinition->parameters.size(),
                                    node.getArguments().size())};
  }

  node.getVariable()->accept(*this);
  for (const auto &arg : node.getArguments()) {
    arg->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::ClassInitExpr &node) {
  node.getVariable()->accept(*this);
  for (const auto &arg : node.getArguments()) {
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
  functionChecks.returnFound = true;
  node.getReturnValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::BreakStmt &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::ContinueStmt &) {
  // nothing to do
}

void SemanticAnalysis::visit(AST::VarDeclStmt &node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVariable().get()));
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::VarAssignStmt &node) {
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::ExprStmt &node) {
  node.getExpression()->accept(*this);
}

void SemanticAnalysis::visit(AST::ReadStmt &node) {
  for (const auto &var : node.getVariables()) {
    var->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::WriteStmt &node) {
  for (const auto &expr : node.getExpressions()) {
    expr->accept(*this);
  }
}

void SemanticAnalysis::visit(AST::Param &node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVariable().get()));
  node.getVariable()->accept(*this);
}

void SemanticAnalysis::visit(AST::FnDef &node) {
  const auto functionName = node.getVariable()->getToken()->getValue<std::string>();
  if (functionName == "main") {
    programChecks.mainFunctionFound = true;
  }

  std::vector<Parameter> parameters;
  std::transform(node.getParameters().begin(), node.getParameters().end(), std::back_inserter(parameters), [](const auto &param) {
    return Parameter {
      param->getToken()->template getValue<std::string>(),
      param->getToken()->getType()
    };
  });

  programChecks.functionDefinitions.emplace_back(Function{
    .name = functionName,
    .parameters = std::move(parameters)
  });

  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVariable().get()));
  node.getVariable()->accept(*this);
  for (const auto &param : node.getParameters()) {
    param->accept(*this);
  }
  node.getBody()->accept(*this);

  if (node.getVariable()->getToken()->getType() != TType::IDENT_VOID && !functionChecks.returnFound) {
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
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVariable().get()));
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);
}

void SemanticAnalysis::visit(AST::ClassDef &node) {
  m_table.addSymbol(dynamic_cast<VarExpr *>(node.getVariable().get()));
  node.getVariable()->accept(*this);
  for (const auto &field : node.getFields()) {
    field->accept(*this);
  }
  if (node.getConstructor()) node.getConstructor()->accept(*this);
  if (node.getDestructor()) node.getDestructor()->accept(*this);
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
