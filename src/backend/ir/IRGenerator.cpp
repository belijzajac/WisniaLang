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

#include <array>
#include <cassert>
// Wisnia
#include "IRGenerator.hpp"
#include "Instruction.hpp"
#include "AST.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace AST;

AST::Root *IRGenerator::popNode() {
  assert(!m_stack.empty());
  auto *topNode{m_stack.top()};
  m_stack.pop();
  return topNode;
}

constexpr std::array<std::array<Operation, 2>, 12> binaryExprMapping {{
  {{Operation::IADD, Operation::FADD}},
  {{Operation::ISUB, Operation::FSUB}},
  {{Operation::IMUL, Operation::FMUL}},
  {{Operation::IDIV, Operation::FDIV}},
  {{Operation::IEQ,  Operation::FEQ }},
  {{Operation::ILT,  Operation::FLT }},
  {{Operation::ILE,  Operation::FLE }},
  {{Operation::IGT,  Operation::FGT }},
  {{Operation::IGE,  Operation::FGE }},
  {{Operation::INE,  Operation::FNE }},
  {{Operation::AND,  Operation::NOP }},
  {{Operation::OR,   Operation::NOP }}
}};

constexpr Operation getOperationForBinaryExpr(Basic::TType exprType, bool isFloat) {
  switch (exprType) {
    case Basic::TType::OP_ADD:
      return binaryExprMapping[0][isFloat];
    case Basic::TType::OP_SUB:
      return binaryExprMapping[1][isFloat];
    case Basic::TType::OP_MUL:
      return binaryExprMapping[2][isFloat];
    case Basic::TType::OP_DIV:
      return binaryExprMapping[3][isFloat];
    case Basic::TType::OP_EQ:
      return binaryExprMapping[4][isFloat];
    case Basic::TType::OP_L:
      return binaryExprMapping[5][isFloat];
    case Basic::TType::OP_LE:
      return binaryExprMapping[6][isFloat];
    case Basic::TType::OP_G:
      return binaryExprMapping[7][isFloat];
    case Basic::TType::OP_GE:
      return binaryExprMapping[8][isFloat];
    case Basic::TType::OP_NE:
      return binaryExprMapping[9][isFloat];
    case Basic::TType::OP_AND:
      return binaryExprMapping[10][0];
    case Basic::TType::OP_OR:
      return binaryExprMapping[11][0];
    default:
      throw InstructionError{"Undefined binary expression"};
  }
}

constexpr TType getIdentForLitType(TType type) {
  switch (type) {
    case TType::LIT_INT:  return TType::IDENT_INT;
    case TType::LIT_FLT:  return TType::IDENT_FLOAT;
    case TType::LIT_STR:  return TType::IDENT_STRING;
    case TType::LIT_BOOL: return TType::IDENT_BOOL;
    default: return type;
  }
}

void IRGenerator::genBinaryExpr(Basic::TType exprType) {
  auto rhs = popNode();
  auto lhs = popNode();

  auto varToken = std::make_shared<Basic::Token>(
    getIdentForLitType(rhs->getToken()->getType()),
    "_t" + std::to_string(m_tempVars.size())
  );

  m_tempVars.emplace_back(std::make_unique<VarExpr>(varToken));
  m_stack.push(m_tempVars.back().get());

  bool isFloat = rhs->getToken()->getType() == TType::LIT_FLT ||
                 rhs->getToken()->getType() == TType::IDENT_FLOAT;
  Operation op = getOperationForBinaryExpr(exprType, isFloat);

  // _tx = a <op> b;
  m_instructions.emplace_back(std::make_unique<Instruction>(
    op,              // <op>
    varToken,        // _tx
    lhs->getToken(), // b
    rhs->getToken()  // a
  ));
}

void IRGenerator::visit(AST::Root *node) {
  size_t last{0};
  for (const auto &function : node->getGlobalFunctions()) {
    function->accept(this);
    const size_t start = last;
    const size_t end   = last = m_instructions.size();
    if (m_allocateRegisters) {
      registerAllocator.allocateRegisters(vec_slice(m_instructions, start, end));
    }
  }

  /*
    mov rax, 0x3c      ;; exit
    mov rdi, 0x0       ;; exit code is 0
    syscall            ;; make the system call
  */
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rdi"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 0)
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    std::make_shared<Basic::Token>(TType::REGISTER, "rax"),
    std::make_shared<Basic::Token>(TType::LIT_INT, 60)
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::SYSCALL
  ));

  // Insert the last two instructions
  if (m_allocateRegisters) {
    registerAllocator.allocateRegisters(vec_slice(m_instructions, m_instructions.size() - 3, m_instructions.size()));
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
  genBinaryExpr(node->getToken()->getType());
}

void IRGenerator::visit(AST::EqExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
  genBinaryExpr(node->getToken()->getType());
}

void IRGenerator::visit(AST::CompExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
  genBinaryExpr(node->getToken()->getType());
}

void IRGenerator::visit(AST::AddExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
  genBinaryExpr(node->getToken()->getType());
}

void IRGenerator::visit(AST::SubExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
  genBinaryExpr(node->getToken()->getType());
}

void IRGenerator::visit(AST::MultExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
  genBinaryExpr(node->getToken()->getType());
}

void IRGenerator::visit(AST::DivExpr *node) {
  node->lhs()->accept(this);
  node->rhs()->accept(this);
  genBinaryExpr(node->getToken()->getType());
}

void IRGenerator::visit(AST::UnaryExpr *node) {
  node->lhs()->accept(this);
  // TODO: similar to `genBinaryExpr`
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
    Operation::MOV,
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

/*
  mov rdx, N         ;; length N of the string X
  mov rsi, X         ;; starting at the string X
  mov rax, 0x1       ;; write
  mov rdi, 0x1       ;; stdout file descriptor
  syscall            ;; make the system call
*/
void IRGenerator::visit(AST::WriteStmt *node) {
  //for (const auto &expr : node->getExprs()) {
  //  expr->accept(this);
  //}

  // todo: what do we do about binary expressions that have both lhs and rhs values ???
  for (const auto &expr : node->getExprs()) {
    //popNode();

    const auto &token = expr->getToken();
    const auto &type  = token->getType();

    const auto satisfiesIdentifierType {
      type == TType::IDENT_INT    || type == TType::IDENT_FLOAT ||
      type == TType::IDENT_STRING || type == TType::IDENT_BOOL
    };
    const auto satisfiesLiteralType {
      type == TType::LIT_INT || type == TType::LIT_FLT ||
      type == TType::LIT_STR || type == TType::LIT_BOOL
    };

    if (satisfiesIdentifierType) {
      // Resolved at compiled program's run-time
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Basic::Token>(TType::REGISTER, "rdx"), // todo: push old rdx value on the stack
        std::make_shared<Basic::Token>(TType::LIT_INT, 5) // print 5 bytes
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
        std::make_shared<Basic::Token>(type, token->getValue<std::string>())
      ));
    } else if (satisfiesLiteralType) {
      // Resolved at "compile-time"
      const auto str = token->getValueStr();
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Basic::Token>(TType::REGISTER, "rdx"),
        std::make_shared<Basic::Token>(TType::LIT_INT, static_cast<int>(str.size()))
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Basic::Token>(TType::REGISTER, "rsi"),
        std::make_shared<Basic::Token>(TType::LIT_STR, str)
      ));
    } else {
      throw std::runtime_error{"fuck"};
    }

    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::MOV,
      std::make_shared<Basic::Token>(TType::REGISTER, "rax"),
      std::make_shared<Basic::Token>(TType::LIT_INT, 1)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::MOV,
      std::make_shared<Basic::Token>(TType::REGISTER, "rdi"),
      std::make_shared<Basic::Token>(TType::LIT_INT, 1)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::SYSCALL
    ));
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
