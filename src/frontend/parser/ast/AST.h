#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <sstream>
#include "../../lexer/token/Token.h"

// Root node
class AST {
protected:
    std::vector<std::unique_ptr<AST>> children_;    // children nodes
    std::unique_ptr<AST> parent_;                   // parent node
    std::shared_ptr<Token> token_;                  // token
public:
    AST() = default;
    virtual ~AST() = default;

    // Returns what kind of node it is
    virtual const std::string kind() const { return "Root"; }

    // Outputs tree content
    virtual void print(size_t level = 0) const {
        printf("%s%s\n", std::string(level*2, ' ').c_str(), kind().c_str());
        level++;

        for (const auto &node : children_)
            node->print(level);
    }

    // Appends a child
    void addNode(std::unique_ptr<AST> child) { children_.push_back(std::move(child)); }
};

//----------------------------------------------------------------------------------------------------------------------
// Expressions
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Expr node
class Expr : public AST {
protected:
    TokenType op_; // operand for expression (+, *, &&, ...)
public:
    void print(size_t level) const override {
        AST::print(level);
    }
};

// Identifier node
class Identifier : public Expr {
public:
    explicit Identifier(const std::shared_ptr<Token> &tok) { token_ = tok; }
    Identifier() = default;

    const std::string kind() const override { return "Identifier"; }

    void print(size_t level) const override {
        AST::print(level);
        printf("%sValue: %s\n", std::string((level + 1) * 2, ' ').c_str(), token_->getValueStr().c_str());
    }
};

// Boolean Expression node
class BooleanExpr : public Expr {
public:
    explicit BooleanExpr(TokenType opType) { op_ = opType; }
    BooleanExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "BooleanExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Binary Expression node
class BinaryExpr : public Expr {
public:
    explicit BinaryExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    BinaryExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "BinaryExpression" << " (" << token_->getValueStr() << ")";
        return ss.str();
    }
};

//----------------------------------------------------------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Def node
class Def : public AST {
public:
    void print(size_t level) const override {
        AST::print(level);
    }
};

// Function Definition node
class FnDef : public Def {
public:
    explicit FnDef(const std::shared_ptr<Token> &tok) { token_ = tok; }
    FnDef() = default;

    const std::string kind() const override { return "FnDef"; }

    void print(size_t level) const override {
        Def::print(level);
    }
};

// Class Definition node
class ClassDef : public Def {
public:
    explicit ClassDef(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ClassDef() = default;

    const std::string kind() const override { return "ClassDef"; }

    void print(size_t level) const override {
        Def::print(level);
    }
};

//----------------------------------------------------------------------------------------------------------------------
// Parameters
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Param node
class Param : public AST {
public:
    void print(size_t level) const override {
        AST::print(level);
    }
};

// A single parameter
class SingleParam : public Param {
public:
    explicit SingleParam(const std::shared_ptr<Token> &tok) { token_ = tok; }
    SingleParam() = default;

    const std::string kind() const override { return "SingleParam"; }

    void print(size_t level) const override {
        Param::print(level);
    }
};

// Parameters List node
class ParamsList : public Param {
public:
    explicit ParamsList(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ParamsList() = default;

    const std::string kind() const override { return "ParamsList"; }

    void print(size_t level) const override {
        Param::print(level);
    }
};

//----------------------------------------------------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Type node
class Type : public AST {
public:
    void print(size_t level) const override {
        AST::print(level);
    }
};
// Function Type node
class FnType : public Type {
public:
    explicit FnType(const std::shared_ptr<Token> &tok) { token_ = tok; }
    FnType() = default;

    const std::string kind() const override { return "FnType"; }

    void print(size_t level) const override {
        AST::print(level);
        level++;
        printf("%sValue: %s\n", std::string((level + 1) * 2, ' ').c_str(), token_->getName().c_str());
    }
};

//----------------------------------------------------------------------------------------------------------------------
// Statements
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Stmt (statement) node
class Stmt : public AST {
public:
    void print(size_t level) const override {
        AST::print(level);
    }
};

// Statement block node
class StmtBlock : public Stmt {
public:
    explicit StmtBlock(const std::shared_ptr<Token> &tok) { token_ = tok; }
    StmtBlock() = default;

    const std::string kind() const override { return "StmtBlock"; }
};

// Return statement node
class ReturnStmt : public Stmt {
public:
    explicit ReturnStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ReturnStmt() = default;

    const std::string kind() const override { return "ReturnStmt"; }
};

#endif // AST_H
