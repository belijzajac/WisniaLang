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
#include <fstream>
// Wisnia
#include "AST.hpp"
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "IRGenerator.hpp"
#include "Lexer.hpp"
#include "NameResolver.hpp"
#include "Parser.hpp"

using namespace Wisnia;

class IProgramTestFixture : public testing::Test {
  struct Program {
    int m_status;
    std::string m_output;
    std::string m_error;
  };

 protected:
  void SetUp(std::string_view program) {
    std::istringstream iss{program.data()};
    auto lexer = std::make_unique<Lexer>(iss);
    auto parser = std::make_unique<Parser>(*lexer);
    auto root = parser->parse();
    root->accept(&m_resolver);
    root->accept(&m_generator);
    auto codeGenerator = std::make_unique<CodeGenerator>();
    codeGenerator->generateCode(m_generator.getUpdatedInstructions());
    auto elf = std::make_unique<ELF>(codeGenerator->getTextSection(), codeGenerator->getDataSection());
    elf->compile();
  }

  void exec(std::string_view cmd) {
    const auto outFile = makeTemporaryFilename();
    const auto errFile = makeTemporaryFilename();

    // Redirect the command's output appropriately
    std::ostringstream ss;
    ss << cmd.data() << " >" << outFile << " 2>" << errFile;

    m_program.m_status = std::system(ss.str().c_str());
    m_program.m_output = readFile(outFile);
    m_program.m_error  = readFile(errFile);

    std::remove(outFile.c_str());
    std::remove(errFile.c_str());
  }

 private:
  static std::string randomString(size_t length) {
    const auto randomChar = []() -> char {
      constexpr char charset[] =
          "0123456789"
          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "abcdefghijklmnopqrstuvwxyz";
      constexpr size_t maxIndex = (sizeof(charset) - 1);
      return charset[rand() % maxIndex];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randomChar);
    return str;
  }

  static std::string makeTemporaryFilename() {
    std::ostringstream ss;
    ss << "/tmp/" << randomString(10);
    return ss.str();
  }

  static std::string readFile(std::string_view filename) {
    std::string result{};
    std::ifstream file(filename.data());
    char c;
    while (file.get(c)) {
      result += c;
    }
    return result;
  }

 protected:
  Program m_program{};

 private:
  NameResolver m_resolver{};
  IRGenerator m_generator{};
};

#define EXPECT_PROGRAM_OUTPUT(statement, output)      \
  do {                                                \
    statement;                                        \
    EXPECT_EQ(m_program.m_status, 0);                 \
    EXPECT_STREQ(m_program.m_error.c_str(), "");      \
    EXPECT_STREQ(m_program.m_output.c_str(), output); \
  } while (0)

using ProgramTest = IProgramTestFixture;

TEST_F(ProgramTest, PrintStrings) {
  constexpr std::string_view program = R"(
  fn main () -> void {
    print "hello world\n";
    print "hahaha\n";
    print "lole\n";
  })";
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "hello world\nhahaha\nlole\n");
}

TEST_F(ProgramTest, PrintStringVariables) {
  constexpr std::string_view program = R"(
  fn main () -> void {
    string str = "ABCDE";
    print str, "123", 45;
  })";
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "ABCDE12345");
}

TEST_F(ProgramTest, PrintIntVariables) {
  constexpr std::string_view program = R"(
  fn main () -> void {
    int num1 = 123;
    int num2 = 456;
    int num3 = 789;
    int num4 = 101;
    print num1, num2;
    print num3, num4;
  })";
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "123456789101");
}

TEST_F(ProgramTest, AddVariables) {
  constexpr std::string_view program = R"(
  fn main () -> void {
    int num1  = 1;
    int num2  = 10;
    int num3  = 100;
    int num4  = 1000;
    int num5  = 10000;
    int num6  = 100000;
    int num7  = 1000000;
    int num8  = 10000000;
    int num9  = 100000000;
    int num10 = 1000000000;
    int sum = num1 + num2 + num3 + num4 + num5 + num6 + num7 + num8 + num9 + num10;
    print sum;
  })";
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "1111111111");
}
