#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

using namespace Wisnia;
using namespace Basic;

TEST(ParserTest, Loops) {
  std::string program = R"(
  fn loops () -> void {
    while (i < 5) {}
    for (int i = 0; i < 5; i = i + 1) {}
    for_each (elem in elems) {}
  }
  )";
  std::istringstream iss{program};

  auto lexer = std::make_unique<Lexer>(iss);
  auto parser = std::make_unique<Parser>(std::move(*lexer));
  const auto &root = parser->parse();
}
