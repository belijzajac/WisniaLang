#include "Parser.h"
#include "../lexer/Lexer.h"
#include "../lexer/token/Token.h"
#include "../lexer/token/TokenType.h"
#include "../parser/ast/AST.h"
#include "../../utilities/Exception.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>

Parser::Parser(const Lexer &lexer) {
    tokens_ = lexer.getTokens();

    // Parse and output AST
    auto root = parse();
    root->print();
}

bool Parser::has(const TokenType &token) {
    return peek()->getType() == token;
}

bool Parser::has2(const TokenType &token) {
    return tokens_.at(pos_ + 2)->getType() == token;
}

// TODO: Instead of throwing exceptions, make an option to instead print error msg to STDERR
void Parser::expect(const TokenType &token) {
    if (peek()->getType() != token)
        throw Exception{"Expected " + TokenTypeToStr[token] + " but found " + TokenTypeToStr[peek()->getType()]};
    else
        consume();
}

std::unique_ptr<AST> Parser::parse() {
    auto root = std::make_unique<AST>();

    try {
        // continue parsing as long as there are tokens
        while (!has(TokenType::TOK_EOF)) {
            // TODO: Rewrite in switch block
            // TODO: switch ( peek() )
            if (has(TokenType::KW_FN))
                root->addNode(parseFnDef());
            else if (has(TokenType::KW_CLASS))
                root->addNode(parseClassDef());
            else
                throw Exception{"Not a global definition of either a class, or a function"};
        }
    } catch (const Exception &error) {
        std::cerr << "Parser error: " << error.what() << "\n";
    }

    return root;
}

std::unique_ptr<Expr> Parser::parseIdent() {
    if (has(TokenType::IDENT)) {
        consume(); // eat identifier
        return std::make_unique<Identifier>(curr());
    } else {
        throw Exception{"Not an identifier"};
    }
}

// <FN_DECL> ::= "fn" <IDENT> <PARAMS> "->" <TYPE> <STMT_BLOCK>
std::unique_ptr<Def> Parser::parseFnDef() {
    expect(TokenType::KW_FN);                     // expect "fn"

    consume();                                    // eat identifier
    auto fnDef = std::make_unique<FnDef>(curr()); // parse <IDENT>

    fnDef->addParams(parseParamsList()); // parse <PARAMS>
    expect(TokenType::OP_FN_ARROW);      // expect "->"
    fnDef->addRetType(parseFnType());    // <TYPE>
    fnDef->addBody(parseStmtBlock());    // <STMT_BLOCK>

    return fnDef;
}

// <PARAM> ::= <IDENT> ":" <TYPE>
std::unique_ptr<Param> Parser::parseParam() {
    auto param = std::make_unique<Param>();

    param->addNode(parseIdent());  // parse <IDENT>
    expect(TokenType::OP_COL);    // expect ":"
    param->addNode(parseFnType()); // parse <TYPE>

    return param;
}

// <PARAMS> ::= "(" ")" | "(" <PARAMS_SEQ> ")"
std::vector<std::unique_ptr<Param>> Parser::parseParamsList() {
    expect(TokenType::OP_PAREN_O); // expect "("
    std::vector<std::unique_ptr<Param>> paramsList;

    // <PARAMS_SEQ> ::= <PARAM> | <PARAMS_SEQ> "," <PARAM>
    while (hasNext() && !has(TokenType::OP_PAREN_C)) {
        paramsList.push_back(parseParam());

        // Check whether we've parsed all params already
        if (has(TokenType::OP_PAREN_C))
            break;

        // If not, continue parsing
        expect(TokenType::OP_COMMA);
    }
    expect(TokenType::OP_PAREN_C); // expect ")"
    return paramsList;
}

// Primitive function return types
static inline std::vector<TokenType> FnTypes {
    TokenType::KW_VOID,
    TokenType::KW_INT,
    TokenType::KW_BOOL,
    TokenType::KW_FLOAT,
    TokenType::KW_STRING
};

// <TYPE> ::= "void" | "int" | "bool" | "float" | "string"
std::unique_ptr<Type> Parser::parseFnType() {
    if (std::any_of(FnTypes.begin(), FnTypes.end(), [&](TokenType t) { return peek()->getType() == t; })) {
        consume(); // eat function type
        return std::make_unique<FnType>(curr());
    } else {
        throw Exception{"Function definition doesn't have any of the supported types"};
    }
}

// <STMT_BLOCK> ::= "{" "}" | "{" <STMTS> "}"
std::unique_ptr<Stmt> Parser::parseStmtBlock() {
    if (has(TokenType::OP_BRACE_O)) {
        consume(); // eat "{"
        auto stmtBlock = std::make_unique<StmtBlock>();

        // <STMTS> ::= <STMT> | <STMTS> <STMT>
        while (hasNext() && !has(TokenType::OP_BRACE_C))
            stmtBlock->addNode(parseStmt());

        expect(TokenType::OP_BRACE_C); // expect "}"
        return stmtBlock;
    } else {
        return parseStmt();
    }
}

// <STMT> ::= <FN_RETURN_STMT> <STMT_END>
//          | <LOOP_BREAK_STMT> <STMT_END>
//          | <VAR_DECL> <STMT_END>
//          | <ASSIGNMENT_STMT> <STMT_END>
//          | <EXPRESSION> <STMT_END>
//          | <IO_STMT> <STMT_END>
//          | <LOOP_STMT>
//          | <IF_STMT>
std::unique_ptr<Stmt> Parser::parseStmt() {
    switch (peek()->getType()) {
        // <FN_RETURN_STMT>
        case TokenType::KW_RETURN :
            return parseReturnStmt();

        // <LOOP_BREAK_STMT>
        case TokenType::KW_BREAK :
        case TokenType::KW_CONTINUE :
            return parseLoopBrkStmt();

        // <VAR_DECL>
        case TokenType::KW_VOID :
        case TokenType::KW_INT :
        case TokenType::KW_BOOL :
        case TokenType::KW_FLOAT :
        case TokenType::KW_STRING :
            if (has2(TokenType::IDENT))
                return parseVarDeclStmt();

        // <ASSIGNMENT_STMT>
        case TokenType::IDENT :
            if (has2(TokenType::OP_ASSN))
                return parseVarAssignStmt();

        // <IO_STMT>
        case TokenType::KW_READ :
        case TokenType::KW_PRINT :
            return parseIOStmt();

        // <LOOP_STMT>
        case TokenType::KW_WHILE :
        case TokenType::KW_FOR :
        case TokenType::KW_FOREACH :
            return parseLoops();

        // <IF_STMT>
        case TokenType::KW_IF:
            return parseIfBlock();

        // <EXPRESSION>
        default:
            return parseExprStmt();
    }
}

// <FN_RETURN_STMT> <STMT_END>
// <FN_RETURN_STMT> ::= <RETURN_SYMB> | <RETURN_SYMB> <EXPRESSION>
std::unique_ptr<Stmt> Parser::parseReturnStmt() {
    expect(TokenType::KW_RETURN); // expect "return"
    if (has(TokenType::OP_SEMICOLON)) { // if the following token is ";"
        consume(); // eat ";"
        return std::make_unique<ReturnStmt>();
    } else {
        auto returnStmt = std::make_unique<ReturnStmt>();
        returnStmt->addNode(parseExpr());
        expect(TokenType::OP_SEMICOLON); // expect ";"
        return returnStmt;
    }
}

// <EXPRESSION> ::= <AND_EXPR> | <EXPRESSION> <OR_SYMB> <AND_EXPR>
// <EXPRESSION> ::= <AND_EXPR> { <OR_SYMB> <AND_EXPR> }
std::unique_ptr<Expr> Parser::parseExpr() {
    auto lhs = parseAndExpr();

    while (has(TokenType::OP_OR)) { // ... <OR_SYMB> <AND_EXPR>
        expect(TokenType::OP_OR);   // expect "||"
        auto rhs = parseAndExpr();

        // Make a temporary copy of the lhs
        auto tempLhs = std::unique_ptr<Expr>(std::move(lhs));

        // Move rhs and tempLhs nodes
        lhs = std::make_unique<BooleanExpr>(TokenType::OP_OR);
        lhs->addNode(std::move(tempLhs));
        lhs->addNode(std::move(rhs));
    }
    return lhs;
}

// <AND_EXPR> ::= <EQUAL_EXPR> | <AND_EXPR> <AND_SYMB> <EQUAL_EXPR>
// <AND_EXPR> ::= <EQUAL_EXPR> { <AND_SYMB> <EQUAL_EXPR> }
std::unique_ptr<Expr> Parser::parseAndExpr() {
    auto lhs = parseEqExpr();

    while (has(TokenType::OP_AND)) { // ... <AND_SYMB> <EQUAL_EXPR>
        expect(TokenType::OP_AND);   // expect "&&"
        auto rhs = parseEqExpr();

        // Make a temporary copy of the lhs
        auto tempLhs = std::unique_ptr<Expr>(std::move(lhs));

        // Move rhs and tempLhs nodes
        lhs = std::make_unique<BooleanExpr>(TokenType::OP_AND);
        lhs->addNode(std::move(tempLhs));
        lhs->addNode(std::move(rhs));
    }
    return lhs;
}

// <EQUAL_EXPR> ::= <COMPARE_EXPR> | <EQUAL_EXPR> <EQUALITY_SYMB> <COMPARE_EXPR>
// <EQUAL_EXPR> ::= <COMPARE_EXPR> { <EQUALITY_SYMB> <COMPARE_EXPR> }
std::unique_ptr<Expr> Parser::parseEqExpr() {
    auto lhs = parseCompExpr();

    while (hasAnyOf(TokenType::OP_EQ, TokenType::OP_NE)) { // ... <EQUALITY_SYMB> <COMPARE_EXPR>
        auto tokType = peek()->getType();
        expect(tokType); // expect either "==" or "!="
        auto rhs = parseCompExpr();

        // Make a temporary copy of the lhs
        auto tempLhs = std::unique_ptr<Expr>(std::move(lhs));

        // Move rhs and tempLhs nodes
        lhs = std::make_unique<EqExpr>(tokType);
        lhs->addNode(std::move(tempLhs));
        lhs->addNode(std::move(rhs));
    }
    return lhs;
}

// <COMPARE_EXPR> ::= <ADD_EXPR> | <COMPARE_EXPR> <COMPARISON_SYMB> <ADD_EXPR>
// <COMPARE_EXPR> ::= <ADD_EXPR> { <COMPARISON_SYMB> <ADD_EXPR> }
std::unique_ptr<Expr> Parser::parseCompExpr() {
    auto lhs = parseAddExpr();

    while (hasAnyOf(TokenType::OP_G, TokenType::OP_GE, TokenType::OP_L, TokenType::OP_LE)) {  // ... <COMPARISON_SYMB> <ADD_EXPR>
        auto tokType = peek()->getType();
        expect(tokType); // expect any of ">", ">=", "<", "<="
        auto rhs = parseAddExpr();

        // Make a temporary copy of the lhs
        auto tempLhs = std::unique_ptr<Expr>(std::move(lhs));

        // Move rhs and tempLhs nodes
        lhs = std::make_unique<CompExpr>(tokType);
        lhs->addNode(std::move(tempLhs));
        lhs->addNode(std::move(rhs));
    }
    return lhs;
}

// <ADD_EXPR> ::= <MULT_EXPR> | <ADD_EXPR> <ADD_OP> <MULT_EXPR>
// <ADD_EXPR> ::= <MULT_EXPR> { <ADD_OP> <MULT_EXPR> }
std::unique_ptr<Expr> Parser::parseAddExpr() {
    auto lhs = parseMultExpr();

    while (hasAnyOf(TokenType::OP_ADD, TokenType::OP_SUB)) { // ... <ADD_OP> <MULT_EXPR>
        auto tokType = peek()->getType();
        expect(tokType); // expect either "+" or "-"
        auto rhs = parseMultExpr();

        // Make a temporary copy of the lhs
        auto tempLhs = std::unique_ptr<Expr>(std::move(lhs));

        // Move rhs and tempLhs nodes
        lhs = std::make_unique<AddExpr>(tokType);
        lhs->addNode(std::move(tempLhs));
        lhs->addNode(std::move(rhs));
    }
    return lhs;
}
// <MULT_EXPR> ::= <UNARY_EXPR> | <MULT_EXPR> <MULT_OP> <UNARY_EXPR>
// <MULT_EXPR> ::= <UNARY_EXPR> { <MULT_OP> <UNARY_EXPR> }
std::unique_ptr<Expr> Parser::parseMultExpr() {
    auto lhs = parseUnaryExpr();

    while (hasAnyOf(TokenType::OP_MUL, TokenType::OP_DIV)) { // ... <MULT_OP> <UNARY_EXPR>
        auto tokType = peek()->getType();
        expect(tokType); // expect either "*" or "/"
        auto rhs = parseUnaryExpr();

        // Make a temporary copy of the lhs
        auto tempLhs = std::unique_ptr<Expr>(std::move(lhs));

        // Move rhs and tempLhs nodes
        lhs = std::make_unique<MultExpr>(tokType);
        lhs->addNode(std::move(tempLhs));
        lhs->addNode(std::move(rhs));
    }
    return lhs;
}

// <UNARY_EXPR> ::= <SOME_EXPR> | <UNARY_SYM> <UNARY_EXPR>
// <UNARY_EXPR> ::= {UNARY_SYM} <SOME_EXPR>
std::unique_ptr<Expr> Parser::parseUnaryExpr() {
    // Checks for acceptable token types
    auto isAnyOf { [&]() -> bool { return hasAnyOf(TokenType::OP_UNEG, TokenType::OP_UADD); } };

    // {!|++} <SOME_EXPR>
    if (isAnyOf()) {
        auto lhs = std::unique_ptr<Expr>();

        while (isAnyOf()) { // <UNARY_SYM> ...
            auto tokType = peek()->getType();
            expect(tokType); // expect either "!" or "++"
            auto rhs = parseUnaryExpr();

            // Append the unary expression we've just found
            lhs = std::make_unique<UnaryExpr>(tokType);
            lhs->addNode(std::move(rhs));
        }
        return lhs;
    } else { // <SOME_EXPR>
        return parseSomeExpr();
    }
}

// <SOME_EXPR> ::= "(" <EXPRESSION> ")" | <VAR> | <FN_CALL> | <CLASS_M_CALL> | <CONSTANT_LIT> | <CLASS_INIT>
std::unique_ptr<Expr> Parser::parseSomeExpr() {
    switch (peek()->getType()) {
        // "(" <EXPRESSION> ")"
        case TokenType::OP_PAREN_O: {
            expect(TokenType::OP_PAREN_O); // eat "("
            auto exp = parseExpr();
            expect(TokenType::OP_PAREN_C); // eat ")"
            return exp;
        }
        // <VAR> | <FN_CALL> | <CLASS_M_CALL>
        case TokenType::IDENT:
            return parseVarExp();

        // <CLASS_INIT>
        case TokenType::KW_CLASS_INIT:
            return parseClassInit();

        // <CONSTANT_LIT>
        default:
            return parseConstExpr();
    }
}

// Returns any of the following: <VAR> | <FN_CALL> | <CLASS_M_CALL>
std::unique_ptr<Expr> Parser::parseVarExp() {
    if (has(TokenType::IDENT)) { // a
        // <FN_CALL>
        if (has2(TokenType::OP_PAREN_O) || has2(TokenType::OP_BRACE_O)) // a( or a{
            return parseFnCall();

        // <CLASS_M_CALL>
        else if (has2(TokenType::OP_METHOD_CALL) || has2(TokenType::OP_FN_ARROW)) // a. or a->
            return parseMethodCall();

        // <VAR>
        else
            return parseIdent();
    } else {
        ; // do nothing. The branch is never reached
    }
    throw Exception{"Undefined expression"};
}

// <FN_CALL> ::= <IDENT> <ARGUMENTS>
std::unique_ptr<Expr> Parser::parseFnCall() {
    auto fnCallPtr = std::make_unique<FnExpr>();
    fnCallPtr->addFnName(parseIdent());
    fnCallPtr->addArgs(parseArgsList());
    return fnCallPtr;
}

// <CLASS_M_CALL> ::= <IDENT> <CLASS_M_CALL_SYM> <VAR> <ARGUMENTS {<CLASS_M_CALL_SYM> <VAR> <ARGUMENTS}
std::unique_ptr<Expr> Parser::parseMethodCall() {
    auto methodCallPtr = std::make_unique<FnExpr>();
    methodCallPtr->addClassName(parseIdent());

    // eat "." or "->"
    hasAnyOf(TokenType::OP_METHOD_CALL, TokenType::OP_FN_ARROW) ? consume() : throw Exception{"Unknown method call symbol"};

    methodCallPtr->addFnName(parseIdent());
    methodCallPtr->addArgs(parseArgsList());

    while (hasAnyOf(TokenType::OP_METHOD_CALL, TokenType::OP_FN_ARROW)) {
        consume(); // eat "." or "->"
        auto rhs = std::make_unique<FnExpr>();
        rhs->addClassName(methodCallPtr->getClassName());
        rhs->addFnName(parseIdent());
        rhs->addArgs(parseArgsList());

        // Make a temporary copy of the lhs
        auto tempLhs = std::unique_ptr<Expr>(std::move(methodCallPtr));

        // Move rhs and tempLhs nodes
        methodCallPtr = std::make_unique<FnExpr>();
        methodCallPtr->addNode(std::move(tempLhs));
        methodCallPtr->addNode(std::move(rhs));
    }
    return methodCallPtr;
}

// <ARGUMENTS> ::= "{" "}" | "{" <EXPR_LIST> "}" | "(" ")" | "(" <EXPR_LIST> ")"
std::vector<std::unique_ptr<Param>> Parser::parseArgsList() {
    // To map possible argsBody types to their selectable closing body types
    std::unordered_map<TokenType, TokenType> expectBodyType = {
        {TokenType::OP_PAREN_O, TokenType::OP_PAREN_C},
        {TokenType::OP_BRACE_O, TokenType::OP_BRACE_C}
    };

    consume(); // idk why's this required,
               // but it solved the issue with incorrect token types

    auto argsCurrType = curr()->getType();              // either "(" or "{"
    auto argsExpType = expectBodyType.at(argsCurrType); // the opposite of argsCurrType
    std::vector<std::unique_ptr<Param>> argsList;       // to store function arguments

    // <EXPR_LIST> ::= <EXPRESSION> | <EXPR_LIST> "," <EXPRESSION>
    while (hasNext() && !has(argsExpType)) {
        // Parse a single argument
        auto arg = std::make_unique<Param>();
        arg->addNode(parseExpr()); // parse <EXPRESSION>
        argsList.push_back(std::move(arg));

        // Check whether we've parsed all args
        if (has(argsExpType))
            break;

        // If not, continue parsing
        expect(TokenType::OP_COMMA);
    }
    expect(argsExpType); // expect either ")" or "}"
    return argsList;
}

// <CLASS_INIT> ::= "new" <IDENT> <ARGUMENTS>
std::unique_ptr<Expr> Parser::parseClassInit() {
    expect(TokenType::KW_CLASS_INIT); // expect "new"
    auto classInitPtr = std::make_unique<ClassInitExpr>();
    classInitPtr->addName(parseIdent());
    classInitPtr->addArgs(parseArgsList());
    return classInitPtr;
}

// <CONSTANT_LIT> ::= <BOOL_CONSTANT> | <NUMERIC_CONSTANT> | <STRING>
std::unique_ptr<Expr> Parser::parseConstExpr() {
    switch (peek()->getType()) {
        // <NUMERIC_CONSTANT> : integer
        case TokenType::LIT_INT: {
            consume();
            auto intExpr = std::make_unique<IntExpr>(curr());
            return intExpr;
        }
        // <NUMERIC_CONSTANT> : float
        case TokenType::LIT_FLT: {
            consume();
            auto fltExpr = std::make_unique<FloatExpr>(curr());
            return fltExpr;
        }
        // <BOOL_CONSTANT>
        case TokenType::KW_TRUE:
        case TokenType::KW_FALSE:
        {
            consume();
            auto boolExpr = std::make_unique<BoolExpr>(curr());
            return boolExpr;
        }
        // <STRING>
        case TokenType::LIT_STR: {
            consume();
            auto strExpr = std::make_unique<StringExpr>(curr());
            return strExpr;
        }
    }
    throw Exception{"Unknown constant expression"};
}

// <LOOP_BREAK_STMT> <STMT_END>
// <LOOP_BREAK_STMT> ::= <BREAK_SYMB> | <CONTINUE_SYMB>
std::unique_ptr<Stmt> Parser::parseLoopBrkStmt() {
    // expect either "break" or "continue"
    if (hasAnyOf(TokenType::KW_BREAK, TokenType::KW_CONTINUE)) {
        consume(); // eat either "break" or "continue"

        // if the following token is ";"
        if (has(TokenType::OP_SEMICOLON)) {
            consume(); // eat ";"
            return std::make_unique<LoopBrkStmt>();
        }
    }
    throw Exception{"Unterminated loop break statement"};
}

// <VAR_DECL> <STMT_END>
// <VAR_DECL> ::= <TYPE> <VAR> | <TYPE> <VAR> "=" <EXPRESSION> | <TYPE> <VAR> "{" <EXPRESSION> "}"
std::unique_ptr<Stmt> Parser::parseVarDeclStmt() {
    auto varDeclPtr = std::make_unique<VarDeclStmt>();

    varDeclPtr->addType(parseFnType());
    varDeclPtr->addName((parseIdent()));
    std::unique_ptr<Expr> varValue;

    // <TYPE> <VAR> "=" <EXPRESSION>
    if (has(TokenType::OP_ASSN)) {
        consume(); // eat "="
        varValue = parseExpr();
    }
    // <TYPE> <VAR> "{" <EXPRESSION> "}"
    else if (has(TokenType::OP_BRACE_O)) {
        consume(); // eat "{"
        varValue = parseExpr();
        consume(); // eat "}"
    }
    // <TYPE> <VAR>
    else {
        expect(TokenType::OP_SEMICOLON);
        return varDeclPtr;
    }
    varDeclPtr->addValue(std::move(varValue));
    expect(TokenType::OP_SEMICOLON);
    return varDeclPtr;
}

// <ASSIGNMENT_STMT> <STMT_END>
// <ASSIGNMENT_STMT> ::= <VAR> "=" <EXPRESSION>
std::unique_ptr<Stmt> Parser::parseVarAssignStmt() {
    auto varAssignPtr = std::make_unique<VarAssignStmt>();

    varAssignPtr->addName((parseIdent()));
    expect(TokenType::OP_ASSN); // eat "="
    varAssignPtr->addValue(parseExpr());

    expect(TokenType::OP_SEMICOLON);
    return varAssignPtr;
}

// <EXPRESSION> <STMT_END>
std::unique_ptr<Stmt> Parser::parseExprStmt() {
    auto exprStmtPtr = std::make_unique<ExprStmt>();
    exprStmtPtr->addExpr(parseExpr());
    expect(TokenType::OP_SEMICOLON);
    return exprStmtPtr;
}

// <IO_STMT> ::= <INPUT_STMT> | <OUTPUT_STMT>
std::unique_ptr<Stmt> Parser::parseIOStmt() {
    if (has(TokenType::KW_READ))
        return parseReadIOStmt();
    else
        return parseWriteIOStmt();
}

// <INPUT_STMT> ::= "read" <INPUT_SEQ>
// <INPUT_SEQ>  ::= <VAR> | <INPUT_SEQ> "," <VAR>
std::unique_ptr<Stmt> Parser::parseReadIOStmt() {
    consume(); // eat "read"
    auto readIO = std::make_unique<readIOStmt>();

    // <INPUT_SEQ>
    while (hasNext()) { // a, b, c
        readIO->addVar(parseIdent());

        if (has(TokenType::OP_COMMA))
            consume(); // eat ","
        else
            break;
    }

    expect(TokenType::OP_SEMICOLON);
    return readIO;
}

// <OUTPUT_STMT> ::= "print" <OUTPUT_SEQ>
// <OUTPUT_SEQ>  ::= <EXPRESSION> | <OUTPUT_SEQ> "," <EXPRESSION>
std::unique_ptr<Stmt> Parser::parseWriteIOStmt() {
    consume(); // eat "print"
    auto writeIO = std::make_unique<writeIOStmt>();

    // <OUTPUT_SEQ>
    while (hasNext()) { // a, b, c
        writeIO->addExpr(parseExpr());

        if (has(TokenType::OP_COMMA))
            consume(); // eat ","
        else
            break;
    }

    expect(TokenType::OP_SEMICOLON);
    return writeIO;
}

// <LOOP_STMT> ::= <WHILE_LOOP> | <FOR_LOOP> | <FOREACH_LOOP>
std::unique_ptr<Loop> Parser::parseLoops() {
    // <WHILE_LOOP>
    if (has(TokenType::KW_WHILE))
        return parseWhileLoop();
    // <FOR_LOOP>
    else if (has(TokenType::KW_FOR))
        return parseForLoop();
    // <FOREACH_LOOP>
    else
        return parseForEachLoop();
}

// <WHILE_LOOP> ::= "while" "(" <EXPRESSION> ")" <STMT_BLOCK>
std::unique_ptr<Loop> Parser::parseWhileLoop() {
    auto whileLoopPtr = std::make_unique<WhileLoop>();

    consume();                               // eat "while"
    expect(TokenType::OP_PAREN_O);           // expect "("
    whileLoopPtr->addCond(parseExpr());      // <EXPRESSION>
    expect(TokenType::OP_PAREN_C);           // expect ")"
    whileLoopPtr->addBody(parseStmtBlock()); // { ... }

    return whileLoopPtr;
}

// <FOR_LOOP> ::= "for" "(" <FOR_CONDITION> ")" <STMT_BLOCK>
// <FOR_CONDITION> ::= <VAR_DECL> ";" <EXPRESSION> ";" <EXPRESSION>
std::unique_ptr<Loop> Parser::parseForLoop() {
    auto forLoopPtr = std::make_unique<ForLoop>();

    consume();                            // eat "for"
    expect(TokenType::OP_PAREN_O);        // expect "("

    // For loop body
    forLoopPtr->addInit(parseVarDeclStmt()); // parses var declaration, which is expected to end with a semicolon
    forLoopPtr->addCond(parseExpr());
    expect(TokenType::OP_SEMICOLON);
    forLoopPtr->addIncDec(parseExpr());

    expect(TokenType::OP_PAREN_C);         // expect ")"
    forLoopPtr->addBody(parseStmtBlock()); // { ... }

    return forLoopPtr;
}

// <FOREACH_LOOP> ::= "for_each" "(" <FOREACH_CONDITION> ")" <STMT_BLOCK>
// <FOREACH_CONDITION> ::= <VAR> "in" <EXPRESSION>
std::unique_ptr<Loop> Parser::parseForEachLoop() {
    auto foreachLoopPtr = std::make_unique<ForEachLoop>();

    consume();                     // eat "for_each"
    expect(TokenType::OP_PAREN_O); // expect "("

    // Foreach loop body
    foreachLoopPtr->addElem(parseIdent());
    expect(TokenType::KW_FOREACH_IN);
    foreachLoopPtr->addIterElem(parseExpr());

    expect(TokenType::OP_PAREN_C);             // expect ")"
    foreachLoopPtr->addBody(parseStmtBlock()); // { ... }

    return foreachLoopPtr;
}

// <IF_STMT> ::= <IF_BLOCK> [<MULTIPLE_ELSE_IF_BLOCK>]
std::unique_ptr<BaseIf> Parser::parseIfBlock() {
    auto ifStmtPtr = std::make_unique<IfStmt>();

    // <IF_ELSE_COND_BODY> ::= "(" <EXPRESSION> ")"
    auto ifCond = [&]() -> std::unique_ptr<Expr> {
        expect(TokenType::KW_IF);      // expect "if"
        expect(TokenType::OP_PAREN_O); // expect "("
        auto cond = parseExpr();       // parse if header condition
        expect(TokenType::OP_PAREN_C); // expect ")"
        return cond;
    };

    ifStmtPtr->addCond(ifCond());
    ifStmtPtr->addBody(parseStmtBlock());

    // a vector of else statement blocks
    std::vector<std::unique_ptr<BaseIf>> elseBlocks;

    // Parse else blocks
    // [<MULTIPLE_ELSE_IF_BLOCK>]
    if (hasAnyOf(TokenType::KW_ELIF, TokenType::KW_ELSE)) {
        elseBlocks = parseMultipleElseBlock();
        ifStmtPtr->addElseBlocks(std::move(elseBlocks));
    } else {
        return ifStmtPtr;
    }

    return ifStmtPtr;
}

// <MULTIPLE_ELSE_IF_BLOCK> ::= <MULTIPLE_ELIF> | <ELSE_BLOCK>
// <MULTIPLE_ELIF> ::= <ELIF_BLOCK> {<ELIF_BLOCK>}
std::vector<std::unique_ptr<BaseIf>> Parser::parseMultipleElseBlock() {
    std::vector<std::unique_ptr<BaseIf>> elseBlocks;

    // <ELIF_BLOCK> ::= "elif" "(" <EXPRESSION> ")" <STMT_BLOCK>
    auto elifBodyPtr = [&]() -> std::unique_ptr<ElseIfStmt> {
        expect(TokenType::KW_ELIF);

        // parse elif condition
        expect(TokenType::OP_PAREN_O); // expect "("
        auto cond = parseExpr();       // parse if header condition
        expect(TokenType::OP_PAREN_C); // expect ")"

        // construct elif statement pointer
        auto elifStmtPtr = std::make_unique<ElseIfStmt>();
        elifStmtPtr->addCond(std::move(cond));
        elifStmtPtr->addBody(parseStmtBlock());
        return elifStmtPtr;
    };

    // <ELSE_BLOCK> ::= "else" <STMT_BLOCK>
    auto elseBodyPtr = [&]() -> std::unique_ptr<ElseStmt> {
        expect(TokenType::KW_ELSE);
        auto elseStmtPtr = std::make_unique<ElseStmt>();
        elseStmtPtr->addBody(parseStmtBlock());
        return elseStmtPtr;
    };

    // Parse elif blocks while there are any left
    while (has(TokenType::KW_ELIF))
        elseBlocks.push_back(std::move(elifBodyPtr()));

    // Eventually, parse the else block
    if (has(TokenType::KW_ELSE))
        elseBlocks.push_back(std::move(elseBodyPtr()));

    return elseBlocks;
}

// <CLASS_DECL> ::= "class" <IDENT> <CLASS_BODY>
std::unique_ptr<Def> Parser::parseClassDef() {
    expect(TokenType::KW_CLASS); // expect "class"

    consume();                                    // eat identifier
    auto classDef = std::make_unique<ClassDef>(curr()); // parse <IDENT>

    // Parse <CLASS_BODY>
    // <CLASS_BODY> ::= "{" "}" | "{" <CLASS_STMTS> "}"
    if (has(TokenType::OP_BRACE_O)) {
        consume(); // eat "{"

        // <CLASS_STMT> ::= <METHOD_DECLS> | <FIELD_DECLS>
        while (hasNext() && !has(TokenType::OP_BRACE_C)) {
            switch (peek()->getType()) {
                // <CONSTRUCTOR_DECL>
                case TokenType::KW_CLASS_DEF :
                    classDef->addConstructor(parseClassCtorDef());
                    break;
                // <DESTRUCTOR_DECL>
                case TokenType::KW_CLASS_REM :
                    classDef->addDestructor(parseClassDtorDef());
                    break;
                // <FN_DECL>
                case TokenType::KW_FN :
                    classDef->addMethod(parseFnDef());
                    break;
                // <FIELD_DECLS>
                default:
                    classDef->addField(parseVarDeclStmt());
                    break;
            }
        }
        expect(TokenType::OP_BRACE_C); // expect "}"
    }

    return classDef;
}

// <CONSTRUCTOR_DECL> ::= <CONSTRUCTOR_DEF> <IDENT> <PARAMS> <STMT_BLOCK>
std::unique_ptr<Def> Parser::parseClassCtorDef() {
    expect(TokenType::KW_CLASS_DEF);                         // expect "def"

    consume();                                               // eat identifier
    auto ctorDef = std::make_unique<ConstructorDef>(curr()); // parse <IDENT>

    ctorDef->addParams(parseParamsList()); // parse <PARAMS>
    ctorDef->addBody(parseStmtBlock());    // <STMT_BLOCK>

    return ctorDef;
}

// <DESTRUCTOR_DECL> ::= <DESTRUCTOR_DEF>  <IDENT> <PARAMS> <STMT_BLOCK>
std::unique_ptr<Def> Parser::parseClassDtorDef() {
    expect(TokenType::KW_CLASS_REM);                        // expect "rem"

    consume();                                              // eat identifier
    auto dtorDef = std::make_unique<DestructorDef>(curr()); // parse <IDENT>

    dtorDef->addParams(parseParamsList()); // parse <PARAMS>
    dtorDef->addBody(parseStmtBlock());    // <STMT_BLOCK>

    return dtorDef;
}
