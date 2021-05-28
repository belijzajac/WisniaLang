#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "AST.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  using namespace Wisnia;
  auto lexer = std::make_unique<Lexer>();
  auto root_node = std::make_unique<AST::Root>();
  
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
