// Wisnia
#include "Lexer.hpp"
#include "Parser.hpp"
#include "AST.hpp"
#include "NodeCollector.hpp"
#include "NameResolver.hpp"
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
  NameResolver resolver;
  m_root->accept(&resolver);
  m_root->accept(&collector);
  auto collectedVars = collector.getNodes();
  EXPECT_EQ(collectedVars.size(), 44);

  constexpr std::array<VarInfo, 44> kExpectedVars{
      VarInfo{"Foo", "class"}, VarInfo{"is_fifteen", "bool"}, VarInfo{"number", "float"},
      VarInfo{"digit", "int"}, VarInfo{"simple_operations", "void"}, VarInfo{"a", "float"},
      VarInfo{"number", "float"}, VarInfo{"i", "int"}, VarInfo{"i", "int"}, VarInfo{"i", "int"},
      VarInfo{"i", "int"}, VarInfo{"a", "float"}, VarInfo{"a", "float"}, VarInfo{"a", "float"},
      VarInfo{"is_fifteen", "bool"}, VarInfo{"digit", "int"}, VarInfo{"a", "float"},
      VarInfo{"a", "float"}, VarInfo{"digit", "int"}, VarInfo{"digit", "int"}, VarInfo{"a", "float"},
      VarInfo{"digit", "int"}, VarInfo{"output_hello", "string"}, VarInfo{"do_output", "bool"},
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
