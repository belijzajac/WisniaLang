#include "Parser.h"
#include "../lexer/Lexer.h"
#include "../lexer/token/Token.h"
#include "../lexer/token/TokenType.h"
#include "../parser/ast/AST.h"
#include "../../utilities/Exception.h"
#include <algorithm>
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

// TODO: Instead of throwing exceptions, make an option to instead print error msg to STDERR
void Parser::expect(const TokenType &token) {
    if (peek()->getType() != token) {
        throw Exception{"Expected " + TokenTypeToStr[token] + " but found " + TokenTypeToStr[peek()->getType()]};
    } else {
        consume();
    }
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
            else
                throw Exception{"You're fucked, goyim"};
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
    expect(TokenType::KW_FN);         // expect "fn"
    auto fnDef = std::make_unique<FnDef>();

    fnDef->addNode(parseIdent());      // parse <IDENT>
    fnDef->addNode(parseParamsList()); // parse <PARAMS>
    expect(TokenType::OP_FN_ARROW);   // expect "->"
    fnDef->addNode(parseFnType());     // <TYPE>
    fnDef->addNode(parseStmtBlock());  // <STMT_BLOCK>

    return fnDef;
}

// <PARAM> ::= <IDENT> ":" <TYPE>
std::unique_ptr<Param> Parser::parseParam() {
    auto param = std::make_unique<SingleParam>();

    param->addNode(parseIdent());  // parse <IDENT>
    expect(TokenType::OP_COL);    // expect ":"
    param->addNode(parseFnType()); // parse <TYPE>

    return param;
}

// <PARAMS> ::= "(" ")" | "(" <PARAMS_SEQ> ")"
std::unique_ptr<Param> Parser::parseParamsList() {
    expect(TokenType::OP_PAREN_O); // expect "("
    auto paramsList = std::make_unique<ParamsList>();

    // <PARAMS_SEQ> ::= <PARAM> | <PARAMS_SEQ> "," <PARAM>
    while (hasNext() && !has(TokenType::OP_PAREN_C)) {
        paramsList->addNode(parseParam());

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

std::unique_ptr<Stmt> Parser::parseStmt() {
    switch (peek()->getType()) {
        case TokenType::KW_RETURN :
            return parseReturnStmt();
    }
    throw Exception{"You shouldn't have gotten here"};
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

    while (has(TokenType::OP_EQ) || has(TokenType::OP_NE)) { // ... <EQUALITY_SYMB> <COMPARE_EXPR>
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

    // TODO: rewrite in folding expressions? method: hasAny(TokenType::OP_G, TokenType::OP_GE, ...)
    while (has(TokenType::OP_G) || has(TokenType::OP_GE) ||
           has(TokenType::OP_L) || has(TokenType::OP_LE)) { // ... <COMPARISON_SYMB> <ADD_EXPR>

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

    while (has(TokenType::OP_ADD) || has(TokenType::OP_SUB)) { // ... <ADD_OP> <MULT_EXPR>
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

    while (has(TokenType::OP_MUL) || has(TokenType::OP_DIV)) { // ... <MULT_OP> <UNARY_EXPR>
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
    // {!|++} <SOME_EXPR>
    if (has(TokenType::OP_UNEG) || has(TokenType::OP_UADD)) {
        auto lhs = std::unique_ptr<Expr>();

        while (has(TokenType::OP_UNEG) || has(TokenType::OP_UADD)) { // <UNARY_SYM> ...
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

std::unique_ptr<Expr> Parser::parseSomeExpr() {
    // For testing purposes
    expect(TokenType::LIT_INT);
    return std::make_unique<BinaryExpr>(curr());

    //consume();
    //return std::unique_ptr<Expr>();
}

/*
std::unique_ptr<Expr> Parser::parseAndExpr() {
    // For testing purposes
    expect(TokenType::LIT_INT);
    return std::make_unique<BinaryExpr>(curr());

    //consume();
    //return std::unique_ptr<Expr>();
}*/
