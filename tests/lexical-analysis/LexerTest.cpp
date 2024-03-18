// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <gtest/gtest.h>
// Wisnia
#include "Lexer.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace std::literals;

TEST(LexerTest, Identifiers) {
  constexpr auto program = R"(ab + ac;)"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 5);
  EXPECT_EQ(tokens[0]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[0]->getValue<std::string>(), "ab");
  EXPECT_EQ(tokens[1]->getType(), TType::OP_ADD);
  EXPECT_EQ(tokens[2]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[2]->getValue<std::string>(), "ac");
  EXPECT_EQ(tokens[3]->getType(), TType::OP_SEMICOLON);
  EXPECT_EQ(tokens[4]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, Operators) {
  constexpr auto program = R"(
    = == >==< <== -->- <---
    +++ /**/ ++ ++++ ; . , :
    ><=><<=> =!= () {} || /
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 37);
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
  EXPECT_EQ(tokens[35]->getType(), TType::OP_DIV);
  EXPECT_EQ(tokens[36]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, StringWithUnknownEscapeSymbolsShouldPass) {
  constexpr auto program = R"("\o/ >\\\<")"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0]->getType(), TType::LIT_STR);
  EXPECT_EQ(tokens[0]->getValue<std::string>(), "\\o/ >\\\\\\<");
  EXPECT_EQ(tokens[1]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, IllegalCharacters) {
  EXPECT_THROW(
      {
        constexpr auto program = R"(「ロリ」)"sv;
        std::istringstream iss{program.data()};
        Lexer lexer{iss};
      },
      LexerError);
}

TEST(LexerTest, Numbers) {
  constexpr auto program = R"(12345 123.45)"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 3);
  EXPECT_EQ(tokens[0]->getType(), TType::LIT_INT);
  EXPECT_EQ(tokens[0]->getValue<int>(), 12345);
  EXPECT_EQ(tokens[1]->getType(), TType::LIT_FLT);
  EXPECT_EQ(tokens[1]->getValue<float>(), 123.45f);
  EXPECT_EQ(tokens[2]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, IllegalInteger) {
  EXPECT_THROW(
      {
        constexpr auto program = R"(123a45)"sv;
        std::istringstream iss{program.data()};
        Lexer lexer{iss};
      },
      LexerError);
}

TEST(LexerTest, IllegalFloatWithExtraLetter) {
  EXPECT_THROW(
      {
        constexpr auto program = R"(123.45aa)"sv;
        std::istringstream iss{program.data()};
        Lexer lexer{iss};
      },
      LexerError);
}

TEST(LexerTest, IllegalFloatWithExtraDot) {
  EXPECT_THROW(
      {
        constexpr auto program = R"(123.45.67)"sv;
        std::istringstream iss{program.data()};
        Lexer lexer{iss};
      },
      LexerError);
}

TEST(LexerTest, LogicOperations) {
  constexpr auto program = R"(
    hello && world
    hello || world
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 7);
  EXPECT_EQ(tokens[0]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[1]->getType(), TType::OP_AND);
  EXPECT_EQ(tokens[2]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[3]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[4]->getType(), TType::OP_OR);
  EXPECT_EQ(tokens[5]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[6]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, IllegalLogicOperationAmpersand) {
  EXPECT_THROW(
      {
        constexpr auto program = R"(hello &&& world)"sv;
        std::istringstream iss{program.data()};
        Lexer lexer{iss};
      },
      LexerError);
}

TEST(LexerTest, IllegalLogicOperationTilde) {
  EXPECT_THROW(
      {
        constexpr auto program = R"(hello ||| world)"sv;
        std::istringstream iss{program.data()};
        Lexer lexer{iss};
      },
      LexerError);
}

TEST(LexerTest, CommentsSingleLine) {
  constexpr auto program = R"(
    /* comment */ int var = 5;
    var = 6; /* comment */
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 10);
  // int var = 5;
  EXPECT_EQ(tokens[0]->getType(), TType::KW_INT);
  EXPECT_EQ(tokens[1]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[2]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[3]->getType(), TType::LIT_INT);
  EXPECT_EQ(tokens[4]->getType(), TType::OP_SEMICOLON);
  // var = 6;
  EXPECT_EQ(tokens[5]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[6]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[7]->getType(), TType::LIT_INT);
  EXPECT_EQ(tokens[8]->getType(), TType::OP_SEMICOLON);
  EXPECT_EQ(tokens[9]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, CommentsMultiLine) {
  constexpr auto program = R"(
    int var = 5;
    /*
    comment that goes through multiple lines ***
    here we try to assign a new value to var ///
    */
    var = 6;
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 10);
  // int var = 5;
  EXPECT_EQ(tokens[0]->getType(), TType::KW_INT);
  EXPECT_EQ(tokens[1]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[2]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[3]->getType(), TType::LIT_INT);
  EXPECT_EQ(tokens[4]->getType(), TType::OP_SEMICOLON);
  // var = 6;
  EXPECT_EQ(tokens[5]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[6]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[7]->getType(), TType::LIT_INT);
  EXPECT_EQ(tokens[8]->getType(), TType::OP_SEMICOLON);
  EXPECT_EQ(tokens[9]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, PrettyPrinter) {
  constexpr auto program = R"(
  fn main() {
    print(123456);
  }
  )"sv;
  std::istringstream iss{program.data()};
  std::stringstream ss;
  Lexer lexer{iss};
  lexer.print(ss);

  EXPECT_STREQ(ss.str().c_str(),
    "  ID  |  LN  |      TYPE       |      VALUE      \n"
    "------+------+-----------------+-----------------\n"
    "  0   |  2   |      KW_FN      |       fn        \n"
    "  1   |  2   |      IDENT      |      main       \n"
    "  2   |  2   |   OP_PAREN_O    |        (        \n"
    "  3   |  2   |   OP_PAREN_C    |        )        \n"
    "  4   |  2   |   OP_BRACE_O    |        {        \n"
    "  5   |  3   |    KW_PRINT     |      print      \n"
    "  6   |  3   |   OP_PAREN_O    |        (        \n"
    "  7   |  3   |     LIT_INT     |     123456      \n"
    "  8   |  3   |   OP_PAREN_C    |        )        \n"
    "  9   |  3   |  OP_SEMICOLON   |        ;        \n"
    "  10  |  4   |   OP_BRACE_C    |        }        \n"
    "  11  |  6   |     TOK_EOF     |      [EOF]      \n");
}
