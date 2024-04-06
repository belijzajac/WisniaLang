// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <array>
#include <cassert>
#include <ranges>
#include <algorithm>
// Wisnia
#include "AST.hpp"
#include "IRGenerator.hpp"
#include "Instruction.hpp"
#include "Modules.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace AST;

AST::Root &IRGenerator::popNode() {
  assert(!m_stack.empty() && "The stack is empty");
  auto &topNode{*m_stack.top()};
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

constexpr Operation getOperationForBinaryExpression(Basic::TType exprType, bool isFloat) {
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
      throw InstructionError{"Unknown binary expression"};
  }
}

constexpr TType getIdentForLiteralType(TType type) {
  switch (type) {
    case TType::LIT_INT:  return TType::IDENT_INT;
    case TType::LIT_FLT:  return TType::IDENT_FLOAT;
    case TType::LIT_STR:  return TType::IDENT_STRING;
    case TType::LIT_BOOL:
    case TType::KW_TRUE:
    case TType::KW_FALSE: return TType::IDENT_BOOL;
    default:              return type;
  }
}

void IRGenerator::createBinaryExpression(Basic::TType expressionType) {
  const auto &rhs = popNode();
  const auto &lhs = popNode();

  auto varToken = std::make_shared<Basic::Token>(
    getIdentForLiteralType(rhs.getToken()->getType()),
    "_t" + std::to_string(m_tempVars.size())
  );

  m_tempVars.emplace_back(std::make_unique<VarExpr>(varToken));
  m_stack.push(m_tempVars.back().get());

  bool isFloat = rhs.getToken()->getType() == TType::LIT_FLT ||
                 rhs.getToken()->getType() == TType::IDENT_FLOAT;
  Operation op = getOperationForBinaryExpression(expressionType, isFloat);

  // _tx = a <op> b rewritten as
  //    _tx = a
  //    _tx = _tx <op> b
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    varToken,       // _tx
    lhs.getToken()  // a
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    op,             // <op>
    varToken,       // _tx
    rhs.getToken()  // b
  ));
}

std::tuple<IRGenerator::TokenPtr, Basic::TType> IRGenerator::getExpression(Root &node, bool createVariableForLiteral) {
  IRGenerator::TokenPtr token;
  TType type;

  if (dynamic_cast<AST::BinaryExpr *>(&node)) {
    token = m_tempVars.back()->getToken();
    type  = m_tempVars.back()->getToken()->getType();
  } else if (dynamic_cast<AST::FnCallExpr *>(&node)) {
    type  = node.getToken()->getType();
    token = std::make_shared<Basic::Token>(
      getIdentForLiteralType(type),
      "_t" + std::to_string(m_tempVars.size())
    );
    m_tempVars.emplace_back(std::make_unique<VarExpr>(token));
    if (type != TType::IDENT_VOID) {
      // we expect a non-void function to return a value that is kept in the most distant register because:
      //   1. we push all registers (rax, ..., r15)
      //   2. we generate IRs for function call
      //   3. we pop all registers (r15, ..., rax) <-- thus return value is stored in r15
      auto functionReturn = std::make_shared<Basic::Token>(
        TType::REGISTER,
        Basic::register_t::R15
      );
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        token,
        functionReturn
      ));
    }
  } else if (node.getToken()->isIdentifierType()) {
    token = node.getToken();
    type  = token->getType();
  } else if (node.getToken()->isLiteralType()) {
    if (createVariableForLiteral) {
      const auto &litToken = node.getToken();
      auto varToken = std::make_shared<Basic::Token>(
        getIdentForLiteralType(litToken->getType()),
        "_t" + std::to_string(m_tempVars.size())
      );
      m_tempVars.emplace_back(std::make_unique<VarExpr>(varToken));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        varToken, // _tx
        litToken  // literal
      ));
      token = m_tempVars.back()->getToken();
      type  = m_tempVars.back()->getToken()->getType();
    } else {
      token = node.getToken();
      type  = node.getToken()->getType();
    }
  } else {
    assert(0 && "Unknown expression");
  }

  return {token, type};
}

void IRGenerator::visit(AST::Root &node) {
  size_t last{0};
  const auto &functions = node.getGlobalFunctions();
  for (const auto &function : std::ranges::reverse_view(functions)) {
    function->accept(*this);
    const size_t start = last;
    const size_t end   = last = m_instructions.size();
    if (m_allocateRegisters) {
      registerAllocator.allocate(vec_slice(m_instructions, start, end));
    }
  }

  // Load modules
  auto [moduleCalculateStringLength, moduleCalculateStringLengthUsed] = Modules::getModule(Module::CALCULATE_STRING_LENGTH);
  auto [modulePrintNumber, modulePrintNumberUsed] = Modules::getModule(Module::PRINT_NUMBER);
  auto [modulePrintBoolean, modulePrintBooleanUsed] = Modules::getModule(Module::PRINT_BOOLEAN);
  auto [moduleExit, moduleExitUsed] = Modules::getModule(Module::EXIT);

  if (moduleCalculateStringLengthUsed) registerAllocator.allocate(std::move(moduleCalculateStringLength), false);
  if (modulePrintNumberUsed) registerAllocator.allocate(std::move(modulePrintNumber), false);
  if (modulePrintBooleanUsed) registerAllocator.allocate(std::move(modulePrintBoolean), false);
  if (moduleExitUsed) registerAllocator.allocate(std::move(moduleExit), false);

  // Instruction optimization
  const auto &instructions = getInstructions(Transformation::REGISTER_ALLOCATION);
  irOptimization.optimize(vec_slice(instructions, 0, instructions.size()));
}

void IRGenerator::visit(AST::PrimitiveType &) {
}

void IRGenerator::visit(AST::VarExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(AST::BooleanExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(AST::EqExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(AST::CompExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(AST::AddExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(AST::SubExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(AST::MultExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(AST::DivExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(AST::UnaryExpr &node) {
  node.lhs()->accept(*this);
  throw NotImplementedError{fmt::format("Unary expressions are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::FnCallExpr &node) {
  node.getVariable()->accept(*this);
  constexpr auto registers = RegisterAllocator::getAllocatableRegisters;

  // suboptimal approach to avoid overriding registers inside the called function
  std::transform(registers.begin(), registers.end(), std::back_inserter(m_instructions), [&](auto reg) {
    return std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, reg)
    );
  });

  for (const auto &arg : node.getArguments()) {
    arg->accept(*this);
    const auto &[argToken, _] = getExpression(*arg);
    auto varToken = std::make_shared<Basic::Token>(
      getIdentForLiteralType(argToken->getType()),
      "_t" + std::to_string(m_tempVars.size())
    );
    m_tempVars.emplace_back(std::make_unique<VarExpr>(varToken));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::MOV,
      varToken, // _tx
      argToken  // arg
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      varToken
    ));
  }

  const auto &functionName = node.getFunctionName();
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::CALL,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, functionName->getValue<std::string>())
  ));

  // following the function call, restore old register values
  std::transform(std::ranges::rbegin(registers), std::ranges::rend(registers), std::back_inserter(m_instructions), [](auto reg) {
    return std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, reg)
    );
  });
}

void IRGenerator::visit(AST::ClassInitExpr &node) {
  node.getVariable()->accept(*this);
  for (const auto &arg : node.getArguments()) {
    arg->accept(*this);
  }
  throw NotImplementedError{fmt::format("Class initialization expressions are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::IntExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(AST::FloatExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(AST::BoolExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(AST::StringExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(AST::StmtBlock &node) {
  for (const auto &stmt : node.getStatements()) {
    stmt->accept(*this);
  }
}

void IRGenerator::visit(AST::ReturnStmt &node) {
  node.getReturnValue()->accept(*this);
  const auto &[token, _] = getExpression(*node.getReturnValue());
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    token
  ));
}

void IRGenerator::visit(AST::BreakStmt &node) {
  throw NotImplementedError{fmt::format("Break statements are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::ContinueStmt &node) {
  throw NotImplementedError{fmt::format("Continue statements are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::VarDeclStmt &node) {
  node.getValue()->accept(*this);
  const auto &[token, _] = getExpression(*node.getValue());
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    node.getVariable()->getToken(), // int a
    token                           // _tx
  ));
}

void IRGenerator::visit(AST::VarAssignStmt &node) {
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);

  const auto &[token, _] = getExpression(*node.getValue());
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    node.getVariable()->getToken(),
    token
  ));
}

void IRGenerator::visit(AST::ExprStmt &node) {
  node.getExpression()->accept(*this);
}

void IRGenerator::visit(AST::ReadStmt &node) {
  for (const auto &var : node.getVariableList()) {
    var->accept(*this);
  }
  throw NotImplementedError{fmt::format("Read statements are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

/*
  mov rdx, N         ;; length N of the string X
  mov rsi, X         ;; starting at the string X
  mov rax, 0x1       ;; write
  mov rdi, 0x1       ;; stdout file descriptor
  syscall            ;; make the system call
*/
void IRGenerator::visit(AST::WriteStmt &node) {
  for (const auto &expr : node.getExpressions()) {
    expr->accept(*this);
  }

  for (const auto &expr : node.getExpressions()) {
    const auto &[token, type] = getExpression(*expr, false);

    if (token->isIdentifierType()) {
      // Resolved at compiled program's run-time
      switch (type) {
        case TType::IDENT_STRING:
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::PUSH,
            nullptr,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDX)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::PUSH,
            nullptr,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RSI)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::MOV,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RSI),
            std::make_shared<Basic::Token>(type, token->getValue<std::string>())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::CALL,
            std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::CALCULATE_STRING_LENGTH].data())
          ));
          Modules::markAsUsed(Module::CALCULATE_STRING_LENGTH);
          break;
        case TType::IDENT_INT:
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::PUSH,
            nullptr,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::MOV,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI),
            std::make_shared<Basic::Token>(type, token->getValue<std::string>())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::CALL,
            std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::PRINT_NUMBER].data())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::POP,
            nullptr,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI)
          ));
          Modules::markAsUsed(Module::PRINT_NUMBER);
          continue;
        case TType::IDENT_BOOL:
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::PUSH,
            nullptr,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::MOV,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI),
            std::make_shared<Basic::Token>(type, token->getValue<std::string>())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::CALL,
            std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::PRINT_BOOLEAN].data())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::POP,
            nullptr,
            std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI)
          ));
          Modules::markAsUsed(Module::PRINT_BOOLEAN);
          continue;
        case TType::IDENT_FLOAT:
          throw InstructionError{fmt::format("Float variables are not supported in print statements in {}:{}",
                                             token->getPosition().getFileName(),
                                             token->getPosition().getLineNo())};
        default:
          throw InstructionError{fmt::format("Unknown variable type for print statement in {}:{}",
                                             token->getPosition().getFileName(),
                                             token->getPosition().getLineNo())};
      }
    }

    if (token->isLiteralType()) {
      // Resolved at "compile-time"
      const auto str = token->getValueStr();
      const auto length = (type == TType::LIT_STR) ? str.size() - 1 : str.size();
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::PUSH,
        nullptr,
        std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDX)
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::PUSH,
        nullptr,
        std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RSI)
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDX),
        std::make_shared<Basic::Token>(TType::LIT_INT, static_cast<int>(length))
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RSI),
        std::make_shared<Basic::Token>(TType::LIT_STR, str)
      ));
    }

    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RAX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RCX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::R11)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::MOV,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RAX),
      std::make_shared<Basic::Token>(TType::LIT_INT, 1)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::MOV,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI),
      std::make_shared<Basic::Token>(TType::LIT_INT, 1)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::SYSCALL
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDI)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::R11)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RCX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RAX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RSI)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Basic::Token>(TType::REGISTER, Basic::register_t::RDX)
    ));
  }
}

void IRGenerator::visit(AST::Param &node) {
  node.getVariable()->accept(*this);
  throw NotImplementedError{fmt::format("Function parameters are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::FnDef &node) {
  node.getVariable()->accept(*this);
  const auto &functionName = popNode();
  const auto functionNameStr = functionName.getToken()->getValue<std::string>();
  IRGenerator::TokenPtr returnAddressToken;

  if (functionNameStr != "main") {
    // the main function doesn't require a label indicating where it begins
    // because we have already designated address "0x4000b0" as the program's main entry point
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::LABEL,
      nullptr,
      std::make_shared<Basic::Token>(TType::IDENT_VOID, functionNameStr)
    ));
    // put the function return address into a variable because we'll be popping out the arguments
    // passed to the function in the later steps
    returnAddressToken = std::make_shared<Basic::Token>(
      TType::IDENT_INT,
      "_t" + std::to_string(m_tempVars.size())
    );
    m_tempVars.emplace_back(std::make_unique<VarExpr>(returnAddressToken));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      returnAddressToken
    ));
  }

  std::transform(node.getParameters().rbegin(), node.getParameters().rend(), std::back_inserter(m_instructions), [](const auto &param) {
    return std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      param->getToken()
    );
  });

  node.getBody()->accept(*this);

  if (functionNameStr != "main") {
    // put the function return address back on the stack
    // only functions other than "main" must return to the caller
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      returnAddressToken
    ));
  }

  if (functionNameStr == "main") {
    // the "ret" instruction isn't required in the main function
    // because we terminate the program immediately in the "_exit_" function
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::CALL,
      std::make_shared<Basic::Token>(TType::IDENT_VOID, Module2Str[Module::EXIT].data())
    ));
    Modules::markAsUsed(Module::EXIT);
  } else {
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::RET
    ));
  }
}

void IRGenerator::visit(AST::CtorDef &node) {
  throw NotImplementedError{fmt::format("Constructors are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::DtorDef &node) {
  throw NotImplementedError{fmt::format("Destructors are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::Field &node) {
  throw NotImplementedError{fmt::format("Class fields are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::ClassDef &node) {
  throw NotImplementedError{fmt::format("Classes are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::WhileLoop &node) {
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
  throw NotImplementedError{fmt::format("While loops are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::ForLoop &node) {
  node.getInitial()->accept(*this);
  node.getCondition()->accept(*this);
  node.getIncrement()->accept(*this);
  node.getBody()->accept(*this);
  throw NotImplementedError{fmt::format("For loops are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::ForEachLoop &node) {
  node.getElement()->accept(*this);
  node.getCollection()->accept(*this);
  node.getBody()->accept(*this);
  throw NotImplementedError{fmt::format("For-each loops are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(AST::IfStmt &node) {
  node.getCondition()->accept(*this);

  const auto &[token, _] = getExpression(*node.getCondition());
  const std::array<std::string_view, 3> labels {
    ".L" + std::to_string(m_labelCount) + "_true",
    ".L" + std::to_string(m_labelCount) + "_false",
    ".L" + std::to_string(m_labelCount) + "_end",
  };

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::TEST,
    nullptr,
    token,
    token
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JNZ,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, labels[0].data())
  ));

  for (const auto &elseBl : node.getElseStatements()) {
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::LABEL,
      nullptr,
      std::make_shared<Basic::Token>(TType::IDENT_VOID, labels[1].data())
    ));
    elseBl->accept(*this);
  }

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JMP,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, labels[2].data())
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, labels[0].data())
  ));

  node.getBody()->accept(*this);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Basic::Token>(TType::IDENT_VOID, labels[2].data())
  ));

  m_labelCount++;
}

void IRGenerator::visit(AST::ElseStmt &node) {
  node.getBody()->accept(*this);
}

void IRGenerator::visit(AST::ElseIfStmt &node) {
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
  throw NotImplementedError{fmt::format("Else-if statements are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}
