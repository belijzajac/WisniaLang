#ifndef SEMANTICTESTFIXTURE_H
#define SEMANTICTESTFIXTURE_H

#include <gtest/gtest.h>

namespace Wisnia {

class SemanticTestFixture : public testing::Test {
  const std::string kProgram = R"(
  class Foo {
    bool is_fifteen{false};
    float number = 5.0;
    int digit = 123456;

    fn simple_operations () -> void {
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
         print a; print digit;
       }
     }
  }

  fn output_hello (do_output : bool) -> string {
    if (do_output) { return "hello"; }
    return "bye";
  }

  fn main (argc : int, argv : string) -> int {
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
  }
  )";

 public:
  SemanticTestFixture() {
    std::istringstream iss{kProgram};
    m_lexer = std::make_unique<Lexer>(iss);
    m_parser = std::make_unique<Parser>(*m_lexer);
    m_root = m_parser->parse();
  }

 protected:
  std::unique_ptr<Lexer> m_lexer;
  std::unique_ptr<Parser> m_parser;
  std::unique_ptr<AST::Root> m_root;
};

}  // namespace Wisnia

#endif  // SEMANTICTESTFIXTURE_H
