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

#include <gtest/gtest.h>
// Wisnia
#include "Lexer.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace std::literals;

TEST(TokenTest, IntegerToken) {
  constexpr auto program = R"(5)"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0]->getType(), TType::LIT_INT);
  EXPECT_EQ(tokens[0]->getValue<uint64_t>(), 5);
  EXPECT_STREQ(tokens[0]->getValueStr().c_str(), "5");
  EXPECT_EQ(tokens[1]->getType(), TType::TOK_EOF);
}

TEST(TokenTest, FloatToken) {
  constexpr auto program = R"(5.55)"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0]->getType(), TType::LIT_FLT);
  EXPECT_EQ(tokens[0]->getValue<float>(), 5.55f);
  EXPECT_STREQ(tokens[0]->getValueStr().c_str(), "5.550000");
  EXPECT_EQ(tokens[1]->getType(), TType::TOK_EOF);
}

TEST(TokenTest, IllegalIntegerToken) {
  constexpr auto program = R"(5.55)"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0]->getType(), TType::LIT_FLT);
  EXPECT_THROW({ int integer = tokens[0]->getValue<uint64_t>(); }, TokenError);
  EXPECT_EQ(tokens[1]->getType(), TType::TOK_EOF);
}

TEST(TokenTest, StringToken) {
  constexpr auto program = R"("string")"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0]->getType(), TType::LIT_STR);
  EXPECT_STREQ(tokens[0]->getValue<std::string>().c_str(), "string");
  EXPECT_STREQ(tokens[0]->getASTValueStr().c_str(), "\"string\"");
  EXPECT_STREQ(tokens[0]->getValueStr().c_str(), "string");
  EXPECT_EQ(tokens[1]->getType(), TType::TOK_EOF);
}

TEST(TokenTest, StringTokensWithEscapeSymbols) {
  constexpr auto program = R"(
    "hello\fworld"
    "hello\rworld"
    "hello\tworld"
    "hello\vworld"
    "hello\nworld"
    "hello \"world\""
  )"sv;
  std::istringstream iss{program.data()};
  Lexer lexer{iss};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 7);
  EXPECT_EQ(tokens[0]->getType(), TType::LIT_STR);
  EXPECT_EQ(tokens[0]->getValue<std::string>(), "hello\fworld");
  EXPECT_STREQ(tokens[0]->getASTValueStr().c_str(), "\"hello\\fworld\"");
  EXPECT_STREQ(tokens[0]->getValueStr().c_str(), "hello\fworld");
  EXPECT_EQ(tokens[1]->getType(), TType::LIT_STR);
  EXPECT_EQ(tokens[1]->getValue<std::string>(), "hello\rworld");
  EXPECT_STREQ(tokens[1]->getASTValueStr().c_str(), "\"hello\\rworld\"");
  EXPECT_STREQ(tokens[1]->getValueStr().c_str(), "hello\rworld");
  EXPECT_EQ(tokens[2]->getType(), TType::LIT_STR);
  EXPECT_EQ(tokens[2]->getValue<std::string>(), "hello\tworld");
  EXPECT_STREQ(tokens[2]->getASTValueStr().c_str(), "\"hello\\tworld\"");
  EXPECT_STREQ(tokens[2]->getValueStr().c_str(), "hello\tworld");
  EXPECT_EQ(tokens[3]->getType(), TType::LIT_STR);
  EXPECT_EQ(tokens[3]->getValue<std::string>(), "hello\vworld");
  EXPECT_STREQ(tokens[3]->getASTValueStr().c_str(), "\"hello\\vworld\"");
  EXPECT_STREQ(tokens[3]->getValueStr().c_str(), "hello\vworld");
  EXPECT_EQ(tokens[4]->getType(), TType::LIT_STR);
  EXPECT_EQ(tokens[4]->getValue<std::string>(), "hello\nworld");
  EXPECT_STREQ(tokens[4]->getASTValueStr().c_str(), "\"hello\\nworld\"");
  EXPECT_STREQ(tokens[4]->getValueStr().c_str(), "hello\nworld");
  EXPECT_EQ(tokens[5]->getType(), TType::LIT_STR);
  EXPECT_EQ(tokens[5]->getValue<std::string>(), "hello \"world\"");
  EXPECT_STREQ(tokens[5]->getASTValueStr().c_str(), "\"hello \\world\\\"");
  EXPECT_STREQ(tokens[5]->getValueStr().c_str(), "hello \"world\"");
  EXPECT_EQ(tokens[6]->getType(), TType::TOK_EOF);
}
