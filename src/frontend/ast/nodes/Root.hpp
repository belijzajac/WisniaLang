// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0
  
#ifndef WISNIALANG_AST_ROOT_HPP
#define WISNIALANG_AST_ROOT_HPP

#include <memory>
#include <utility>
#include <vector>
// Wisnia
#include "Visitor.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

using TokenPtr = std::shared_ptr<Basic::Token>;

class IVisitable {
 public:
  virtual void accept(Visitor &) = 0;
};

class Root : public IVisitable {
  using RootPtr = std::unique_ptr<Root>;

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

  void addGlobalClass(RootPtr klass) {
    m_globalClasses.push_back(std::move(klass));
  }

  void addGlobalFunction(RootPtr function) {
    m_globalFunctions.push_back(std::move(function));
  }

  const TokenPtr &getToken() const {
    return m_token;
  }

  const std::vector<RootPtr> &getGlobalClasses() const {
    return m_globalClasses;
  }

  const std::vector<RootPtr> &getGlobalFunctions() const {
    return m_globalFunctions;
  }

 protected:
  explicit Root(TokenPtr token)
      : m_token{std::move(token)} {}

 protected:
  TokenPtr m_token;

 private:
  std::vector<RootPtr> m_globalClasses;
  std::vector<RootPtr> m_globalFunctions;
};

}  // namespace AST
}  // namespace Wisnia

#endif  // WISNIALANG_AST_ROOT_HPP
