/***

  WisniaLang - A Compiler for an Experimental Programming Language
  Copyright (C) 2022 Tautvydas Povilaitis (belijzajac) and contributors

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

***/
  
#ifndef WISNIALANG_AST_CONDITIONALS_HPP
#define WISNIALANG_AST_CONDITIONALS_HPP

// Wisnia
#include "Root.hpp"
#include "Statements.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}  // namespace Basic

namespace AST {

class BaseIf : public BaseStmt {
 public:
  void accept(Visitor *v) override = 0;

  void print(std::ostream &output, size_t level) const override {
    BaseStmt::print(output, level);
  }

  void addBody(std::unique_ptr<BaseStmt> body) {
    m_body = std::move(body);
  }

  const std::unique_ptr<BaseStmt> &getBody() const {
    return m_body;
  }

 protected:
  explicit BaseIf(const std::shared_ptr<Basic::Token> &tok)
      : BaseStmt(tok) {}

 protected:
  std::unique_ptr<BaseStmt> m_body;
};

class IfStmt : public BaseIf {
 public:
  explicit IfStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseIf(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "IfStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseIf::print(output, level++);
    m_condition->print(output, level);
    m_body->print(output, level);
    level--; // reset for else statements
    for (const auto &stmt : m_elseStmts) {
      stmt->print(output, level);
    }
  }

  void addCondition(std::unique_ptr<BaseExpr> expr) {
    m_condition = std::move(expr);
  }

  void addElseBlocks(std::vector<std::unique_ptr<BaseIf>> expr) {
    m_elseStmts = std::move(expr);
  }

  const std::unique_ptr<BaseExpr> &getCondition() const {
    return m_condition;
  }

  const std::vector<std::unique_ptr<BaseIf>> &getElseStatements() const {
    return m_elseStmts;
  }

 private:
  std::unique_ptr<BaseExpr> m_condition;
  std::vector<std::unique_ptr<BaseIf>> m_elseStmts;
};

class ElseStmt : public BaseIf {
 public:
  explicit ElseStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseIf(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ElseStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseIf::print(output, level++);
    m_body->print(output, level);
  }
};

class ElseIfStmt : public BaseIf {
 public:
  explicit ElseIfStmt(const std::shared_ptr<Basic::Token> &tok)
      : BaseIf(tok) {}

  void accept(Visitor *v) override {
    v->visit(this);
  }

  std::string kind() const override {
    return "ElseIfStmt";
  }

  void print(std::ostream &output, size_t level) const override {
    BaseIf::print(output, level++);
    m_condition->print(output, level);
    m_body->print(output, level);
  }

  void addCondition(std::unique_ptr<BaseExpr> expr) {
    m_condition = std::move(expr);
  }

  const std::unique_ptr<BaseExpr> &getCondition() const {
    return m_condition;
  }

 private:
  std::unique_ptr<BaseExpr> m_condition;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_CONDITIONALS_HPP
