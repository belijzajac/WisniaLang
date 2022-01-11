#include "SymbolTable.h"
#include "AST.h"

using namespace Wisnia;
using namespace AST;

void SymbolTable::addSymbol(AST::VarExpr *var) {
  m_table->addSymbol(var->m_token->getValue<std::string>(), var); // todo: refactor
}
