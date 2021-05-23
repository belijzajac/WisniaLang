#include <iostream>
// Wisnia
#include "Lexer.h"
#include "Parser.h"
#include "Exceptions.h"

using namespace Wisnia;
using namespace Utils;

int main(int argc, char *argv[]) {
  try {
    if (argc < 2) throw WisniaError{"No arguments provided"};

    // Tokenize a single file
    auto lexer = std::make_unique<Lexer>();
    lexer->tokenize(argv[1]);

    // Output the tokens to STDOUT
    // lexer->prettyPrint();

    // Pass tokens to the parser
    auto parser = std::make_unique<Parser>(std::move(*lexer));

  } catch (const WisniaError &ex) {
    std::cerr << ex.what() << "\n";
    return -1;
  } catch (...) {
    std::cerr << "Caught an undefined exception\n";
    return -1;
  }

  return 0;
}
