// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_SEMANTIC_TEST_FIXTURE_HPP
#define WISNIALANG_SEMANTIC_TEST_FIXTURE_HPP

#include <gtest/gtest.h>
// Wisnia
#include "Parser.hpp"

namespace Wisnia {
using namespace std::literals;

class SemanticTestFixture : public testing::Test {
  static constexpr auto kProgram = R"(
  class Foo {
    bool is_fifteen{false};
    float number = 5.0;
    int digit = 123456;

    fn simple_operations() {
       float a = number + 7.75;
       for (int i = 0; i < 5; i = i + 1) {
         a = a + 1.45;
         if (a == 15.0) {
           is_fifteen = true;
           break;
         }
       }
       while (digit < 150000) {
         a = a - 1.33;
         digit = digit * 2;
         print(a, digit);
       }
     }
  }

  fn output_hello(do_output: bool) -> string {
    if (do_output) { return "hello"; }
    return "bye";
  }

  fn main(argc: int, argv: string) -> int {
    bool correct = (argc == 5);
    string answer = output_hello(correct);
    void fooPtr = new Foo{};

    while (true) {
      if (correct) {
        bool yes = false;
        answer = output_hello(yes);
      }
      elif (argc < 5) {
        answer = output_hello(false);
      }
      elif (argc > 7) {
        continue;
      }
      else {
        break;
      }
    }

    return 5;
  })"sv;

 public:
  SemanticTestFixture() {
    std::istringstream iss{kProgram.data()};
    Lexer lexer{iss};
    Parser parser{lexer};
    m_root = parser.parse();
  }

 protected:
  std::unique_ptr<AST::Root> m_root;
};

}  // namespace Wisnia

#endif  // WISNIALANG_SEMANTIC_TEST_FIXTURE_HPP
