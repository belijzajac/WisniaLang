#ifndef AST_ROOT_H
#define AST_ROOT_H

#include <memory>
#include <vector>

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {
// Root node
class Root {
 protected:
  std::vector<std::unique_ptr<Root>> children_;  // children nodes
  std::unique_ptr<Root> parent_;                 // parent node
  std::shared_ptr<Basic::Token> token_;          // token (for holding names, etc.)
 private:
  std::vector<std::unique_ptr<Root>> globalClassDefs_;  // global class definitions
  std::vector<std::unique_ptr<Root>> globalFnDefs_;     // global function definitions
 public:
  Root() = default;
  virtual ~Root() = default;

  // Returns what kind of node it is
  virtual std::string kind() const { return "Root"; }

  // Outputs tree content
  virtual void print(size_t level = 0) const {
    printf("%s%s\n", std::string(level * 2, ' ').c_str(), kind().c_str());
    level++;

    for (const auto &globalClass : globalClassDefs_)
      globalClass->print(level);
    for (const auto &globalFn : globalFnDefs_)
      globalFn->print(level);
  }

  void addGlobalClassDef(std::unique_ptr<Root> classDef) {
    globalClassDefs_.push_back(std::move(classDef));
  }

  void addGlobalFnDef(std::unique_ptr<Root> fnDef) {
    globalFnDefs_.push_back(std::move(fnDef));
  }

  // Appends a child
  void addNode(std::unique_ptr<Root> child) {
    children_.push_back(std::move(child));
  }

  // Access children
  Root *first() const { return children_.at(0).get(); }
  Root *second() const { return children_.at(1).get(); }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_ROOT_H
