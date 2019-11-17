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

std::unique_ptr<Stmt> Parser::parseReturnStmt() {
    expect(TokenType::KW_RETURN); // expect "return"
    if (has(TokenType::OP_SEMICOLON)) { // if the following token is ";"
        consume(); // eat ";"
        return std::make_unique<ReturnStmt>();
    } else {
        auto returnStmt = std::make_unique<ReturnStmt>();
        //returnStmt->addNode(/*parse expression*/);
        expect(TokenType::OP_SEMICOLON); // expect ";"
        return returnStmt;
    }
}
