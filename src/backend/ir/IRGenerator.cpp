#include <cassert>
// Wisnia
#include "IRGenerator.hpp"
#include "Instruction.hpp"
#include "AST.hpp"

using namespace Wisnia;
using namespace Utils;
using namespace AST;

void IRGenerator::printInstructions() const {
  size_t index = 0;
  fmt::print("{:^17}|{:^5}|{:^17}|{:^17}\n", "Target", "Op", "Arg1", "Arg2");
  fmt::print("-----------------+-----+-----------------+-----------------\n");
  for (const auto &ir : m_instructions) {
    ir->print();
    ++index;
  }
}

AST::Root *IRGenerator::popNode() {
  assert(!m_stack.empty());
  auto *topNode{m_stack.top()};
  m_stack.pop();
  return topNode;
}

void IRGenerator::visit(AST::Root *node) {
  for (const auto &function : node->getGlobalFunctions()) {
    function->accept(this);
  }
}

void IRGenerator::visit(AST::PrimitiveType *node) {
}

void IRGenerator::visit(AST::VarExpr *node) {
  m_stack.push(node);
}

void IRGenerator::visit(AST::BooleanExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void IRGenerator::visit(AST::EqExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void IRGenerator::visit(AST::CompExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void IRGenerator::visit(AST::AddExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);

  auto rhs = popNode();
  auto lhs = popNode();
  auto varToken = std::make_shared<Basic::Token>(
    Basic::TType::IDENT,
    "_t" + std::to_string(m_tempVars.size())
  );

  m_tempVars.emplace_back(std::make_unique<VarExpr>(varToken));
  m_stack.push(m_tempVars.back().get());

  // _tx = a + b;
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operator::ADD,   // +
    varToken,        // _tx
    rhs->getToken(), // a
    lhs->getToken()  // b
  ));
}

void IRGenerator::visit(AST::MultExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);

  auto rhs = popNode();
  auto lhs = popNode();
  auto varToken = std::make_shared<Basic::Token>(
    Basic::TType::IDENT,
    "_t" + std::to_string(m_tempVars.size())
  );

  m_tempVars.emplace_back(std::make_unique<VarExpr>(varToken));
  m_stack.push(m_tempVars.back().get());

  // _tx = a * b;
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operator::MUL,   // *
    varToken,        // _tx
    rhs->getToken(), // a
    lhs->getToken()  // b
  ));
}

void IRGenerator::visit(AST::UnaryExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
}

void IRGenerator::visit(AST::FnCallExpr *node) {
  node->getVar()->accept(this);
  for (const auto &arg : node->getArgs()) {
    arg->accept(this);
  }
}

void IRGenerator::visit(AST::ClassInitExpr *node) {
  node->getVar()->accept(this);
  for (const auto &arg : node->getArgs()) {
    arg->accept(this);
  }
}

void IRGenerator::visit(AST::IntExpr *node) {
  m_stack.push(node);
}

void IRGenerator::visit(AST::FloatExpr *node) {
  m_stack.push(node);
}

void IRGenerator::visit(AST::BoolExpr *node) {
  m_stack.push(node);
}

void IRGenerator::visit(AST::StringExpr *node) {
  m_stack.push(node);
}

void IRGenerator::visit(AST::StmtBlock *node) {
  for (const auto &stmt : node->getStatements()) {
    stmt->accept(this);
  }
}

void IRGenerator::visit(AST::ReturnStmt *node) {
  node->getReturnValue()->accept(this);
}

void IRGenerator::visit(AST::BreakStmt *node) {
}

void IRGenerator::visit(AST::ContinueStmt *node) {
}

void IRGenerator::visit(AST::VarDeclStmt *node) {
  node->getVar()->accept(this);
  node->getValue()->accept(this);

  auto rhs = popNode(); // _tx
  // int a = _tx
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operator::MOV,
    std::make_shared<Basic::Token>(
      node->getVar()->getToken()->getType(),
      node->getVar()->getToken()->getValue<std::string>()
    ), // int a
    rhs->getToken()
  ));
}

void IRGenerator::visit(AST::VarAssignStmt *node) {
  node->getVar()->accept(this);
  node->getValue()->accept(this);
}

void IRGenerator::visit(AST::ExprStmt *node) {
  node->getExpr()->accept(this);
}

void IRGenerator::visit(AST::ReadStmt *node) {
  for (const auto &var : node->getVars()) {
    var->accept(this);
  }
}

void IRGenerator::visit(AST::WriteStmt *node) {
  for (const auto &expr : node->getExprs()) {
    expr->accept(this);
  }
}

void IRGenerator::visit(AST::Param *node) {
  node->getVar()->accept(this);
}

void IRGenerator::visit(AST::FnDef *node) {
  node->getVar()->accept(this);
  for (const auto &param : node->getParams()) {
    param->accept(this);
  }
  node->getBody()->accept(this);
}

void IRGenerator::visit(AST::CtorDef *node) {
  throw NotImplementedError{"Constructors are not supported"};
}

void IRGenerator::visit(AST::DtorDef *node) {
  throw NotImplementedError{"Destructors are not supported"};
}

void IRGenerator::visit(AST::Field *node) {
  throw NotImplementedError{"Class fields are not supported"};
}

void IRGenerator::visit(AST::ClassDef *node) {
  throw NotImplementedError{"Classes are not supported"};
}

void IRGenerator::visit(AST::WhileLoop *node) {
  node->getCondition()->accept(this);
  node->getBody()->accept(this);
}

void IRGenerator::visit(AST::ForLoop *node) {
  node->getInitial()->accept(this);
  node->getCondition()->accept(this);
  node->getIncrement()->accept(this);
  node->getBody()->accept(this);
}

void IRGenerator::visit(AST::ForEachLoop *node) {
  node->getElement()->accept(this);
  node->getCollection()->accept(this);
  node->getBody()->accept(this);
}

void IRGenerator::visit(AST::IfStmt *node) {
  node->getCondition()->accept(this);
  node->getBody()->accept(this);
  for (const auto &elseBl : node->getElseStatements()) {
    elseBl->accept(this);
  }
}

void IRGenerator::visit(AST::ElseStmt *node) {
  node->getBody()->accept(this);
}

void IRGenerator::visit(AST::ElseIfStmt *node) {
  node->getCondition()->accept(this);
  node->getBody()->accept(this);
}
