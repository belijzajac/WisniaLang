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
    const auto &root = parser->parse();

    NameResolver resolver{};
    root->accept(&resolver);

    //lexer->prettyPrint();
    //root->print();
  } catch (const WisniaError &ex) {
    std::cerr << ex.what() << "\n";
    return -1;
  } catch (...) {
    std::cerr << "Caught an undefined exception\n";
    return -1;
  }

  return 0;
}
