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

#include <iostream>
// Wisnia
#include "AST.hpp"
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "Exceptions.hpp"
#include "IRGenerator.hpp"
#include "Lexer.hpp"
#include "NameResolver.hpp"
#include "Parser.hpp"

using namespace Wisnia;

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
    NameResolver resolver;
    root->accept(&resolver);
    fmt::print("<~~~ {} ~~~>\n", "updated ast tree");
    root->print();
    //...
    fmt::print("<~~~ {} ~~~>\n", "ir instructions");
    IRGenerator generator;
    root->accept(&generator);
    generator.printInstructions();
    //...
    fmt::print("<~~~ {} ~~~>\n", "updated ir instructions");
    generator.printUpdatedInstructions();
    //...
    auto codeGenerator = std::make_unique<CodeGenerator>();
    codeGenerator->generateCode(generator.getInstructions());
    if (const auto &data = codeGenerator->getDataSection(); data.size() > 0) std::cout << "<~~~ data section ~~~>\n" << data.getString() << "\n";
    if (const auto &text = codeGenerator->getTextSection(); text.size() > 0) std::cout << "<~~~ text section ~~~>\n" << text.getString() << "\n";
    //...
    auto elf = std::make_unique<ELF>(codeGenerator->getTextSection(), codeGenerator->getDataSection());
    elf->compile();
  } catch (const WisniaError &ex) {
    std::cerr << ex.what() << "\n";
    return -1;
  } catch (...) {
    std::cerr << "Caught an undefined exception\n";
    return -1;
  }
}
