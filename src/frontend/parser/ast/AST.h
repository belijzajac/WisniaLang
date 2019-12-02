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

// Loop break statement node
class LoopBrkStmt : public Stmt {
public:
    explicit LoopBrkStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    LoopBrkStmt() = default;

    const std::string kind() const override { return "LoopBrkStmt"; }
};

// Variable declaration statement node
class VarDeclStmt : public Stmt {
    std::unique_ptr<Type> type_;  // variable type
    std::unique_ptr<Expr> name_;  // variable name
    std::unique_ptr<Expr> value_; // variable value
public:
    explicit VarDeclStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    VarDeclStmt() = default;

    const std::string kind() const override { return "VarDeclStmt"; }

    // Mutators
    void addType(std::unique_ptr<Type> varType) { type_ = std::move(varType); }
    void addName(std::unique_ptr<Expr> varName) { name_ = std::move(varName); }
    void addValue(std::unique_ptr<Expr> varValue) { value_ = std::move(varValue); }
};

// Variable assignment statement node
class VarAssignStmt : public Stmt {
    std::unique_ptr<Expr> name_;  // variable name
    std::unique_ptr<Expr> value_; // variable value
public:
    explicit VarAssignStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    VarAssignStmt() = default;

    const std::string kind() const override { return "VarAssignStmt"; }

    // Mutators
    void addName(std::unique_ptr<Expr> varName) { name_ = std::move(varName); }
    void addValue(std::unique_ptr<Expr> varValue) { value_ = std::move(varValue); }
};

// Expression statement node
class ExprStmt : public Stmt {
    std::unique_ptr<Expr> expr_;
public:
    explicit ExprStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ExprStmt() = default;

    const std::string kind() const override { return "ExprStmt"; }

    // Mutators
    void addExpr(std::unique_ptr<Expr> expr) { expr_ = std::move(expr); }
};

// Read IO statement node
class readIOStmt : public Stmt {
    std::vector<std::unique_ptr<Expr>> vars_;
public:
    explicit readIOStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    readIOStmt() = default;

    const std::string kind() const override { return "readIOStmt"; }

    // Mutators
    void addVar(std::unique_ptr<Expr> var) { vars_.push_back(std::move(var)); }
};

// Write IO statement node
class writeIOStmt : public Stmt {
    std::vector<std::unique_ptr<Expr>> exprs_;
public:
    explicit writeIOStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    writeIOStmt() = default;

    const std::string kind() const override { return "writeIOStmt"; }

    // Mutators
    void addExpr(std::unique_ptr<Expr> expr) { exprs_.push_back(std::move(expr)); }
};

//----------------------------------------------------------------------------------------------------------------------
// Loops
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Loop node
class Loop : public Stmt {
    std::unique_ptr<Stmt> body_; // surrounded by "{ and "}"
public:
    void print(size_t level) const override {
        AST::print(level);
    }

    // Mutators
    void addBody(std::unique_ptr<Stmt> body) { body_ = std::move(body); }
};

// While loop statement node
class WhileLoop : public Loop {
    std::unique_ptr<Expr> cond_;
public:
    explicit WhileLoop(const std::shared_ptr<Token> &tok) { token_ = tok; }
    WhileLoop() = default;

    const std::string kind() const override { return "WhileLoop"; }

    // Mutators
    void addCond(std::unique_ptr<Expr> expr) { cond_ = std::move(expr); }
};

// For loop statement node
class ForLoop : public Loop {
    std::unique_ptr<Stmt> init_;
    std::unique_ptr<Expr> cond_;
    std::unique_ptr<Expr> incdec_;
public:
    explicit ForLoop(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ForLoop() = default;

    const std::string kind() const override { return "ForLoop"; }

    // Mutators
    void addInit(std::unique_ptr<Stmt> expr) { init_ = std::move(expr); }
    void addCond(std::unique_ptr<Expr> expr) { cond_ = std::move(expr); }
    void addIncDec(std::unique_ptr<Expr> expr) { incdec_ = std::move(expr); }
};

// ForEach loop statement node
class ForEachLoop : public Loop {
    std::unique_ptr<Expr> elem_;     // element
    std::unique_ptr<Expr> iterElem_; // iterable element
public:
    explicit ForEachLoop(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ForEachLoop() = default;

    const std::string kind() const override { return "ForEachLoop"; }

    // Mutators
    void addElem(std::unique_ptr<Expr> expr) { elem_ = std::move(expr); }
    void addIterElem(std::unique_ptr<Expr> expr) { iterElem_ = std::move(expr); }
};

//----------------------------------------------------------------------------------------------------------------------
// Conditional if statements
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for if statement node
class BaseIf : public Stmt {
    std::unique_ptr<Stmt> body_; // surrounded by "{ and "}"
public:
    void print(size_t level) const override {
        AST::print(level);
        //body_->print(level);
    }

    // Mutators
    void addBody(std::unique_ptr<Stmt> body) { body_ = std::move(body); }
};

// If statement node
class IfStmt : public BaseIf {
    std::unique_ptr<Expr> cond_;                     // if condition
    std::vector<std::unique_ptr<BaseIf>> elseBlcks_; // else blocks
public:
    explicit IfStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    IfStmt() = default;

    const std::string kind() const override { return "IfStmt"; }

    void print(size_t level) const override {
        BaseIf::print(level);
        //level++;
        //cond_->print(level);

        for (const auto &elseBl : elseBlcks_)
            elseBl->print(level);
    }

    // Mutators
    void addCond(std::unique_ptr<Expr> expr) { cond_ = std::move(expr); }
    void addElseBlocks( std::vector<std::unique_ptr<BaseIf>> expr) { elseBlcks_ = std::move(expr); }
};

// Else statement node
class ElseStmt : public BaseIf {
public:
    explicit ElseStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ElseStmt() = default;

    const std::string kind() const override { return "ElseStmt"; }

    void print(size_t level) const override {
        BaseIf::print(level);
    }
};

// Else If statement node
class ElseIfStmt : public BaseIf {
    std::unique_ptr<Expr> cond_;   // else if condition
public:
    explicit ElseIfStmt(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ElseIfStmt() = default;

    const std::string kind() const override { return "ElseIfStmt"; }

    void print(size_t level) const override {
        BaseIf::print(level);
    }

    // Mutators
    void addCond(std::unique_ptr<Expr> expr) { cond_ = std::move(expr); }
};

#endif // AST_H
