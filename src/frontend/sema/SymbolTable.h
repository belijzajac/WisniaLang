#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include <fmt/format.h>
#include "Exceptions.h"

namespace Wisnia {
namespace AST {
class VarExpr;
}  // namespace AST

class SymbolTable {
  // internal struct
  struct ScopedSymbolTable {
    std::unordered_map<std::string, const AST::VarExpr *> info{};
    std::unique_ptr<ScopedSymbolTable> parentScope{};

    void addSymbol(const std::string &name, const AST::VarExpr *var) {
      info[name] = var;
    }

    const AST::VarExpr *findSymbol(const std::string &name) const {
      if (auto search = info.find(name); search != info.end())
        return search->second;
      if (parentScope) {
        return parentScope->findSymbol(name);
      }
      throw Utils::SemanticError{"No variable named " + name};
    }
  };

 public:
  SymbolTable() { table = std::make_unique<ScopedSymbolTable>(); };
  ~SymbolTable() = default;

  void addSymbol(AST::VarExpr *var);

  const AST::VarExpr *findSymbol(const std::string &name) const {
    return table->findSymbol(name);
  }

  void pushScope() {
    auto tmp = std::make_unique<ScopedSymbolTable>();
    tmp->parentScope = std::move(table);
    table = std::move(tmp);
  }

  void popScope() {
    table = std::move(table->parentScope);
  }

 private:
  std::unique_ptr<ScopedSymbolTable> table{};
};

}  // namespace Wisnia

#endif  // SYMBOLTABLE_H
