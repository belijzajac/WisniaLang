// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0
  
#ifndef WISNIALANG_AST_ROOT_HPP
#define WISNIALANG_AST_ROOT_HPP

#include <memory>
#include <vector>
// Wisnia
#include "Visitor.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

class IVisitable {
 public:
  virtual void accept(Visitor &) = 0;
};

class Root : public IVisitable {
 public:
  Root() = default;
  virtual ~Root() = default;

  void accept(Visitor &v) override {
    v.visit(*this);
  }

  virtual std::string kind() const {
    return "Root";
  }

  virtual void print(std::ostream &output, size_t level = 0) const {
    output << std::string(level * 2, ' ') << kind() << "\n"; level++;
    for (const auto &klass : m_globalClasses) {
      klass->print(output, level);
    }
    for (const auto &function : m_globalFunctions) {
      function->print(output, level);
    }
  }

  void addGlobalClassDef(std::unique_ptr<Root> classDef) {
    m_globalClasses.push_back(std::move(classDef));
  }

  void addGlobalFnDef(std::unique_ptr<Root> fnDef) {
    m_globalFunctions.push_back(std::move(fnDef));
  }

  const std::shared_ptr<Basic::Token> &getToken() const {
    return m_token;
  }

  const std::vector<std::unique_ptr<Root>> &getGlobalClasses() const {
    return m_globalClasses;
  }

  const std::vector<std::unique_ptr<Root>> &getGlobalFunctions() const {
    return m_globalFunctions;
  }

 protected:
  explicit Root(const std::shared_ptr<Basic::Token> &tok)
      : m_token{tok} {}

 protected:
  std::shared_ptr<Basic::Token> m_token;

 private:
  std::vector<std::unique_ptr<Root>> m_globalClasses;
  std::vector<std::unique_ptr<Root>> m_globalFunctions;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_ROOT_HPP
