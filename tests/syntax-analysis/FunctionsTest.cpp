#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

using namespace Wisnia;
using namespace Basic;

TEST(ParserTest, Functions) {
  std::string program = R"(
  fn empty () -> void {}
  fn main (int argc, string argv) -> int { return 5; }
  )";
  std::istringstream iss{program};

  auto lexer = std::make_unique<Lexer>(iss);
  auto parser = std::make_unique<Parser>(std::move(*lexer));
  const auto &root = parser->parse();

  EXPECT_EQ(root->globalFnDefs_.size(), 1);
}
