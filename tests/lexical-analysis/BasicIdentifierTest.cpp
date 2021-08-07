#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "Token.h"

using namespace Wisnia;
using namespace Basic;

TEST(LexerTest, BasicIdentifier) {
  std::string program = R"(ab + ac;)";
  std::istringstream iss{program};

  auto lexer = std::make_unique<Lexer>(iss);
  const auto &tokens = lexer->getTokens();

  EXPECT_GT(tokens.size(), 0);
  EXPECT_EQ(tokens.size(), 5);
  EXPECT_EQ(tokens[0]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[0]->getValue<std::string>(), "ab");
  EXPECT_EQ(tokens[1]->getType(), TType::OP_ADD);
  EXPECT_EQ(tokens[2]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[2]->getValue<std::string>(), "ac");
  EXPECT_EQ(tokens[3]->getType(), TType::OP_SEMICOLON);
  EXPECT_EQ(tokens[4]->getType(), TType::TOK_EOF);
}
