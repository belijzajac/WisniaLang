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

Root &IRGenerator::popNode() {
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

constexpr Operation getOperationForBinaryExpression(const TType exprType, const bool isFloat) {
  switch (exprType) {
    case TType::OP_ADD:
      return binaryExprMapping[0][isFloat];
    case TType::OP_SUB:
      return binaryExprMapping[1][isFloat];
    case TType::OP_MUL:
      return binaryExprMapping[2][isFloat];
    case TType::OP_DIV:
      return binaryExprMapping[3][isFloat];
    case TType::OP_EQ:
      return binaryExprMapping[4][isFloat];
    case TType::OP_L:
      return binaryExprMapping[5][isFloat];
    case TType::OP_LE:
      return binaryExprMapping[6][isFloat];
    case TType::OP_G:
      return binaryExprMapping[7][isFloat];
    case TType::OP_GE:
      return binaryExprMapping[8][isFloat];
    case TType::OP_NE:
      return binaryExprMapping[9][isFloat];
    case TType::OP_AND:
      return binaryExprMapping[10][0];
    case TType::OP_OR:
      return binaryExprMapping[11][0];
    default:
      throw InstructionError{"Unknown binary expression"};
  }
}

constexpr TType getIdentForLiteralType(const TType type) {
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

void IRGenerator::createBinaryExpression(const TType expressionType) {
  const auto &rhs = popNode();
  const auto &lhs = popNode();

  const auto rhsType = rhs.getToken()->getType();
  const bool isFloat = rhsType == TType::LIT_FLT || rhsType == TType::IDENT_FLOAT;
  const Operation op = getOperationForBinaryExpression(expressionType, isFloat);

  auto varToken = std::make_shared<Token>(
    getIdentForLiteralType(rhsType),
    "_t" + std::to_string(m_tempVars.size())
  );

  m_tempVars.emplace_back(std::make_unique<VarExpr>(varToken));
  m_stack.push(m_tempVars.back().get());

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

std::unordered_map<Operation, Operation> jumpConditionMap1 = {
  {Operation::IGT, Operation::JLE},
  {Operation::IGE, Operation::JL },
  {Operation::ILT, Operation::JGE},
  {Operation::ILE, Operation::JG },
  {Operation::IEQ, Operation::JNE},
  {Operation::INE, Operation::JE },
};

std::unordered_map<Operation, Operation> jumpConditionMap2 = {
  {Operation::IGT, Operation::JG },
  {Operation::IGE, Operation::JGE},
  {Operation::ILT, Operation::JL },
  {Operation::ILE, Operation::JLE},
  {Operation::IEQ, Operation::JE },
  {Operation::INE, Operation::JNE},
};

Operation IRGenerator::createJumpOpFromCondition(Root &node, const bool opposite) {
  const auto comparisonOp = m_comparisonOp.empty() ? Operation::NOP : m_comparisonOp.top();
  if (!m_comparisonOp.empty()) m_comparisonOp.pop();

  if (opposite ? jumpConditionMap1.contains(comparisonOp) : jumpConditionMap2.contains(comparisonOp)) {
    // if (register <op> number)
    // the visitor pattern goes through either the EqExpr or CompExpr case
    return opposite ? jumpConditionMap1[comparisonOp] : jumpConditionMap2[comparisonOp];
  }

  // if (register) ==> if (register > 0)
  const auto &[token, _] = getExpression(node);
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::CMP,
    nullptr,
    token,
    std::make_shared<Token>(TType::LIT_INT, 0)
  ));
  return Operation::JE;
}

std::tuple<IRGenerator::TokenPtr, TType> IRGenerator::getExpression(Root &node, const bool createVariableForLiteral) {
  TokenPtr token;
  TType type;

  if (dynamic_cast<BinaryExpr *>(&node)) {
    token = m_tempVars.back()->getToken();
    type  = m_tempVars.back()->getToken()->getType();
  } else if (dynamic_cast<FnCallExpr *>(&node)) {
    type  = node.getToken()->getType();
    token = std::make_shared<Token>(
      getIdentForLiteralType(type),
      "_t" + std::to_string(m_tempVars.size())
    );
    m_tempVars.emplace_back(std::make_unique<VarExpr>(token));
    if (type != TType::IDENT_VOID) {
      // we expect a non-void function to return a value that is kept in the most distant register because:
      //   1. we push all registers (rax, ..., r15)
      //   2. we generate IRs for function call
      //   3. we pop all registers (r15, ..., rax) <-- thus return value is stored in r15
      auto functionReturn = std::make_shared<Token>(
        TType::REGISTER,
        R15
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
      auto varToken = std::make_shared<Token>(
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

void IRGenerator::visit(Root &node) {
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
  auto [module1, module1Used] = Modules::getModule(CALCULATE_STRING_LENGTH);
  auto [module2, module2Used] = Modules::getModule(PRINT_NUMBER);
  auto [module3, module3Used] = Modules::getModule(PRINT_BOOLEAN);
  auto [module4, module4Used] = Modules::getModule(EXIT);

  if (module1Used) registerAllocator.allocate(std::move(module1), false);
  if (module2Used) registerAllocator.allocate(std::move(module2), false);
  if (module3Used) registerAllocator.allocate(std::move(module3), false);
  if (module4Used) registerAllocator.allocate(std::move(module4), false);

  // Instruction optimization
  const auto &instructions = getInstructions(Transformation::REGISTER_ALLOCATION);
  irOptimization.optimize(vec_slice(instructions, 0, instructions.size()));
}

void IRGenerator::visit(PrimitiveType &) {
}

void IRGenerator::visit(VarExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(BooleanExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(EqExpr &node) {
  const auto comparisonOp = getOperationForBinaryExpression(node.getToken()->getType(), false);
  m_comparisonOp.push(comparisonOp);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::CMP,
    nullptr,
    node.lhs()->getToken(),
    node.rhs()->getToken()
  ));
}

void IRGenerator::visit(CompExpr &node) {
  const auto comparisonOp = getOperationForBinaryExpression(node.getToken()->getType(), false);
  m_comparisonOp.push(comparisonOp);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::CMP,
    nullptr,
    node.lhs()->getToken(),
    node.rhs()->getToken()
  ));
}

void IRGenerator::visit(AddExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(SubExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(MultExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(DivExpr &node) {
  node.lhs()->accept(*this);
  node.rhs()->accept(*this);
  createBinaryExpression(node.getToken()->getType());
}

void IRGenerator::visit(UnaryExpr &node) {
  node.lhs()->accept(*this);
  throw NotImplementedError{fmt::format("Unary expressions are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(FnCallExpr &node) {
  node.getVariable()->accept(*this);
  constexpr auto registers = RegisterAllocator::getAllocatableRegisters;

  // suboptimal approach to avoid overriding registers inside the called function
  std::transform(registers.begin(), registers.end(), std::back_inserter(m_instructions), [&](auto reg) {
    return std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, reg)
    );
  });

  for (const auto &arg : node.getArguments()) {
    arg->accept(*this);
    const auto &[argToken, _] = getExpression(*arg);
    auto varToken = std::make_shared<Token>(
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
    std::make_shared<Token>(TType::IDENT_VOID, functionName->getValue<std::string>())
  ));

  // following the function call, restore old register values
  std::transform(std::ranges::rbegin(registers), std::ranges::rend(registers), std::back_inserter(m_instructions), [](auto reg) {
    return std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, reg)
    );
  });
}

void IRGenerator::visit(ClassInitExpr &node) {
  node.getVariable()->accept(*this);
  for (const auto &arg : node.getArguments()) {
    arg->accept(*this);
  }
  throw NotImplementedError{fmt::format("Class initialization expressions are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(IntExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(FloatExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(BoolExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(StringExpr &node) {
  m_stack.push(&node);
}

void IRGenerator::visit(StmtBlock &node) {
  for (const auto &stmt : node.getStatements()) {
    stmt->accept(*this);
  }
}

void IRGenerator::visit(ReturnStmt &node) {
  node.getReturnValue()->accept(*this);
  const auto &[token, _] = getExpression(*node.getReturnValue());
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::PUSH,
    nullptr,
    token
  ));
}

void IRGenerator::visit(BreakStmt &) {
  const auto breakLabel = m_breakLabel.top();
  m_breakLabel.pop();
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JMP,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, breakLabel)
  ));
}

void IRGenerator::visit(ContinueStmt &node) {
  throw NotImplementedError{fmt::format("Continue statements are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(VarDeclStmt &node) {
  node.getValue()->accept(*this);
  const auto &[token, _] = getExpression(*node.getValue());
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    node.getVariable()->getToken(), // int a
    token                           // _tx
  ));
}

void IRGenerator::visit(VarAssignStmt &node) {
  node.getVariable()->accept(*this);
  node.getValue()->accept(*this);
  const auto &[token, _] = getExpression(*node.getValue());
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::MOV,
    node.getVariable()->getToken(),
    token
  ));
}

void IRGenerator::visit(ExprStmt &node) {
  node.getExpression()->accept(*this);
}

void IRGenerator::visit(ReadStmt &node) {
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
void IRGenerator::visit(WriteStmt &node) {
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
            std::make_shared<Token>(TType::REGISTER, RDX)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::PUSH,
            nullptr,
            std::make_shared<Token>(TType::REGISTER, RSI)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::MOV,
            std::make_shared<Token>(TType::REGISTER, RSI),
            std::make_shared<Token>(type, token->getValue<std::string>())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::CALL,
            std::make_shared<Token>(TType::IDENT_VOID, Module2Str[CALCULATE_STRING_LENGTH])
          ));
          Modules::markAsUsed(CALCULATE_STRING_LENGTH);
          break;
        case TType::IDENT_INT:
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::PUSH,
            nullptr,
            std::make_shared<Token>(TType::REGISTER, RDI)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::MOV,
            std::make_shared<Token>(TType::REGISTER, RDI),
            std::make_shared<Token>(type, token->getValue<std::string>())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::CALL,
            std::make_shared<Token>(TType::IDENT_VOID, Module2Str[PRINT_NUMBER])
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::POP,
            nullptr,
            std::make_shared<Token>(TType::REGISTER, RDI)
          ));
          Modules::markAsUsed(PRINT_NUMBER);
          continue;
        case TType::IDENT_BOOL:
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::PUSH,
            nullptr,
            std::make_shared<Token>(TType::REGISTER, RDI)
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::MOV,
            std::make_shared<Token>(TType::REGISTER, RDI),
            std::make_shared<Token>(type, token->getValue<std::string>())
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::CALL,
            std::make_shared<Token>(TType::IDENT_VOID, Module2Str[PRINT_BOOLEAN])
          ));
          m_instructions.emplace_back(std::make_unique<Instruction>(
            Operation::POP,
            nullptr,
            std::make_shared<Token>(TType::REGISTER, RDI)
          ));
          Modules::markAsUsed(PRINT_BOOLEAN);
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
        std::make_shared<Token>(TType::REGISTER, RDX)
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::PUSH,
        nullptr,
        std::make_shared<Token>(TType::REGISTER, RSI)
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Token>(TType::REGISTER, RDX),
        std::make_shared<Token>(TType::LIT_INT, static_cast<int>(length))
      ));
      m_instructions.emplace_back(std::make_unique<Instruction>(
        Operation::MOV,
        std::make_shared<Token>(TType::REGISTER, RSI),
        std::make_shared<Token>(TType::LIT_STR, str)
      ));
    }

    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RAX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RCX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, R11)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::PUSH,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RDI)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::MOV,
      std::make_shared<Token>(TType::REGISTER, RAX),
      std::make_shared<Token>(TType::LIT_INT, 1)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::MOV,
      std::make_shared<Token>(TType::REGISTER, RDI),
      std::make_shared<Token>(TType::LIT_INT, 1)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::SYSCALL
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RDI)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, R11)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RCX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RAX)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RSI)
    ));
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::POP,
      nullptr,
      std::make_shared<Token>(TType::REGISTER, RDX)
    ));
  }
}

void IRGenerator::visit(Param &node) {
  node.getVariable()->accept(*this);
  throw NotImplementedError{fmt::format("Function parameters are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(FnDef &node) {
  node.getVariable()->accept(*this);
  const auto &functionName = popNode();
  const auto functionNameStr = functionName.getToken()->getValue<std::string>();
  TokenPtr returnAddressToken;

  if (functionNameStr != "main") {
    // the main function doesn't require a label indicating where it begins
    // because we have already designated address "0x4000b0" as the program's main entry point
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::LABEL,
      nullptr,
      std::make_shared<Token>(TType::IDENT_VOID, functionNameStr)
    ));
    // put the function return address into a variable because we'll be popping out the arguments
    // passed to the function in the later steps
    returnAddressToken = std::make_shared<Token>(
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
      std::make_shared<Token>(TType::IDENT_VOID, Module2Str[EXIT])
    ));
    Modules::markAsUsed(EXIT);
  } else {
    m_instructions.emplace_back(std::make_unique<Instruction>(
      Operation::RET
    ));
  }
}

void IRGenerator::visit(CtorDef &node) {
  throw NotImplementedError{fmt::format("Constructors are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(DtorDef &node) {
  throw NotImplementedError{fmt::format("Destructors are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(Field &node) {
  throw NotImplementedError{fmt::format("Class fields are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(ClassDef &node) {
  throw NotImplementedError{fmt::format("Classes are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(WhileLoop &node) {
  m_whileLabelCount++;

  const std::array labels {
    ".L" + std::to_string(m_whileLabelCount) + "_while_body",
    ".L" + std::to_string(m_whileLabelCount) + "_while_check",
    ".L" + std::to_string(m_whileLabelCount) + "_while_end",
  };

  m_breakLabel.emplace(labels[2]);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JMP,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[1])
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[0])
  ));

  node.getBody()->accept(*this);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[1])
  ));

  node.getCondition()->accept(*this);
  const auto jumpOp = createJumpOpFromCondition(*node.getCondition(), false);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    jumpOp,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[0])
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[2])
  ));
}

void IRGenerator::visit(ForLoop &node) {
  m_forLabelCount++;

  const std::array labels {
    ".L" + std::to_string(m_forLabelCount) + "_for_body",
    ".L" + std::to_string(m_forLabelCount) + "_for_check",
    ".L" + std::to_string(m_forLabelCount) + "_for_end",
  };

  m_breakLabel.emplace(labels[2]);
  node.getInitial()->accept(*this);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JMP,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[1])
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[0])
  ));

  node.getBody()->accept(*this);
  node.getIncrement()->accept(*this);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[1])
  ));

  node.getCondition()->accept(*this);
  const auto jumpOp = createJumpOpFromCondition(*node.getCondition(), false);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    jumpOp,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[0])
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[2])
  ));
}

void IRGenerator::visit(ForEachLoop &node) {
  node.getElement()->accept(*this);
  node.getCollection()->accept(*this);
  node.getBody()->accept(*this);
  throw NotImplementedError{fmt::format("For-each loops are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}

void IRGenerator::visit(IfStmt &node) {
  m_ifLabelCount++;

  const std::array labels {
    ".L" + std::to_string(m_ifLabelCount) + "_if_false",
    ".L" + std::to_string(m_ifLabelCount) + "_if_end",
  };

  node.getCondition()->accept(*this);
  const auto jumpOp = createJumpOpFromCondition(*node.getCondition(), true);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    jumpOp,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[0])
  ));

  node.getBody()->accept(*this);

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::JMP,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[1])
  ));
  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[0])
  ));

  for (const auto &stmt : node.getElseStatements()) {
    stmt->accept(*this);
  }

  m_instructions.emplace_back(std::make_unique<Instruction>(
    Operation::LABEL,
    nullptr,
    std::make_shared<Token>(TType::IDENT_VOID, labels[1])
  ));
}

void IRGenerator::visit(ElseStmt &node) {
  node.getBody()->accept(*this);
}

void IRGenerator::visit(ElseIfStmt &node) {
  node.getCondition()->accept(*this);
  node.getBody()->accept(*this);
  throw NotImplementedError{fmt::format("Else-if statements are not supported in {}:{}",
                                        node.getToken()->getPosition().getFileName(),
                                        node.getToken()->getPosition().getLineNo())};
}
