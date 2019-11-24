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

// Equality Expression node
class EqExpr : public Expr {
public:
    explicit EqExpr(TokenType opType) { op_ = opType; }
    EqExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "EqExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Comparison Expression node
class CompExpr : public Expr {
public:
    explicit CompExpr(TokenType opType) { op_ = opType; }
    CompExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "CompExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Addition (and subtraction) Expression node
class AddExpr : public Expr {
public:
    explicit AddExpr(TokenType opType) { op_ = opType; }
    AddExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "AddExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Multiplication (and division) Expression node
class MultExpr : public Expr {
public:
    explicit MultExpr(TokenType opType) { op_ = opType; }
    MultExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "MultExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Unary Expression node
class UnaryExpr : public Expr {
public:
    explicit UnaryExpr(TokenType opType) { op_ = opType; }
    UnaryExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "UnaryExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Function Expression node
class FnExpr : public Expr {
    std::unique_ptr<Expr> name_;       // function name
    std::shared_ptr<Expr> className_;  // can be nullptr if the function isn't inside a class
    std::unique_ptr<ParamsList> args_; // function arguments
public:
    explicit FnExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    FnExpr() = default;

    // Mutators
    void addFnName(std::unique_ptr<Expr> fnName) { name_ = std::move(fnName); }
    void addClassName(std::shared_ptr<Expr> className) { className_ = className; }
    void addArgs(std::unique_ptr<ParamsList> args) { args_ = std::move(args); }

    // Accessors
    std::shared_ptr<Expr> getClassName() const { return className_; }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "FnExpr";
        return ss.str();
    }

    void print(size_t level) const override {
        Expr::print(level);
        /*
        printf("%s%s\n", std::string(level*2, ' ').c_str(), kind().c_str());
        level++;

        if (className_ != nullptr) className_->print(level);
        name_->print(level);
        args_->print(level);*/
    }
};

// Function Expression node
class ClassInitExpr : public Expr {
    std::unique_ptr<Expr> name_;       // function name
    std::unique_ptr<ParamsList> args_; // function arguments
public:
    explicit ClassInitExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ClassInitExpr() = default;

    // Mutators
    void addName(std::unique_ptr<Expr> fnName) { name_ = std::move(fnName); }
    void addArgs(std::unique_ptr<ParamsList> args) { args_ = std::move(args); }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "ClassInitExpr";
        return ss.str();
    }

    void print(size_t level) const override {
        Expr::print(level);
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

// An abstract definition for constant expression node
class ConstExpr : public Expr {
public:
    const std::string kind() const override {
        std::stringstream ss;

        // Returns a string representation of the value that a token holds
        // It's needed because boolean expressions normally don't hold a value
        auto valueStr = [&]() -> std::string {
            switch (token_->getType()) {
                case TokenType::KW_TRUE:
                    return "true";
                case TokenType::KW_FALSE:
                    return "false";
                default:
                    return token_->getValueStr();
            }
        };

        ss << "ConstExpr" << " (" << valueStr() << ")";
        return ss.str();
    }
};

// A node for Integer expression
class IntExpr : public ConstExpr {
public:
    explicit IntExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    IntExpr() = default;
};

// A node for Float expression
class FloatExpr : public ConstExpr {
public:
    explicit FloatExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    FloatExpr() = default;
};

// A node for Bool expression
class BoolExpr : public ConstExpr {
public:
    explicit BoolExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    BoolExpr() = default;
};

// A node for String expression
class StringExpr : public ConstExpr {
public:
    explicit StringExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    StringExpr() = default;
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
