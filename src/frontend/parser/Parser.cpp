#include <algorithm>
#include <array>
#include <unordered_map>
// Wisnia
#include "Parser.hpp"
#include "AST.hpp"
#include "Lexer.hpp"

using namespace Wisnia;
using namespace AST;
using namespace Basic;
using namespace Utils;

Parser::Parser(const Lexer &lexer)
    : m_tokens{lexer.getTokens()} {}

bool Parser::has(const TType &token) const {
  return peek()->getType() == token;
}

bool Parser::has2(const TType &token) const {
  assert(m_pos + 2 < m_tokens.size());
  return m_tokens[m_pos + 2]->getType() == token;
}

void Parser::expect(const TType &token) {
  if (peek()->getType() == token) {
    consume();
    return;
  }
  throw ParserError{
    "Expected " + TokenType2Str[token] +
    " but found " + TokenType2Str[peek()->getType()]
  };
}

std::unique_ptr<Root> Parser::parse() {
  auto root = std::make_unique<Root>();
  // continue parsing as long as there are tokens
  while (!has(TType::TOK_EOF)) {
    switch (peek()->getType()) {
      // <FN_DECL>
      case TType::KW_FN:
        root->addGlobalFnDef(parseFnDef());
        break;
        // <CLASS_DECL>
      case TType::KW_CLASS:
        root->addGlobalClassDef(parseClassDef());
        break;
      default:
        throw ParserError{"Not a global definition of either a class, or a function"};
    }
  }
  return root;
}

std::unique_ptr<BaseExpr> Parser::parseVar() {
  if (has(TType::IDENT)) {
    return std::make_unique<VarExpr>(getNextToken());
  }
  throw ParserError{"Not a variable name"};
}

// <FN_DECL> ::= "fn" <IDENT> <PARAMS> "->" <TYPE> <STMT_BLOCK>
std::unique_ptr<BaseDef> Parser::parseFnDef() {
  expect(TType::KW_FN);                 // expect "fn"
  auto var = parseVar();
  auto fnDef = std::make_unique<FnDef>(var->getToken());
  fnDef->addVar(std::move(var));
  fnDef->addParams(parseParamsList());  // parse <PARAMS>
  expect(TType::OP_FN_ARROW);           // expect "->"
  fnDef->addType(parsePrimitiveType()); // <TYPE>
  fnDef->addBody(parseStmtBlock());     // <STMT_BLOCK>
  return fnDef;
}

// <PARAM> ::= <IDENT> ":" <TYPE>
std::unique_ptr<Param> Parser::parseParam() {
  // parse <IDENT>
  auto var = parseVar();
  auto param = std::make_unique<Param>(var->getToken());
  // expect ":"
  expect(TType::OP_COL);
  // parse <TYPE>
  param->addVar(std::move(var));
  param->addType(parsePrimitiveType());
  return param;
}

// <PARAMS> ::= "(" ")" | "(" <PARAMS_SEQ> ")"
std::vector<std::unique_ptr<Param>> Parser::parseParamsList() {
  expect(TType::OP_PAREN_O);  // expect "("
  std::vector<std::unique_ptr<Param>> paramsList;
  // <PARAMS_SEQ> ::= <PARAM> | <PARAMS_SEQ> "," <PARAM>
  while (hasNext() && !has(TType::OP_PAREN_C)) {
    paramsList.push_back(parseParam());
    // Check whether we've parsed all params already
    if (has(TType::OP_PAREN_C)) break;
    // If not, continue parsing
    expect(TType::OP_COMMA);
  }
  expect(TType::OP_PAREN_C);  // expect ")"
  return paramsList;
}

// <TYPE> ::= "void" | "int" | "bool" | "float" | "string"
std::unique_ptr<BaseType> Parser::parsePrimitiveType() {
  constexpr std::array<TType, 5> kPrimitiveTypes {
    TType::KW_VOID, TType::KW_INT, TType::KW_BOOL, TType::KW_FLOAT, TType::KW_STRING
  };
  if (std::any_of(kPrimitiveTypes.begin(), kPrimitiveTypes.end(), [&](TType t) { return peek()->getType() == t; })) {
    return std::make_unique<PrimitiveType>(getNextToken());
  }
  throw ParserError{"Function definition doesn't have any of the supported types"};
}

// <STMT_BLOCK> ::= "{" "}" | "{" <STMTS> "}"
std::unique_ptr<BaseStmt> Parser::parseStmtBlock() {
  if (has(TType::OP_BRACE_O)) {
    consume();  // eat "{"
    auto stmtBlock = std::make_unique<StmtBlock>();
    while (hasNext() && !has(TType::OP_BRACE_C)) stmtBlock->addStmt(parseStmt());
    expect(TType::OP_BRACE_C);  // expect "}"
    return stmtBlock;
  }
  return parseStmt();
}

// <STMT> ::= <FN_RETURN_STMT> <STMT_END>
//          | <LOOP_BREAK_STMT> <STMT_END>
//          | <VAR_DECL> <STMT_END>
//          | <ASSIGNMENT_STMT> <STMT_END>
//          | <EXPRESSION> <STMT_END>
//          | <IO_STMT> <STMT_END>
//          | <LOOP_STMT>
//          | <IF_STMT>
std::unique_ptr<BaseStmt> Parser::parseStmt() {
  switch (peek()->getType()) {
    // <FN_RETURN_STMT>
    case TType::KW_RETURN:
      return parseReturnStmt();
    // <LOOP_BREAK_STMT>
    case TType::KW_BREAK:
    case TType::KW_CONTINUE:
      return parseLoopBrkStmt();
    // <VAR_DECL>
    case TType::KW_VOID:
    case TType::KW_INT:
    case TType::KW_BOOL:
    case TType::KW_FLOAT:
    case TType::KW_STRING:
      if (has2(TType::IDENT))
        return parseVarDeclStmt();
    // <ASSIGNMENT_STMT>
    case TType::IDENT: {         // "a"
      if (has2(TType::OP_ASSN))  // "a="
        return parseVarAssignStmt();
      else  // "a(", "a{", "a.", "a->"
        return parseExprStmt();
    }
    // <IO_STMT>
    case TType::KW_READ:
    case TType::KW_PRINT:
      return parseIOStmt();
    // <LOOP_STMT>
    case TType::KW_WHILE:
    case TType::KW_FOR:
    case TType::KW_FOREACH:
      return parseLoops();
    // <IF_STMT>
    case TType::KW_IF:
      return parseIfBlock();
    // <EXPRESSION>
    default:
      return parseExprStmt();
  }
}

// <FN_RETURN_STMT> <STMT_END>
// <FN_RETURN_STMT> ::= <RETURN_SYMB> | <RETURN_SYMB> <EXPRESSION>
std::unique_ptr<BaseStmt> Parser::parseReturnStmt() {
  expect(TType::KW_RETURN);        // expect "return"
  if (has(TType::OP_SEMICOLON)) {  // if the following token is ";"
    consume();                     // eat ";"
    return std::make_unique<ReturnStmt>();
  } else {
    auto returnStmt = std::make_unique<ReturnStmt>();
    returnStmt->addReturnValue(parseExpr());
    expect(TType::OP_SEMICOLON);   // expect ";"
    return returnStmt;
  }
}

// <EXPRESSION> ::= <AND_EXPR> | <EXPRESSION> <OR_SYMB> <AND_EXPR>
// <EXPRESSION> ::= <AND_EXPR> { <OR_SYMB> <AND_EXPR> }
std::unique_ptr<BaseExpr> Parser::parseExpr() {
  auto lhs = parseAndExpr();
  while (has(TType::OP_OR)) {  // ... <OR_SYMB> <AND_EXPR>
    const auto &token = peek();
    consume();                 // eat "||"
    auto rhs = parseAndExpr();
    // Make a temporary copy of the lhs
    auto tempLhs = std::unique_ptr<BaseExpr>(std::move(lhs));
    // Move rhs and tempLhs nodes
    lhs = std::make_unique<BooleanExpr>(token);
    lhs->addChild(std::move(tempLhs));
    lhs->addChild(std::move(rhs));
  }
  return lhs;
}

// <AND_EXPR> ::= <EQUAL_EXPR> | <AND_EXPR> <AND_SYMB> <EQUAL_EXPR>
// <AND_EXPR> ::= <EQUAL_EXPR> { <AND_SYMB> <EQUAL_EXPR> }
std::unique_ptr<BaseExpr> Parser::parseAndExpr() {
  auto lhs = parseEqExpr();
  while (has(TType::OP_AND)) {  // ... <AND_SYMB> <EQUAL_EXPR>
    const auto &token = peek();
    consume();                  // eat "&&"
    auto rhs = parseEqExpr();
    // Make a temporary copy of the lhs
    auto tempLhs = std::unique_ptr<BaseExpr>(std::move(lhs));
    // Move rhs and tempLhs nodes
    lhs = std::make_unique<BooleanExpr>(token);
    lhs->addChild(std::move(tempLhs));
    lhs->addChild(std::move(rhs));
  }
  return lhs;
}

// <EQUAL_EXPR> ::= <COMPARE_EXPR> | <EQUAL_EXPR> <EQUALITY_SYMB> <COMPARE_EXPR>
// <EQUAL_EXPR> ::= <COMPARE_EXPR> { <EQUALITY_SYMB> <COMPARE_EXPR> }
std::unique_ptr<BaseExpr> Parser::parseEqExpr() {
  auto lhs = parseCompExpr();
  // ... <EQUALITY_SYMB> <COMPARE_EXPR>
  while (hasAnyOf(TType::OP_EQ, TType::OP_NE)) {
    const auto &token = peek();
    consume(); // eat either "==" or "!="
    auto rhs = parseCompExpr();
    // Make a temporary copy of the lhs
    auto tempLhs = std::unique_ptr<BaseExpr>(std::move(lhs));
    // Move rhs and tempLhs nodes
    lhs = std::make_unique<EqExpr>(token);
    lhs->addChild(std::move(tempLhs));
    lhs->addChild(std::move(rhs));
  }
  return lhs;
}

// <COMPARE_EXPR> ::= <ADD_EXPR> | <COMPARE_EXPR> <COMPARISON_SYMB> <ADD_EXPR>
// <COMPARE_EXPR> ::= <ADD_EXPR> { <COMPARISON_SYMB> <ADD_EXPR> }
std::unique_ptr<BaseExpr> Parser::parseCompExpr() {
  auto lhs = parseAddExpr();
  // ... <COMPARISON_SYMB> <ADD_EXPR>
  while (hasAnyOf(TType::OP_G, TType::OP_GE, TType::OP_L, TType::OP_LE)) {
    const auto &token = peek();
    consume(); // eat any of ">", ">=", "<", "<="
    auto rhs = parseAddExpr();
    // Make a temporary copy of the lhs
    auto tempLhs = std::unique_ptr<BaseExpr>(std::move(lhs));
    // Move rhs and tempLhs nodes
    lhs = std::make_unique<CompExpr>(token);
    lhs->addChild(std::move(tempLhs));
    lhs->addChild(std::move(rhs));
  }
  return lhs;
}

// <ADD_EXPR> ::= <MULT_EXPR> | <ADD_EXPR> <ADD_OP> <MULT_EXPR>
// <ADD_EXPR> ::= <MULT_EXPR> { <ADD_OP> <MULT_EXPR> }
std::unique_ptr<BaseExpr> Parser::parseAddExpr() {
  auto lhs = parseMultExpr();
  // ... <ADD_OP> <MULT_EXPR>
  while (hasAnyOf(TType::OP_ADD, TType::OP_SUB)) {
    const auto &token = peek();
    consume(); // eat either "+" or "-"
    auto rhs = parseMultExpr();
    // Make a temporary copy of the lhs
    auto tempLhs = std::unique_ptr<BaseExpr>(std::move(lhs));
    // Move rhs and tempLhs nodes
    if (token->getType() == TType::OP_ADD) {
      lhs = std::make_unique<AddExpr>(token);
    } else {
      lhs = std::make_unique<SubExpr>(token);
    }
    lhs->addChild(std::move(tempLhs));
    lhs->addChild(std::move(rhs));
  }
  return lhs;
}

// <MULT_EXPR> ::= <UNARY_EXPR> | <MULT_EXPR> <MULT_OP> <UNARY_EXPR>
// <MULT_EXPR> ::= <UNARY_EXPR> { <MULT_OP> <UNARY_EXPR> }
std::unique_ptr<BaseExpr> Parser::parseMultExpr() {
  auto lhs = parseUnaryExpr();
  // ... <MULT_OP> <UNARY_EXPR>
  while (hasAnyOf(TType::OP_MUL, TType::OP_DIV)) {
    const auto &token = peek();
    consume(); // eat either "*" or "/"
    auto rhs = parseUnaryExpr();
    // Make a temporary copy of the lhs
    auto tempLhs = std::unique_ptr<BaseExpr>(std::move(lhs));
    // Move rhs and tempLhs nodes
    if (token->getType() == TType::OP_MUL) {
      lhs = std::make_unique<MultExpr>(token);
    } else {
      lhs = std::make_unique<DivExpr>(token);
    }
    lhs->addChild(std::move(tempLhs));
    lhs->addChild(std::move(rhs));
  }
  return lhs;
}

// <UNARY_EXPR> ::= <SOME_EXPR> | <UNARY_SYM> <UNARY_EXPR>
// <UNARY_EXPR> ::= {UNARY_SYM} <SOME_EXPR>
std::unique_ptr<BaseExpr> Parser::parseUnaryExpr() {
  // Checks for acceptable token types
  auto isAnyOf{[&]() -> bool { return hasAnyOf(TType::OP_UNEG, TType::OP_UADD); }};
  // {!|++} <SOME_EXPR>
  if (isAnyOf()) {
    auto lhs = std::unique_ptr<BaseExpr>();
    while (isAnyOf()) {  // <UNARY_SYM> ...
      const auto &token = peek();
      consume(); // eat either "!" or "++"
      auto rhs = parseUnaryExpr();
      // Append the unary expression we've just found
      lhs = std::make_unique<UnaryExpr>(token);
      lhs->addChild(std::move(rhs));
    }
    return lhs;
  }
  // <SOME_EXPR>
  return parseSomeExpr();
}

// <SOME_EXPR> ::= "(" <EXPRESSION> ")" | <VAR> | <FN_CALL> | <CLASS_M_CALL> |
// <CONSTANT_LIT> | <CLASS_INIT>
std::unique_ptr<BaseExpr> Parser::parseSomeExpr() {
  switch (peek()->getType()) {
    // "(" <EXPRESSION> ")"
    case TType::OP_PAREN_O: {
      expect(TType::OP_PAREN_O);  // eat "("
      auto exp = parseExpr();
      expect(TType::OP_PAREN_C);  // eat ")"
      return exp;
    }
    // <VAR> | <FN_CALL> | <CLASS_M_CALL>
    case TType::IDENT:
      return parseVarExp();
    // <CLASS_INIT>
    case TType::KW_CLASS_INIT:
      return parseClassInit();
    // <CONSTANT_LIT>
    default:
      return parseConstExpr();
  }
}

// Returns any of the following: <VAR> | <FN_CALL> | <CLASS_M_CALL>
std::unique_ptr<BaseExpr> Parser::parseVarExp() {
  if (has(TType::IDENT)) {  // a
    // <FN_CALL>
    if (has2(TType::OP_PAREN_O) || has2(TType::OP_BRACE_O))  // a( or a{
      return parseFnCall();
    // <CLASS_M_CALL>
    else if (has2(TType::OP_METHOD_CALL) || has2(TType::OP_FN_ARROW))  // a. or a->
      return parseMethodCall();
    // <VAR>
    else
      return parseVar();
  }
  throw ParserError{"Undefined expression"};
}

// <FN_CALL> ::= <IDENT> <ARGUMENTS>
std::unique_ptr<BaseExpr> Parser::parseFnCall() {
  auto var = parseVar();
  auto fnCallPtr = std::make_unique<FnCallExpr>(var->getToken());
  fnCallPtr->addVar(std::move(var));
  fnCallPtr->addArgs(parseArgsList());
  return fnCallPtr;
}

// <CLASS_M_CALL> ::= <IDENT> <CLASS_M_CALL_SYM> <VAR> <ARGUMENTS
// {<CLASS_M_CALL_SYM> <VAR> <ARGUMENTS}
std::unique_ptr<BaseExpr> Parser::parseMethodCall() {
  auto className = getNextToken();

  // eat "." or "->"
  hasAnyOf(TType::OP_METHOD_CALL, TType::OP_FN_ARROW)
      ? consume()
      : throw ParserError{"Unknown method call symbol"};

  // Creates a `VarExpr` of concatenated class and var names, i.e. class::var
  auto methodVarExpr = [this, &className]() {
    auto tempVar = parseVar();
    auto methodTok = std::make_shared<Token>(
        tempVar->getToken()->getType(),
        className->getValue<std::string>() + "::" + tempVar->getToken()->getValue<std::string>(),
        tempVar->getToken()->getPosition());
    return std::make_unique<VarExpr>(std::move(methodTok));
  };

  auto lhsVar = methodVarExpr();
  auto methodCallPtr = std::make_unique<FnCallExpr>(lhsVar->getToken());
  methodCallPtr->addVar(std::move(lhsVar));
  methodCallPtr->addClassName(className);
  methodCallPtr->addArgs(parseArgsList());

  while (hasAnyOf(TType::OP_METHOD_CALL, TType::OP_FN_ARROW)) {
    consume();  // eat "." or "->"
    auto rhsVar = methodVarExpr();
    auto rhs = std::make_unique<FnCallExpr>(rhsVar->getToken());
    rhs->addVar(std::move(rhsVar));
    rhs->addClassName(methodCallPtr->getFnName());
    rhs->addArgs(parseArgsList());
    // Make a temporary copy of the lhs
    auto tempLhs = std::unique_ptr<BaseExpr>(std::move(methodCallPtr));
    // Move rhs and tempLhs nodes
    methodCallPtr = std::make_unique<FnCallExpr>();
    methodCallPtr->addChild(std::move(tempLhs));
    methodCallPtr->addChild(std::move(rhs));
  }

  // TODO: create a base class that holds references to this object
  // Reason: when this object with multiple classes (.. -> .. -> getFib{5}) gets
  // created, the base class loses token_ and args_ fields when tempLhs is
  // created (or use a copy ctor?)
  return methodCallPtr;
}

// <ARGUMENTS> ::= "{" "}" | "{" <EXPR_LIST> "}" | "(" ")" | "(" <EXPR_LIST> ")"
std::vector<std::unique_ptr<BaseExpr>> Parser::parseArgsList() {
  // To map possible argsBody types to their selectable closing body types
  std::unordered_map<TType, TType> expectBodyType {
    {TType::OP_PAREN_O, TType::OP_PAREN_C},
    {TType::OP_BRACE_O, TType::OP_BRACE_C}
  };

  consume();  // idk why's this required,
              // but it solved the issue with incorrect token types

  auto argsCurrType = curr()->getType();               // either "(" or "{"
  auto argsExpType = expectBodyType.at(argsCurrType);  // the opposite of argsCurrType
  std::vector<std::unique_ptr<BaseExpr>> argsList;     // function arguments

  // <EXPR_LIST> ::= <EXPRESSION> | <EXPR_LIST> "," <EXPRESSION>
  while (hasNext() && !has(argsExpType)) {
    auto arg = parseExpr();
    argsList.push_back(std::move(arg));
    // Check whether we've parsed all args
    if (has(argsExpType)) break;
    // If not, continue parsing
    expect(TType::OP_COMMA);
  }
  expect(argsExpType);  // expect either ")" or "}"
  return argsList;
}

// <CLASS_INIT> ::= "new" <IDENT> <ARGUMENTS>
std::unique_ptr<BaseExpr> Parser::parseClassInit() {
  expect(TType::KW_CLASS_INIT);  // expect "new"
  auto var = parseVar();
  auto classInitPtr = std::make_unique<ClassInitExpr>(var->getToken());
  classInitPtr->addVar(std::move(var));
  classInitPtr->addArgs(parseArgsList());
  return classInitPtr;
}

// <CONSTANT_LIT> ::= <BOOL_CONSTANT> | <NUMERIC_CONSTANT> | <STRING>
std::unique_ptr<BaseExpr> Parser::parseConstExpr() {
  switch (peek()->getType()) {
    // <NUMERIC_CONSTANT> : integer
    case TType::LIT_INT:
      return std::make_unique<IntExpr>(getNextToken());
    // <NUMERIC_CONSTANT> : float
    case TType::LIT_FLT:
      return std::make_unique<FloatExpr>(getNextToken());
    // <BOOL_CONSTANT>
    case TType::KW_TRUE:
    case TType::KW_FALSE:
      return std::make_unique<BoolExpr>(getNextToken());
    // <STRING>
    case TType::LIT_STR:
      return std::make_unique<StringExpr>(getNextToken());
    default:
      throw ParserError{"Unknown constant expression"};
  }
}

// <LOOP_BREAK_STMT> <STMT_END>
// <LOOP_BREAK_STMT> ::= <BREAK_SYMB> | <CONTINUE_SYMB>
std::unique_ptr<BaseStmt> Parser::parseLoopBrkStmt() {
  // expect either "break" or "continue"
  if (hasAnyOf(TType::KW_BREAK, TType::KW_CONTINUE)) {
    auto tokenName = getNextToken(); // eat either "break" or "continue"
    // if the following token is ";"
    if (has(TType::OP_SEMICOLON)) {
      consume();  // eat ";"
      if (tokenName->getType() == TType::KW_BREAK) {
        return std::make_unique<BreakStmt>(tokenName);
      }
      return std::make_unique<ContinueStmt>(tokenName);
    }
  }
  throw ParserError{"Unterminated loop break statement"};
}

// <VAR_DECL> <STMT_END>
// <VAR_DECL> ::= <TYPE> <VAR> | <TYPE> <VAR> "=" <EXPRESSION> | <TYPE> <VAR>
// "{" <EXPRESSION> "}"
std::unique_ptr<BaseStmt> Parser::parseVarDeclStmt() {
  auto varDeclPtr = std::make_unique<VarDeclStmt>();
  auto varType = parsePrimitiveType();
  varDeclPtr->addVar(parseVar());
  std::unique_ptr<BaseExpr> varValue;

  // <TYPE> <VAR> "=" <EXPRESSION>
  if (has(TType::OP_ASSN)) {
    consume();  // eat "="
    varValue = parseExpr();
  }
  // <TYPE> <VAR> "{" <EXPRESSION> "}"
  else if (has(TType::OP_BRACE_O)) {
    consume();  // eat "{"
    varValue = parseExpr();
    consume();  // eat "}"
  }
  // <TYPE> <VAR>
  else {
    // add default values
    switch (varType->getType()) {
      case TType::KW_INT: {
        auto intToken = std::make_shared<Token>(TType::LIT_INT, 0, varDeclPtr->getVar()->getToken()->getPosition());
        varValue = std::make_unique<IntExpr>(std::move(intToken));
        break;
      }
      case TType::KW_FLOAT: {
        auto floatToken = std::make_shared<Token>(TType::LIT_FLT, 0.0f, varDeclPtr->getVar()->getToken()->getPosition());
        varValue = std::make_unique<FloatExpr>(std::move(floatToken));
        break;
      }
      case TType::KW_STRING: {
        auto stringToken = std::make_shared<Token>(TType::LIT_STR, "", varDeclPtr->getVar()->getToken()->getPosition());
        varValue = std::make_unique<StringExpr>(std::move(stringToken));
        break;
      }
      case TType::KW_BOOL: {
        auto boolToken = std::make_shared<Token>(TType::LIT_BOOL, false, varDeclPtr->getVar()->getToken()->getPosition());
        varValue = std::make_unique<BoolExpr>(std::move(boolToken));
        break;
      }
      default:
        throw ParserError{"Failed to assign default value for " + varDeclPtr->getVar()->getToken()->getASTValueStr()};
    }
  }
  varDeclPtr->addType(std::move(varType));
  varDeclPtr->addValue(std::move(varValue));
  expect(TType::OP_SEMICOLON);
  return varDeclPtr;
}

// <ASSIGNMENT_STMT> <STMT_END>
// <ASSIGNMENT_STMT> ::= <VAR> {"=" | "+=" | "-=" | "*=" | "/="} <EXPRESSION>
std::unique_ptr<BaseStmt> Parser::parseVarAssignStmt(bool expect_semicolon) {
  auto varAssignPtr = std::make_unique<VarAssignStmt>();
  varAssignPtr->addVar(parseVar());
  expect(TType::OP_ASSN);  // eat "=" // TODO: add these operators: +=, -=, *=, /=
  varAssignPtr->addValue(parseExpr());
  if (expect_semicolon) expect(TType::OP_SEMICOLON);
  return varAssignPtr;
}

// <EXPRESSION> <STMT_END>
std::unique_ptr<BaseStmt> Parser::parseExprStmt() {
  auto exprStmtPtr = std::make_unique<ExprStmt>();
  exprStmtPtr->addExpr(parseExpr());
  expect(TType::OP_SEMICOLON);
  return exprStmtPtr;
}

// <IO_STMT> ::= <INPUT_STMT> | <OUTPUT_STMT>
std::unique_ptr<BaseStmt> Parser::parseIOStmt() {
  if (has(TType::KW_READ))
    return parseReadIOStmt();
  return parseWriteIOStmt();
}

// <INPUT_STMT> ::= "read" <INPUT_SEQ>
// <INPUT_SEQ>  ::= <VAR> | <INPUT_SEQ> "," <VAR>
std::unique_ptr<BaseStmt> Parser::parseReadIOStmt() {
  expect(TType::KW_READ);  // expect "read"
  auto readIO = std::make_unique<ReadStmt>();

  // <INPUT_SEQ>
  while (hasNext()) {  // a, b, c
    readIO->addVar(parseVar());
    if (has(TType::OP_COMMA))
      consume();       // eat ","
    else
      break;
  }

  expect(TType::OP_SEMICOLON);
  return readIO;
}

// <OUTPUT_STMT> ::= "print" <OUTPUT_SEQ>
// <OUTPUT_SEQ>  ::= <EXPRESSION> | <OUTPUT_SEQ> "," <EXPRESSION>
std::unique_ptr<BaseStmt> Parser::parseWriteIOStmt() {
  expect(TType::KW_PRINT);  // expect "print"
  auto writeIO = std::make_unique<WriteStmt>();

  // <OUTPUT_SEQ>
  while (hasNext()) {  // a, b, c
    writeIO->addExpr(parseExpr());
    if (has(TType::OP_COMMA))
      consume();       // eat ","
    else
      break;
  }

  expect(TType::OP_SEMICOLON);
  return writeIO;
}

// <LOOP_STMT> ::= <WHILE_LOOP> | <FOR_LOOP> | <FOREACH_LOOP>
std::unique_ptr<BaseLoop> Parser::parseLoops() {
  // <WHILE_LOOP>
  if (has(TType::KW_WHILE))
    return parseWhileLoop();
  // <FOR_LOOP>
  else if (has(TType::KW_FOR))
    return parseForLoop();
  // <FOREACH_LOOP>
  else
    return parseForEachLoop();
}

// <WHILE_LOOP> ::= "while" "(" <EXPRESSION> ")" <STMT_BLOCK>
std::unique_ptr<BaseLoop> Parser::parseWhileLoop() {
  auto whileLoopPtr = std::make_unique<WhileLoop>(getNextToken());
  expect(TType::OP_PAREN_O);                // expect "("
  whileLoopPtr->addCond(parseExpr());       // <EXPRESSION>
  expect(TType::OP_PAREN_C);                // expect ")"
  whileLoopPtr->addBody(parseStmtBlock());  // { ... }
  return whileLoopPtr;
}

// <FOR_LOOP> ::= "for" "(" <FOR_CONDITION> ")" <STMT_BLOCK>
// <FOR_CONDITION> ::= <VAR_DECL> ";" <EXPRESSION> ";" <EXPRESSION>
std::unique_ptr<BaseLoop> Parser::parseForLoop() {
  auto forLoopPtr = std::make_unique<ForLoop>(getNextToken());
  expect(TType::OP_PAREN_O);              // expect "("
  forLoopPtr->addInitial(parseVarDeclStmt());
  forLoopPtr->addCondition(parseExpr());
  expect(TType::OP_SEMICOLON);
  forLoopPtr->addIncrement(parseVarAssignStmt(false));
  expect(TType::OP_PAREN_C);              // expect ")"
  forLoopPtr->addBody(parseStmtBlock());  // { ... }
  return forLoopPtr;
}

// <FOREACH_LOOP> ::= "for_each" "(" <FOREACH_CONDITION> ")" <STMT_BLOCK>
// <FOREACH_CONDITION> ::= <VAR> "in" <EXPRESSION>
std::unique_ptr<BaseLoop> Parser::parseForEachLoop() {
  auto foreachLoopPtr = std::make_unique<ForEachLoop>(getNextToken());
  expect(TType::OP_PAREN_O);                  // expect "("
  foreachLoopPtr->addElement(parseVar());
  expect(TType::KW_FOREACH_IN);
  foreachLoopPtr->addCollection(parseExpr());
  expect(TType::OP_PAREN_C);                  // expect ")"
  foreachLoopPtr->addBody(parseStmtBlock());  // { ... }
  return foreachLoopPtr;
}

// <IF_STMT> ::= <IF_BLOCK> [<MULTIPLE_ELSE_IF_BLOCK>]
std::unique_ptr<BaseIf> Parser::parseIfBlock() {
  auto ifStmtPtr = std::make_unique<IfStmt>(getNextToken());

  // <IF_ELSE_COND_BODY> ::= "(" <EXPRESSION> ")"
  auto ifCond = [&]() -> std::unique_ptr<BaseExpr> {
    expect(TType::OP_PAREN_O);  // expect "("
    auto cond = parseExpr();    // parse if header condition
    expect(TType::OP_PAREN_C);  // expect ")"
    return cond;
  };

  ifStmtPtr->addCondition(ifCond());
  ifStmtPtr->addBody(parseStmtBlock());

  // a vector of else statement blocks
  std::vector<std::unique_ptr<BaseIf>> elseBlocks;

  // Parse else blocks
  // [<MULTIPLE_ELSE_IF_BLOCK>]
  if (hasAnyOf(TType::KW_ELIF, TType::KW_ELSE)) {
    elseBlocks = parseMultipleElseBlock();
    ifStmtPtr->addElseBlocks(std::move(elseBlocks));
  }

  return ifStmtPtr;
}

// <MULTIPLE_ELSE_IF_BLOCK> ::= <MULTIPLE_ELIF> | <ELSE_BLOCK>
// <MULTIPLE_ELIF> ::= <ELIF_BLOCK> {<ELIF_BLOCK>}
std::vector<std::unique_ptr<BaseIf>> Parser::parseMultipleElseBlock() {
  std::vector<std::unique_ptr<BaseIf>> elseBlocks;

  // <ELIF_BLOCK> ::= "elif" "(" <EXPRESSION> ")" <STMT_BLOCK>
  auto elifBodyPtr = [&]() -> std::unique_ptr<ElseIfStmt> {
    auto tokenName = getNextToken();

    // parse elif condition
    expect(TType::OP_PAREN_O);  // expect "("
    auto cond = parseExpr();    // parse if header condition
    expect(TType::OP_PAREN_C);  // expect ")"

    // construct elif statement pointer
    auto elifStmtPtr = std::make_unique<ElseIfStmt>(tokenName);
    elifStmtPtr->addCondition(std::move(cond));
    elifStmtPtr->addBody(parseStmtBlock());
    return elifStmtPtr;
  };

  // <ELSE_BLOCK> ::= "else" <STMT_BLOCK>
  auto elseBodyPtr = [&]() -> std::unique_ptr<ElseStmt> {
    auto elseStmtPtr = std::make_unique<ElseStmt>(getNextToken());
    elseStmtPtr->addBody(parseStmtBlock());
    return elseStmtPtr;
  };

  // Parse elif blocks while there are any left
  while (has(TType::KW_ELIF))
    elseBlocks.push_back(elifBodyPtr());

  // Eventually, parse the else block
  if (has(TType::KW_ELSE))
    elseBlocks.push_back(elseBodyPtr());

  return elseBlocks;
}

// <CLASS_DECL> ::= "class" <IDENT> <CLASS_BODY>
std::unique_ptr<BaseDef> Parser::parseClassDef() {
  expect(TType::KW_CLASS);  // expect "class"

  // parse <IDENT>
  auto var = parseVar();
  auto classDef = std::make_unique<ClassDef>(var->getToken());

  // Creates a token in-place for use in `PrimitiveType`
  auto classTypeTok = std::make_shared<Token>(
      TType::KW_CLASS,
      var->getToken()->getValue<std::string>(),
      var->getToken()->getPosition()
  );

  classDef->addVar(std::move(var));
  classDef->addType(std::make_unique<PrimitiveType>(std::move(classTypeTok)));

  // Parse <CLASS_BODY>
  // <CLASS_BODY> ::= "{" "}" | "{" <CLASS_STMTS> "}"
  if (has(TType::OP_BRACE_O)) {
    consume();  // eat "{"

    // <CLASS_STMT> ::= <METHOD_DECLS> | <FIELD_DECLS>
    while (hasNext() && !has(TType::OP_BRACE_C)) {
      switch (peek()->getType()) {
        // <CONSTRUCTOR_DECL>
        case TType::KW_CLASS_DEF:
          classDef->addCtor(parseClassCtorDef());
          break;
          // <DESTRUCTOR_DECL>
        case TType::KW_CLASS_REM:
          classDef->addDtor(parseClassDtorDef());
          break;
          // <FN_DECL>
        case TType::KW_FN:
          classDef->addMethod(parseFnDef());
          break;
          // <FIELD_DECLS>
        default:
          classDef->addField(parseClassField());
          break;
      }
    }
    expect(TType::OP_BRACE_C);  // expect "}"
  }

  return classDef;
}

// <CONSTRUCTOR_DECL> ::= <CONSTRUCTOR_DEF> <IDENT> <PARAMS> <STMT_BLOCK>
std::unique_ptr<BaseDef> Parser::parseClassCtorDef() {
  expect(TType::KW_CLASS_DEF);            // expect "def"
  auto var = parseVar();
  auto ctorDef = std::make_unique<CtorDef>(var->getToken());
  ctorDef->addVar(std::move(var));
  ctorDef->addParams(parseParamsList());  // parse <PARAMS>
  ctorDef->addBody(parseStmtBlock());     // <STMT_BLOCK>
  return ctorDef;
}

// <DESTRUCTOR_DECL> ::= <DESTRUCTOR_DEF>  <IDENT> <PARAMS> <STMT_BLOCK>
std::unique_ptr<BaseDef> Parser::parseClassDtorDef() {
  expect(TType::KW_CLASS_REM);            // expect "rem"
  auto var = parseVar();
  auto dtorDef = std::make_unique<DtorDef>(var->getToken());
  dtorDef->addVar(std::move(var));
  dtorDef->addParams(parseParamsList());  // parse <PARAMS>
  dtorDef->addBody(parseStmtBlock());     // <STMT_BLOCK>
  return dtorDef;
}

// Same as <VAR_DECL>
std::unique_ptr<Field> Parser::parseClassField() {
  auto varDeclPtr = std::make_unique<Field>();
  auto varType = parsePrimitiveType();
  varDeclPtr->addVar(parseVar());
  varDeclPtr->addType(std::move(varType));
  std::unique_ptr<BaseExpr> varValue;

  // <TYPE> <VAR> "=" <EXPRESSION>
  if (has(TType::OP_ASSN)) {
    consume();  // eat "="
    varValue = parseExpr();
  }
  // <TYPE> <VAR> "{" <EXPRESSION> "}"
  else if (has(TType::OP_BRACE_O)) {
    consume();  // eat "{"
    varValue = parseExpr();
    consume();  // eat "}"
  }
  // <TYPE> <VAR>
  else {
    expect(TType::OP_SEMICOLON);
    return varDeclPtr;
  }
  varDeclPtr->addValue(std::move(varValue));
  expect(TType::OP_SEMICOLON);
  return varDeclPtr;
}
