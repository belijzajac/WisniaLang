#include "SymbolTable.h"
#include "AST.h"

using namespace Wisnia;
using namespace AST;

void SymbolTable::addSymbol(AST::VarExpr *var) {
  table->addSymbol(var->token_->getValue<std::string>(), var); // todo: refactor
}
