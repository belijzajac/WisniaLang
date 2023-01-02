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

// Wisnia
#include "AST.hpp"
#include "Lexer.hpp"
#include "NodeCollector.hpp"
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"
#include "SemanticTestFixture.hpp"

using namespace Wisnia;
using NameResolverTest = SemanticTestFixture;

struct VarInfo {
  constexpr VarInfo(const char *n, const char *t) : m_name{n}, m_type{t} {}
  const char *m_name;
  const char *m_type;
};

TEST_F(NameResolverTest, ResolveVarInfo) {
  NodeCollector<AST::VarExpr> collector;
  SemanticAnalysis analysis;
  m_root->accept(analysis);
  m_root->accept(collector);
  const auto &collectedVars = collector.getNodes();
  EXPECT_EQ(collectedVars.size(), 44);

  constexpr std::array<VarInfo, 44> kExpectedVars{
    VarInfo{"Foo", "class"}, VarInfo{"is_fifteen", "bool"}, VarInfo{"number", "float"},
    VarInfo{"digit", "u32"}, VarInfo{"simple_operations", "void"}, VarInfo{"a", "float"},
    VarInfo{"number", "float"}, VarInfo{"i", "int"}, VarInfo{"i", "int"}, VarInfo{"i", "int"},
    VarInfo{"i", "int"}, VarInfo{"a", "float"}, VarInfo{"a", "float"}, VarInfo{"a", "float"},
    VarInfo{"is_fifteen", "bool"}, VarInfo{"digit", "u32"}, VarInfo{"a", "float"},
    VarInfo{"a", "float"}, VarInfo{"digit", "u32"}, VarInfo{"digit", "u32"}, VarInfo{"a", "float"},
    VarInfo{"digit", "u32"}, VarInfo{"output_hello", "string"}, VarInfo{"do_output", "bool"},
    VarInfo{"do_output", "bool"}, VarInfo{"main", "int"}, VarInfo{"argc", "int"},
    VarInfo{"argv", "string"}, VarInfo{"correct", "bool"}, VarInfo{"argc", "int"},
    VarInfo{"answer", "string"}, VarInfo{"output_hello", "string"}, VarInfo{"correct", "bool"},
    VarInfo{"fooPtr", "void"}, VarInfo{"Foo", "class"}, VarInfo{"correct", "bool"},
    VarInfo{"yes", "bool"}, VarInfo{"answer", "string"}, VarInfo{"output_hello", "string"},
    VarInfo{"yes", "bool"}, VarInfo{"argc", "int"}, VarInfo{"answer", "string"},
    VarInfo{"output_hello", "string"}, VarInfo{"argc", "int"}
  };

  size_t collectedIdx{0};
  for (const auto &[name, type] : kExpectedVars) {
    EXPECT_STREQ(collectedVars[collectedIdx]->getToken()->getValue<std::string>().c_str(), name);
    EXPECT_STREQ(collectedVars[collectedIdx]->getType()->getStrType().c_str(), type);
    collectedIdx++;
  }
}
