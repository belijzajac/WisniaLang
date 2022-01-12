#ifndef AST_ROOT_H
#define AST_ROOT_H

#include <memory>
#include <vector>
#include <fmt/format.h>
// Wisnia
#include "Visitor.hpp"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {

class IVisitable {
 public:
  virtual void accept(Visitor *v) = 0;
};

class Root : public IVisitable {
 public:
  virtual ~Root() = default;

  void accept(Visitor *v) override {
    v->visit(this);
  }

  virtual std::string kind() const { return "Root"; }

  virtual void print(size_t level = 0) const {
    fmt::print("{:>{}}{}\n", "", level * 2, kind()); level++;
    for (const auto &klass : m_globalClasses)
      klass->print(level);
    for (const auto &function : m_globalFunctions)
      function->print(level);
  }

  void addGlobalClassDef(std::unique_ptr<Root> classDef) {
    m_globalClasses.push_back(std::move(classDef));
  }

  void addGlobalFnDef(std::unique_ptr<Root> fnDef) {
    m_globalFunctions.push_back(std::move(fnDef));
  }

 public:
  std::shared_ptr<Basic::Token> m_token;                // token (for holding names, etc.)
  std::vector<std::unique_ptr<Root>> m_globalClasses;   // global class definitions
  std::vector<std::unique_ptr<Root>> m_globalFunctions; // global function definitions
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_ROOT_H
