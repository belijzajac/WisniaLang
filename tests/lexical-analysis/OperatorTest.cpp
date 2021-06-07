#include <gtest/gtest.h>
// Wisnia
#include "Lexer.h"
#include "Token.h"

using namespace Wisnia;
using namespace Basic;

TEST(LexerTest, Operators) {
  std::string program = R"(
    = == >==< <== -->- <---
    +++ /**/ ++ ++++ ; . , :
    ><=><<=> =!= () {} ||
  )";
  std::istringstream iss{program};

  auto lexer = std::make_unique<Lexer>();
  lexer->tokenize(iss);
  const auto &tokens = lexer->getTokens();

  EXPECT_GT(tokens.size(), 0);
  EXPECT_EQ(tokens.size(), 36);
  // = == >==< <== -->- <---
  EXPECT_EQ(tokens[0]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[1]->getType(), TType::OP_EQ);
  EXPECT_EQ(tokens[2]->getType(), TType::OP_GE);
  EXPECT_EQ(tokens[3]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[4]->getType(), TType::OP_L);
  EXPECT_EQ(tokens[5]->getType(), TType::OP_LE);
  EXPECT_EQ(tokens[6]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[7]->getType(), TType::OP_USUB);
  EXPECT_EQ(tokens[8]->getType(), TType::OP_G);
  EXPECT_EQ(tokens[9]->getType(), TType::OP_SUB);
  EXPECT_EQ(tokens[10]->getType(), TType::OP_L);
  EXPECT_EQ(tokens[11]->getType(), TType::OP_USUB);
  EXPECT_EQ(tokens[12]->getType(), TType::OP_SUB);
  // +++ /**/ ++ ++++ ; . , :
  EXPECT_EQ(tokens[13]->getType(), TType::OP_UADD);
  EXPECT_EQ(tokens[14]->getType(), TType::OP_ADD);
  EXPECT_EQ(tokens[15]->getType(), TType::OP_UADD);
  EXPECT_EQ(tokens[16]->getType(), TType::OP_UADD);
  EXPECT_EQ(tokens[17]->getType(), TType::OP_UADD);
  EXPECT_EQ(tokens[18]->getType(), TType::OP_SEMICOLON);
  EXPECT_EQ(tokens[19]->getType(), TType::OP_METHOD_CALL);
  EXPECT_EQ(tokens[20]->getType(), TType::OP_COMMA);
  EXPECT_EQ(tokens[21]->getType(), TType::OP_COL);
  // ><=><<=> =!= () {} ||
  EXPECT_EQ(tokens[22]->getType(), TType::OP_G);
  EXPECT_EQ(tokens[23]->getType(), TType::OP_LE);
  EXPECT_EQ(tokens[24]->getType(), TType::OP_G);
  EXPECT_EQ(tokens[25]->getType(), TType::OP_L);
  EXPECT_EQ(tokens[26]->getType(), TType::OP_LE);
  EXPECT_EQ(tokens[27]->getType(), TType::OP_G);
  EXPECT_EQ(tokens[28]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[29]->getType(), TType::OP_NE);
  EXPECT_EQ(tokens[30]->getType(), TType::OP_PAREN_O);
  EXPECT_EQ(tokens[31]->getType(), TType::OP_PAREN_C);
  EXPECT_EQ(tokens[32]->getType(), TType::OP_BRACE_O);
  EXPECT_EQ(tokens[33]->getType(), TType::OP_BRACE_C);
  EXPECT_EQ(tokens[34]->getType(), TType::OP_OR);
  EXPECT_EQ(tokens[35]->getType(), TType::TOK_EOF);
}
