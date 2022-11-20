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
#include <filesystem>
// Wisnia
#include "Lexer.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

namespace fs = std::filesystem;

TEST(LexerTest, ReadFromFile) {
  const std::array<std::string_view, 2> filePaths {
    "../../tests/lexical-analysis/sample.wsn", // CLion
    "../tests/lexical-analysis/sample.wsn"     // build instructions from repo
  };

  const auto filePath = std::find_if(begin(filePaths), end(filePaths), [&](std::string_view path) {
    return fs::exists(fs::path{path});
  });

  if (filePath == std::end(filePaths)) {
    throw std::runtime_error{"sample.wsn wasn't found"};
  }

  Lexer lexer{*filePath};
  const auto &tokens{lexer.getTokens()};

  EXPECT_EQ(tokens.size(), 17);
  // fn main() {
  EXPECT_EQ(tokens[0]->getType(), TType::KW_FN);
  EXPECT_EQ(tokens[1]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[2]->getType(), TType::OP_PAREN_O);
  EXPECT_EQ(tokens[3]->getType(), TType::OP_PAREN_C);
  EXPECT_EQ(tokens[4]->getType(), TType::OP_BRACE_O);
  // int var = 555;
  EXPECT_EQ(tokens[5]->getType(), TType::KW_INT);
  EXPECT_EQ(tokens[6]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[7]->getType(), TType::OP_ASSN);
  EXPECT_EQ(tokens[8]->getType(), TType::LIT_INT);
  EXPECT_EQ(tokens[9]->getType(), TType::OP_SEMICOLON);
  // print(var);
  EXPECT_EQ(tokens[10]->getType(), TType::KW_PRINT);
  EXPECT_EQ(tokens[11]->getType(), TType::OP_PAREN_O);
  EXPECT_EQ(tokens[12]->getType(), TType::IDENT);
  EXPECT_EQ(tokens[13]->getType(), TType::OP_PAREN_C);
  EXPECT_EQ(tokens[14]->getType(), TType::OP_SEMICOLON);
  // }
  EXPECT_EQ(tokens[15]->getType(), TType::OP_BRACE_C);
  EXPECT_EQ(tokens[16]->getType(), TType::TOK_EOF);
}

TEST(LexerTest, ReadFromNonExistentFileShouldFail) {
  EXPECT_DEATH(
      {
        Lexer lexer{"aaa.wsn"};
      },
      "the provided input was either empty or Lexer::tokenize wasn't called");
}
