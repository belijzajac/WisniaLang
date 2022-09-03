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
#include "AST.hpp"
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "IRGenerator.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

using namespace Wisnia;
using namespace Basic;

class IProgramTestFixture : public testing::Test {
 protected:
  void SetUp(std::string_view program) {
    std::istringstream iss{program.data()};
    auto lexer = std::make_unique<Lexer>(iss);
    auto parser = std::make_unique<Parser>(*lexer);
    auto root = parser->parse();
    root->accept(&generator);
    auto codeGenerator = std::make_unique<CodeGenerator>();
    codeGenerator->generateCode(generator.getInstructions());
    auto elf = std::make_unique<ELF>(codeGenerator->getTextSection(), codeGenerator->getDataSection());
    elf->compile();
  }

  static std::string exec(std::string_view cmd) {
    std::array<char, 128> buffer{};
    std::string result{};
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.data(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
    }
    return result;
  }

 private:
  IRGenerator generator{};
};

using ProgramTest = IProgramTestFixture;

TEST_F(ProgramTest, WriteOutput) {
  constexpr std::string_view program = R"(
  fn main () -> void {
    print "hello world\n";
    print "hahaha\n";
    print "lole\n";
  })";
  SetUp(program);
  auto result{exec("./a.out")};
  EXPECT_STREQ(result.c_str(), "hello world\nhahaha\nlole\n");
}
