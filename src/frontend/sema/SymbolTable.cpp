#include "SymbolTable.h"
#include "AST.h"

using namespace Wisnia;
using namespace AST;

void SymbolTable::addSymbol(AST::VarExpr *var) {
  fmt::print("SymbolTable::addSymbol: {}\n", var->token_->getName());
  table->info[var->token_->getName()] = var;
}
