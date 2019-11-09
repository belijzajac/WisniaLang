#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
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
    virtual void print(size_t level) const {
        printf("%s%s\n", std::string(level*2, ' ').c_str(), kind().c_str());

        for (const auto &node : children_)
            node->print(1);
    }

    // Appends a child
    void addNode(std::unique_ptr<AST> child) { children_.push_back(std::move(child)); }
};

// Binary Expression node
class BinaryExpr : public AST {
public:
    explicit BinaryExpr(const std::shared_ptr<Token> &tok) { token_ = tok; }
    BinaryExpr() = default;

    const std::string kind() const override { return "BinaryExpression"; }
};

// Identifier node
class Identifier : public AST {
public:
    explicit Identifier(const std::shared_ptr<Token> &tok) { token_ = tok; }
    Identifier() = default;

    const std::string kind() const override { return "Identifier"; }

    void print(size_t level) const override {
        AST::print(level);
        printf("%sValue: %s\n", std::string((level + 1) * 2, ' ').c_str(), token_->getValueStr().c_str());
    }
};

// Function Definition node
class FnDef : public AST {
public:
    explicit FnDef(const std::shared_ptr<Token> &tok) { token_ = tok; }
    FnDef() = default;

    const std::string kind() const override { return "FnDef"; }

    // TODO: return *AST args, identifier, return type, etc....

    void print(size_t level) const override {
        AST::print(level);
        level++;

        // Name
        if (auto name = dynamic_cast<Identifier*>(children_.at(0).get()))
            name->print(level);
        // Parameters
        if (auto params = dynamic_cast<Identifier*>(children_.at(1).get()))
            params->print(level);
        // Return type
        if (auto returnType = dynamic_cast<Identifier*>(children_.at(2).get()))
            returnType->print(level);
        // Body
        //if (auto body = dynamic_cast<Identifier*>(children_.at(2).get()))
        //    body->print(level);
    }
};

// A single parameter
class Param : public AST {
public:
    explicit Param(const std::shared_ptr<Token> &tok) { token_ = tok; }
    Param() = default;

    const std::string kind() const override { return "Param"; }

    void print(size_t level) const override {
        AST::print(level);
        level++;

        /*if (Expression != nullptr)
            Expression->print(level);
        if (Seperator != nullptr)
            Seperator->print(level);*/

        AST::print(level);
        printf("%sValue: %s\n", std::string((level + 1) * 2, ' ').c_str(), token_->getValueStr().c_str());
    }
};

// Parameters List node
class ParamsList : public Param {
public:
    explicit ParamsList(const std::shared_ptr<Token> &tok) { token_ = tok; }
    ParamsList() = default;

    const std::string kind() const override { return "ParamsList"; }

    void print(size_t level) const override {
        AST::print(level);
        level++;

        for (const auto &param : children_)
            param->print(level);
    }
};

// Function Type node
class FnType : public AST {
public:
    explicit FnType(const std::shared_ptr<Token> &tok) { token_ = tok; }
    FnType() = default;

    const std::string kind() const override { return "FnType"; }

    void print(size_t level) const override {
        AST::print(level);
        level++;

        printf("%sValue: %s\n", std::string((level + 1) * 2, ' ').c_str(), token_->getValueStr().c_str());
    }
};

#endif // AST_H
