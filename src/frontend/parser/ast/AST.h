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

    // Access children
    AST *first() const { return children_.at(0).get(); }
    AST *second() const { return children_.at(1).get(); }
};

//----------------------------------------------------------------------------------------------------------------------
// Parameters
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Param node
class Param : public AST {
public:
    explicit Param(const std::shared_ptr<Token> &tok) { token_ = tok; }
    Param() = default;

    const std::string kind() const override { return "Param"; }

    void print(size_t level) const override {
        AST::print(level);
    }
};

//----------------------------------------------------------------------------------------------------------------------
// Expressions
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Expr node
class Expr : public AST {
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

    const std::string getValue() const { return token_->getValueStr(); }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "Var" << " (" << token_->getValueStr() << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        AST::print(level);
    }
};

// Binary Expression node
class BinaryExpr : public Expr {
protected:
    TokenType op_; // operand for expression (+, *, &&, ...)
public:
    explicit BinaryExpr(TokenType opType) { op_ = opType; }
    BinaryExpr() = default;

    // Accessors
    Expr *lhs() const { return static_cast<Expr*>(first()); }
    Expr *rhs() const { return static_cast<Expr*>(second()); }

    // TODO:
    const std::string kind() const override {
        std::stringstream ss;
        ss << "BinaryExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        AST::print(level);
        level++;

        //lhs()->print(level);
        //rhs()->print(level);
    }
};

// Boolean Expression node
class BooleanExpr : public BinaryExpr {
public:
    explicit BooleanExpr(TokenType opType) : BinaryExpr(opType) {}
    BooleanExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "BooleanExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Equality Expression node
class EqExpr : public BinaryExpr {
public:
    explicit EqExpr(TokenType opType) : BinaryExpr(opType) {}
    EqExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "EqExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Comparison Expression node
class CompExpr : public BinaryExpr {
public:
    explicit CompExpr(TokenType opType) : BinaryExpr(opType) {}
    CompExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "CompExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Addition (and subtraction) Expression node
class AddExpr : public BinaryExpr {
public:
    explicit AddExpr(TokenType opType) : BinaryExpr(opType) {}
    AddExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "AddExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Multiplication (and division) Expression node
class MultExpr : public BinaryExpr {
public:
    explicit MultExpr(TokenType opType) : BinaryExpr(opType) {}
    MultExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "MultExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Unary Expression node
class UnaryExpr : public BinaryExpr {
public:
    explicit UnaryExpr(TokenType opType) : BinaryExpr(opType) {}
    UnaryExpr() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "UnaryExpr" << " (" << TokenTypeToStr[op_] << ")";
        return ss.str();
    }
};

// Function Call Expression node
class FnCallExpr : public Expr {
    std::shared_ptr<Identifier> className_;    // can be nullptr if the function isn't inside a class
    std::vector<std::unique_ptr<Param>> args_; // function arguments
public:
    explicit FnCallExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    FnCallExpr() = default;

    // Mutators
    void addClassName(std::shared_ptr<Identifier> className) { className_ = className; }
    void addArgs(std::vector<std::unique_ptr<Param>> args) { args_ = std::move(args); }

    // Accessors
    std::shared_ptr<Identifier> getClassName() const { return className_; }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "FnCallExpr";

        if (className_ != nullptr)
            ss << " (" << className_->getValue() << "::" << token_->getValueStr() << ")";
        else
            ss << " (" << token_->getValueStr() << ")";

        return ss.str();
    }

    void print(size_t level) const override {
        Expr::print(level);
        level++;

        for (const auto &arg : args_)
            arg->print(level);
    }
};

// Function Expression node
class ClassInitExpr : public Expr {
    std::vector<std::unique_ptr<Param>> args_; // function arguments
public:
    explicit ClassInitExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }

    // Mutators
    void addArgs(std::vector<std::unique_ptr<Param>> args) { args_ = std::move(args); }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "ClassInitExpr" << " (" << token_->getValueStr() << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        Expr::print(level);
        level++;

        for (const auto &arg : args_)
            arg->print(level);
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
class PrimitiveType : public Type {
private:
    // Map token type to its string equivalent
    static inline std::unordered_map<TokenType, std::string> typeToStr = {
        {TokenType::KW_VOID, "void"},
        {TokenType::KW_INT, "int"},
        {TokenType::KW_BOOL, "bool"},
        {TokenType::KW_FLOAT, "float"},
        {TokenType::KW_STRING, "string"},
    };

public:
    explicit PrimitiveType(const std::shared_ptr<Token> &tok) { token_ = tok; }
    PrimitiveType() = default;

    const std::string kind() const override {
        std::stringstream ss;
        ss << "PrimitiveType" << " (" << typeToStr.at(token_->getType()) << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        AST::print(level);
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

    void print(size_t level) const override {
        Stmt::print(level);
        level++;

        type_->print(level); // TODO: KW_INT ==> "int"?
        name_->print(level);

        if (value_)
            value_->print(level);
    }

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

    void print(size_t level) const override {
        Stmt::print(level);
        name_->print(level);
        value_->print(level);
    }

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

    void print(size_t level) const override {
        Stmt::print(level);
        level++;
        expr_->print(level);
    }

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

    void print(size_t level) const override {
        Stmt::print(level);
        level++;

        for (const auto &var : vars_)
            var->print(level);
    }

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

    void print(size_t level) const override {
        Stmt::print(level);
        level++;

        for (const auto &expr : exprs_)
            expr->print(level);
    }

    // Mutators
    void addExpr(std::unique_ptr<Expr> expr) { exprs_.push_back(std::move(expr)); }
};

//----------------------------------------------------------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Def node
class Def : public AST {
protected:
    std::unique_ptr<Type> retType_;              // return type
    std::vector<std::unique_ptr<Param>> params_; // parameters
    std::unique_ptr<Stmt> body_;                 // body, surrounded by "{ and "}"
public:
    explicit Def(const std::shared_ptr<Token> &tok) { token_ = tok; }

    // Getters
    const std::string getName() const { return token_->getValueStr(); }

    // A bunch of mutators
    void addRetType(std::unique_ptr<Type> type) { retType_ = std::move(type); }
    void addParams(std::vector<std::unique_ptr<Param>> params) { params_ = std::move(params); }
    void addBody(std::unique_ptr<Stmt> body) { body_ = std::move(body); }

    void print(size_t level) const override {
        AST::print(level);
        level++;

        if (retType_)
            retType_->print(level);

        for (const auto &param : params_)
            param->print(level);

        if (body_)
            body_->print(level);
    }
};

// Function Definition node
class FnDef : public Def {
public:
    explicit FnDef(const std::shared_ptr<Token> &tok) : Def(tok) { token_ = tok; }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "FnDef" << " (" << Def::getName() << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        Def::print(level);
    }
};

// Constructor Definition node
class ConstructorDef : public Def {
public:
    explicit ConstructorDef(const std::shared_ptr<Token> &tok) : Def(tok) { token_ = tok; }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "ConstructorDef" << " (" << Def::getName() << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        Def::print(level);
    }
};

// Destructor Definition node
class DestructorDef : public Def {
public:
    explicit DestructorDef(const std::shared_ptr<Token> &tok) : Def(tok) { token_ = tok; }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "DestructorDef" << " (" << Def::getName() << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        Def::print(level);
    }
};

// Class Definition node
class ClassDef : public Def {
    std::unique_ptr<Def> ctor_;                 // constructor
    std::unique_ptr<Def> dtor_;                 // destructor
    std::vector<std::unique_ptr<Def>> methods_; // methods
    std::vector<std::unique_ptr<Stmt>> fields_; // fields
public:
    explicit ClassDef(const std::shared_ptr<Token> &tok) : Def(tok) { token_ = tok; }

    // A bunch of mutators
    void addConstructor(std::unique_ptr<Def> ctor) { ctor_ = std::move(ctor); }
    void addDestructor(std::unique_ptr<Def> dtor) { dtor_ = std::move(dtor); }
    void addMethod(std::unique_ptr<Def> method) { methods_.push_back(std::move(method)); }
    void addField(std::unique_ptr<Stmt> field) { fields_.push_back(std::move(field)); }

    const std::string kind() const override {
        std::stringstream ss;
        ss << "ClassDef" << " (" << Def::getName() << ")";
        return ss.str();
    }

    void print(size_t level) const override {
        Def::print(level);
        level++;

        if (ctor_)
            ctor_->print(level);

        if (dtor_)
            dtor_->print(level);

        for (const auto &method : methods_)
            method->print(level);

        for (const auto &field : fields_)
            field->print(level);
    }
};

//----------------------------------------------------------------------------------------------------------------------
// Loops
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for Loop node
class Loop : public Stmt {
protected:
    std::unique_ptr<Stmt> body_; // surrounded by "{ and "}"
public:
    void print(size_t level) const override {
        AST::print(level);
        body_->print(level);
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

    void print(size_t level) const override {
        // Print header
        printf("%s%s\n", std::string(level*2, ' ').c_str(), kind().c_str());
        level++;

        // Print the for loop insides
        cond_->print(level);
        body_->print(level);
    }

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

    void print(size_t level) const override {
        // Print header
        printf("%s%s\n", std::string(level*2, ' ').c_str(), kind().c_str());
        level++;

        // Print the for loop insides
        init_->print(level);
        cond_->print(level);
        incdec_->print(level);

        body_->print(level);
    }

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

    void print(size_t level) const override {
        // Print header
        printf("%s%s\n", std::string(level*2, ' ').c_str(), kind().c_str());
        level++;

        // Print the for loop insides
        elem_->print(level);
        iterElem_->print(level);

        body_->print(level);
    }

    // Mutators
    void addElem(std::unique_ptr<Expr> expr) { elem_ = std::move(expr); }
    void addIterElem(std::unique_ptr<Expr> expr) { iterElem_ = std::move(expr); }
};

//----------------------------------------------------------------------------------------------------------------------
// Conditional if statements
//----------------------------------------------------------------------------------------------------------------------
// An abstract definition for if statement node
class BaseIf : public Stmt {
protected:
    std::unique_ptr<Stmt> body_; // surrounded by "{ and "}"
public:
    void print(size_t level) const override {
        AST::print(level);
        body_->print(level);
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
        // Print header
        printf("%s%s\n", std::string(level*2, ' ').c_str(), kind().c_str());
        level++;

        // Print the if insides
        cond_->print(level);
        body_->print(level);

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
