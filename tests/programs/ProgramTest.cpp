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
using namespace std::literals;

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
    codeGenerator->generateCode(m_generator.getInstructionsAfterInstructionSimplification());
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
      constexpr auto charset =
          "0123456789"
          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "abcdefghijklmnopqrstuvwxyz"sv;
      constexpr size_t maxIndex = (charset.size() - 1);
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
  constexpr auto program = R"(
  fn main() {
    print("hello world\n");
    print("hahaha\n");
    print("lole\n");
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "hello world\n"
    "hahaha\n"
    "lole\n"
  );
}

TEST_F(ProgramTest, DefaultNumberValue) {
  constexpr auto program = R"(
  fn main() {
    int var;
    print(var);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "0");
}

TEST_F(ProgramTest, DefaultStringValue) {
  constexpr auto program = R"(
  fn main() {
    string var;
    print(var);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "");
}

TEST_F(ProgramTest, DefaultBooleanValue) {
  constexpr auto program = R"(
  fn main() {
    bool var;
    print(var);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, PrintNumbers) {
  constexpr auto program = R"(
  fn main() {
    print(12345, 67890, 55555);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "123456789055555");
}

TEST_F(ProgramTest, PrintBooleans) {
  constexpr auto program = R"(
  fn main() {
    print(true, false, true);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "truefalsetrue");
}

TEST_F(ProgramTest, PrintStringVariables) {
  constexpr auto program = R"(
  fn main() {
    string str1 = "ABCDE";
    string str2 = "12345";
    string str3 = "67890";
    print(str1, str2, str3);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "ABCDE1234567890");
}

TEST_F(ProgramTest, PrintNumberVariables) {
  constexpr auto program = R"(
  fn main() {
    int num1 = 123;
    int num2 = 456;
    int num3 = 789;
    int num4 = 101;
    print(num1, num2, num3, num4);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "123456789101");
}

TEST_F(ProgramTest, PrintBooleanVariables) {
  constexpr auto program = R"(
  fn main() {
    bool aa = true;
    bool bb = false;
    bool cc = true;
    print(aa, bb, cc);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "truefalsetrue");
}

TEST_F(ProgramTest, CalculateSum) {
  constexpr auto program = R"(
  fn main() {
    int sum = 1 + 10 + 100 + 1000 + 10000 + 100000 + 1000000 + 10000000 + 100000000 + 1000000000;
    print(sum);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "1111111111");
}

TEST_F(ProgramTest, CalculateDifference) {
  constexpr auto program = R"(
  fn main() {
    int diff = 23456789 - 1 - 10 - 100 - 1000 - 10000 - 100000 - 1000000 - 10000000;
    print(diff);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "12345678");
}

TEST_F(ProgramTest, CalculateProduct) {
  constexpr auto program = R"(
  fn main() {
    int prod = 2 * 4 * 6 * 8 * 10 * 12 * 14 * 16 * 18 * 20;
    print(prod);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "3715891200");
}

TEST_F(ProgramTest, CalculateExpression) {
  constexpr auto program = R"(
  fn main() {
    int expr = ((1 + 2) * 3 + 4 * 5) - 6 * 7 + 13;
    print(expr);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "0");
}

TEST_F(ProgramTest, PrintSum) {
  constexpr auto program = R"(
  fn main() {
    print(1 + 10 + 100 + 1000 + 10000 + 100000 + 1000000 + 10000000 + 100000000 + 1000000000);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "1111111111");
}

TEST_F(ProgramTest, PrintDifference) {
  constexpr auto program = R"(
  fn main() {
    print(23456789 - 1 - 10 - 100 - 1000 - 10000 - 100000 - 1000000 - 10000000);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "12345678");
}

TEST_F(ProgramTest, PrintProduct) {
  constexpr auto program = R"(
  fn main() {
    print(2 * 4 * 6 * 8 * 10 * 12 * 14 * 16 * 18 * 20);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "3715891200");
}

TEST_F(ProgramTest, PrintExpression) {
  constexpr auto program = R"(
  fn main() {
    print(((1 + 2) * 3 + 4 * 5) - 6 * 7 + 13);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "0");
}

TEST_F(ProgramTest, AddVariables) {
  constexpr auto program = R"(
  fn main() {
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
    print(sum);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "1111111111");
}

TEST_F(ProgramTest, SubtractVariables) {
  constexpr auto program = R"(
  fn main() {
    int num1 = 1;
    int num2 = 10;
    int num3 = 100;
    int num4 = 1000;
    int num5 = 10000;
    int num6 = 100000;
    int num7 = 1000000;
    int num8 = 10000000;
    int num9 = 23456789;
    int diff = num9 - num1 - num2 - num3 - num4 - num5 - num6 - num7 - num8;
    print(diff);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "12345678");
}

TEST_F(ProgramTest, MultiplyVariables) {
  constexpr auto program = R"(
  fn main() {
    int num1  = 2;
    int num2  = 4;
    int num3  = 6;
    int num4  = 8;
    int num5  = 10;
    int num6  = 12;
    int num7  = 14;
    int num8  = 16;
    int num9  = 18;
    int num10 = 20;
    int prod = num1 * num2 * num3 * num4 * num5 * num6 * num7 * num8 * num9 * num10;
    print(prod);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "3715891200");
}

TEST_F(ProgramTest, AssignVariables) {
  constexpr auto program = R"(
  fn main() {
    int num1 = 1;
    num1 = 2 * num1;
    int num2 = 3;
    num2 = num1 * num2 + 2 * num1;
    print(num2);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "10");
}

TEST_F(ProgramTest, CallFunction) {
  constexpr auto program = R"(
  fn foo() {
    print("inside foo\n");
  }
  fn main() {
    print("before foo\n");
    foo();
    print("after foo\n");
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "before foo\n"
    "inside foo\n"
    "after foo\n"
  );
}

TEST_F(ProgramTest, CallFunctionWithArguments) {
  constexpr auto program = R"(
  fn foo(value_1: int, value_2: int, value_3: int) {
    print("inside foo 1\n");
    print(value_1, value_2, value_3);
    print("inside foo 2\n");
  }
  fn main() {
    print("before foo\n");
    foo(123, 456, 789);
    print("after foo\n");
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "before foo\n"
    "inside foo 1\n"
    "123456789"
    "inside foo 2\n"
    "after foo\n"
  );
}

TEST_F(ProgramTest, CallFunctionInsideAnotherWithArguments) {
  constexpr auto program = R"(
  fn bar(value_1: string) {
    print("inside bar 1\n");
    print(value_1);
    print("inside bar 2\n");
  }
  fn foo(value_1: int, value_2: int, value_3: int) {
    print("inside foo 1\n");
    print(value_1, value_2, value_3);
    bar("bar");
    print("inside foo 2\n");
  }
  fn main() {
    print("before foo\n");
    foo(123, 456, 789);
    print("after foo\n");
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "before foo\n"
    "inside foo 1\n"
    "123456789"
    "inside bar 1\n"
    "bar"
    "inside bar 2\n"
    "inside foo 2\n"
    "after foo\n"
  );
}

TEST_F(ProgramTest, CallFunctionShouldNotOverrideVariables) {
  constexpr auto program = R"(
  fn foo(value_1: int, value_2: int, value_3: int) {
    int a = 1;
    int b = 2;
    int c = 3;
    print(a + b + c + value_1 + value_2 + value_3);
    value_1 = 2;
    value_2 = 20;
    value_3 = 200;
  }
  fn main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    foo(a, b, c);
    print(a + b + c + d);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "12"
    "10"
  );
}

TEST_F(ProgramTest, FunctionReturnNumber) {
  constexpr auto program = R"(
  fn foo() -> int {
    return 5;
  }
  fn main() {
    int result = foo();
    print(result);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "5");
}

TEST_F(ProgramTest, FunctionReturnBoolean) {
  constexpr auto program = R"(
  fn foo() -> bool {
    return true;
  }
  fn main() {
    bool result = foo();
    print(result);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, FunctionReturnVariable) {
  constexpr auto program = R"(
  fn foo() -> int {
    int var = 5;
    return var;
  }
  fn main() {
    int result = foo();
    print(result);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "5");
}

TEST_F(ProgramTest, FunctionReturnNumberExpression) {
  constexpr auto program = R"(
  fn foo() -> int {
    return 10 - 2 * 3;
  }
  fn main() {
    int result = foo();
    print(result);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "4");
}

TEST_F(ProgramTest, FunctionReturnVariableExpression) {
  constexpr auto program = R"(
  fn foo() -> int {
    int var = 5;
    return var + 10;
  }
  fn main() {
    int result = foo();
    print(result);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "15");
}

TEST_F(ProgramTest, FunctionReturnVariableWithArgumentExpression) {
  constexpr auto program = R"(
  fn foo(value_1: int, value_2: int) -> int {
    return value_1 + value_2;
  }
  fn main() {
    int var = 6;
    int result = foo(var + 6, 6);
    print(result);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "18");
}

TEST_F(ProgramTest, PrintFunctionReturnNumber) {
  constexpr auto program = R"(
  fn foo() -> int {
    return 5;
  }
  fn main() {
    print(foo());
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "5");
}

TEST_F(ProgramTest, PrintFunctionReturnBoolean) {
  constexpr auto program = R"(
  fn foo() -> bool {
    return true;
  }
  fn main() {
    print(foo());
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, PrintFunctionReturnVariable) {
  constexpr auto program = R"(
  fn foo() -> int {
    int var = 5;
    return var;
  }
  fn main() {
    print(foo());
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "5");
}

TEST_F(ProgramTest, PrintFunctionReturnNumberExpression) {
  constexpr auto program = R"(
  fn foo() -> int {
    return 10 - 2 * 3;
  }
  fn main() {
    print(foo());
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "4");
}

TEST_F(ProgramTest, PrintFunctionReturnVariableExpression) {
  constexpr auto program = R"(
  fn foo() -> int {
    int var = 5;
    return var + 10;
  }
  fn main() {
    print(foo());
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "15");
}

TEST_F(ProgramTest, PrintFunctionReturnVariableWithArgumentExpression) {
  constexpr auto program = R"(
  fn foo(value_1: int, value_2: int) -> int {
    return value_1 + value_2;
  }
  fn main() {
    int var = 6;
    print(foo(var + 6, 6));
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "18");
}

TEST_F(ProgramTest, ConditionalNumberTrue) {
  constexpr auto program = R"(
  fn main() {
    int value = 5;
    if (value) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, ConditionalNumberFalse) {
  constexpr auto program = R"(
  fn main() {
    int value = 0;
    if (value) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, ConditionalInsideFunctionNumber) {
  constexpr auto program = R"(
  fn foo(base: int, number: int) {
    if (number) {
      print(base * number, " ");
      foo(base, number - 1);
    }
  }
  fn main() {
    foo(3, 4);
    print("1\n");
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "12 9 6 3 1\n");
}
