// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#include <gtest/gtest.h>
#include <fstream>
// Wisnia
#include "AST.hpp"
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "IRGenerator.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"

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
    Lexer lexer{iss};
    Parser parser{lexer};
    const auto &root = parser.parse();
    root->accept(m_analysis);
    root->accept(m_generator);
    CodeGenerator codeGenerator{};
    codeGenerator.generate(m_generator.getInstructions(IRGenerator::Transformation::INSTRUCTION_OPTIMIZATION));
    ELF elf{codeGenerator.getTextSection(), codeGenerator.getDataSection()};
    elf.writeELF();
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
  static std::string generateFilename() {
    constexpr auto length = 10;
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
    ss << "/tmp/" << generateFilename();
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
  SemanticAnalysis m_analysis{};
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

// ----------------------------------------------------
// Default values
// ----------------------------------------------------

TEST_F(ProgramTest, DefaultIntValue) {
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

// ----------------------------------------------------
// Print values
// ----------------------------------------------------

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

TEST_F(ProgramTest, PrintMaxInt) {
  constexpr auto program = R"(
  fn main() {
    int max = 2147483647; // mov rax, 0x7fffffff --> 48 c7 c0 | ff ff ff 7f
    print(max);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "2147483647");
}

// ----------------------------------------------------
// Exhaust registers
// ----------------------------------------------------

TEST_F(ProgramTest, ExhaustRegistersForMovInt) {
  // trying to exhaust all 15 registers (rax - r15)
  constexpr auto program = R"(
  fn main() {
    int num1  = 2147483647;
    int num2  = 1073741823;
    int num3  = 536870911;
    int num4  = 268435455;
    int num5  = 134217727;
    int num6  = 67108863;
    int num7  = 33554431;
    int num8  = 16777215;
    int num9  = 8388607;
    int num10 = 4194303;
    int num11 = 2097151;
    int num12 = 1048575;
    int num13 = 524287;
    int num14 = 262143;
    int num15 = 131071;
    print(num1, num2, num3, num4, num5, num6, num7, num8, num9, num10, num11, num12, num13, num14, num15);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "2147483647" "1073741823" "536870911" "268435455" "134217727" "67108863" "33554431"
    "16777215" "8388607" "4194303" "2097151" "1048575" "524287" "262143" "131071"
  );
}

TEST_F(ProgramTest, ExhaustRegistersForMovBoolean) {
  // trying to exhaust all 15 registers (rax - r15)
  constexpr auto program = R"(
  fn main() {
    bool num1  = true;
    bool num2  = false;
    bool num3  = true;
    bool num4  = false;
    bool num5  = true;
    bool num6  = false;
    bool num7  = true;
    bool num8  = false;
    bool num9  = true;
    bool num10 = false;
    bool num11 = true;
    bool num12 = false;
    bool num13 = true;
    bool num14 = false;
    bool num15 = true;
    print(num1, num2, num3, num4, num5, num6, num7, num8, num9, num10, num11, num12, num13, num14, num15);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "true" "false" "true" "false" "true" "false" "true"
    "false" "true" "false" "true" "false" "true" "false" "true"
  );
}

TEST_F(ProgramTest, ExhaustRegistersForAddInt) {
  // trying to exhaust all 15 registers (rax - r15)
  constexpr auto program = R"(
  fn main() {
    int a = a + 143165576; print(a);
    a =     a + 143165576; print(a);
    a =     a + 143165576; print(a);
    a =     a + 143165576; print(a);
    a =     a + 143165576; print(a);
    a =     a + 143165576; print(a);
    a =     a + 143165576; print(a);
    a =     a + 143165576; print(a);
    a =     a + 143165577; print(a);
    a =     a + 143165577; print(a);
    a =     a + 143165577; print(a);
    a =     a + 143165577; print(a);
    a =     a + 143165577; print(a);
    a =     a + 143165577; print(a);
    a =     a + 143165577; print(a);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "143165576" "286331152" "429496728" "572662304" "715827880" "858993456" "1002159032"
    "1145324608" "1288490185" "1431655762" "1574821339" "1717986916" "1861152493" "2004318070" "2147483647"
  );
}

TEST_F(ProgramTest, ExhaustRegistersForSubInt) {
  // trying to exhaust all 14 registers (rcx - r15)
  constexpr auto program = R"(
  fn main() {
    int a = 2147483647;
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
    a = a - 153391689; print(a);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "1994091958" "1840700269" "1687308580" "1533916891" "1380525202" "1227133513" "1073741824"
    "920350135" "766958446" "613566757" "460175068" "306783379" "153391690" "1"
  );
}

TEST_F(ProgramTest, ExhaustRegistersForMulInt) {
  // trying to exhaust all 14 registers (rcx - r15)
  constexpr auto program = R"(
  fn main() {
    int a = 7;
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
    int a = a * 4; print(a);
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(
    exec("./a.out"),
    "28" "112" "448" "1792" "7168" "28672" "114688"
    "458752" "1835008" "7340032" "29360128" "117440512" "469762048" "1879048192"
  );
}

// ----------------------------------------------------
// Calculate expressions
// ----------------------------------------------------

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

// ----------------------------------------------------
// Variable assignment
// ----------------------------------------------------

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

// ----------------------------------------------------
// Function call
// ----------------------------------------------------

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

// ----------------------------------------------------
// Function return
// ----------------------------------------------------

TEST_F(ProgramTest, FunctionReturnInt) {
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

TEST_F(ProgramTest, FunctionReturnIntExpression) {
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

TEST_F(ProgramTest, PrintFunctionReturnInt) {
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

TEST_F(ProgramTest, PrintFunctionReturnIntExpression) {
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

// ----------------------------------------------------
// Conditional expressions
// ----------------------------------------------------

TEST_F(ProgramTest, ConditionalIntTrue) {
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

TEST_F(ProgramTest, ConditionalIntFalse) {
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

TEST_F(ProgramTest, ConditionalIntGreaterThanTrue) {
  constexpr auto program = R"(
  fn main() {
    int value = 7;
    if (value > 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, ConditionalIntGreaterThanFalse) {
  constexpr auto program = R"(
  fn main() {
    int value = 6;
    if (value > 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, ConditionalIntGreaterThanOrEqualTrue) {
  constexpr auto program = R"(
  fn main() {
    int value = 6;
    if (value >= 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, ConditionalIntGreaterThanOrEqualFalse) {
  constexpr auto program = R"(
  fn main() {
    int value = 5;
    if (value >= 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, ConditionalIntLessTrue) {
  constexpr auto program = R"(
  fn main() {
    int value = 5;
    if (value < 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, ConditionalIntLessFalse) {
  constexpr auto program = R"(
  fn main() {
    int value = 7;
    if (value < 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, ConditionalIntLessOrEqualTrue) {
  constexpr auto program = R"(
  fn main() {
    int value = 6;
    if (value <= 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, ConditionalIntLessOrEqualFalse) {
  constexpr auto program = R"(
  fn main() {
    int value = 10;
    if (value <= 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, ConditionalIntEqualTrue) {
  constexpr auto program = R"(
  fn main() {
    int value = 6;
    if (value == 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, ConditionalIntEqualFalse) {
  constexpr auto program = R"(
  fn main() {
    int value = 7;
    if (value == 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, ConditionalIntNotEqualTrue) {
  constexpr auto program = R"(
  fn main() {
    int value = 5;
    if (value != 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "true");
}

TEST_F(ProgramTest, ConditionalIntNotEqualFalse) {
  constexpr auto program = R"(
  fn main() {
    int value = 6;
    if (value != 6) {
      print("true");
    } else {
      print("false");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "false");
}

TEST_F(ProgramTest, ConditionalBooleanFollowup) {
  constexpr auto program = R"(
  fn main() {
    bool value1 = true;
    bool value2 = true;
    bool value3 = true;
    if (value1) {
      print("true");
    }
    if (value2) {
      print("true");
    }
    if (value3) {
      print("true");
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "truetruetrue");
}

TEST_F(ProgramTest, ConditionalBooleanNested) {
  constexpr auto program = R"(
  fn main() {
    bool value = true;
    if (value) {
      print("true");
      if (value) {
        print("true");
        if (value) {
          print("true");
        }
      }
    }
  })"sv;
  SetUp(program);
  EXPECT_PROGRAM_OUTPUT(exec("./a.out"), "truetruetrue");
}
