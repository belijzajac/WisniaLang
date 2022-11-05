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

TEST(LexerTest, BasicIdentifier) {
  constexpr auto program = R"(ab + ac;)"sv;
  std::istringstream iss{program.data()};

  auto lexer = std::make_unique<Lexer>(iss);
  auto tokens = lexer->getTokens();

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
