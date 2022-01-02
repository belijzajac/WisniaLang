#include <iostream>
// Wisnia
#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "NameResolver.h"
#include "Exceptions.h"

using namespace Wisnia;
using namespace Utils;

int main(int argc, char *argv[]) {
  try {
    if (argc < 2) throw WisniaError{"No arguments provided"};

    auto lexer = std::make_unique<Lexer>(argv[1]);
    auto parser = std::make_unique<Parser>(*lexer);
    auto root = parser->parse();
    //...
    fmt::print("<~~~ {} ~~~>\n", "token stream");
    lexer->prettyPrint();
    fmt::print("<~~~ {} ~~~>\n", "ast tree");
    root->print();
    //...
    NameResolver resolver{};
    root->accept(&resolver);
    fmt::print("<~~~ {} ~~~>\n", "updated ast tree");
    root->print();
  } catch (const WisniaError &ex) {
    std::cerr << ex.what() << "\n";
    return -1;
  } catch (...) {
    std::cerr << "Caught an undefined exception\n";
    return -1;
  }

  return 0;
}
