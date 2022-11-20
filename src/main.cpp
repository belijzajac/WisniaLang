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
#include <lyra/lyra.hpp>
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
using namespace lyra;

int main(int argc, char *argv[]) {
  bool show_help{false};
  struct Config {
    std::string file;
    std::string dump;
  } config;

  auto cli = help(show_help).description("")
        | arg(config.file, "file name")("File to compile.").required();
  cli.add_argument(
      opt(config.dump, "tokens|ast|ir|code")
          .name("-d")
          .name("--dump")
          .help("Dump information.")
          .choices("tokens", "ast", "ir", "code"));

  const auto result = cli.parse({ argc, argv });

  if (!result) {
    std::cerr << result.message() << "\n\n";
  }
  if (show_help || !result) {
    std::cout << cli << "\n";
    return 0;
  }
  if (!config.file.ends_with(".wsn")) {
    std::cout << "Expected: <file name> ending with .wsn extension\n\n";
    std::cout << cli << "\n";
    return 0;
  }

  try {
    Lexer lexer{config.file};
    Parser parser{lexer};
    const auto &root = parser.parse();
    if (config.dump == "tokens") {
      lexer.print(std::cout);
    }
    NameResolver resolver;
    root->accept(&resolver);
    if (config.dump == "ast") {
      root->print(std::cout);
    }
    IRGenerator generator;
    root->accept(&generator);
    if (config.dump == "ir") {
      generator.printInstructionsAfterInstructionSimplification();
    }
    CodeGenerator codeGenerator{};
    codeGenerator.generateCode(generator.getInstructionsAfterInstructionSimplification());
    if (config.dump == "code") {
      if (const auto &data = codeGenerator.getDataSection(); data.size() > 0) {
        std::cout << "Data section:\n";
        std::cout << data.getString() << "\n\n";
      }
      if (const auto &text = codeGenerator.getTextSection(); text.size() > 0) {
        std::cout << "Text section:\n";
        std::cout << text.getString() << "\n";
      }
    }
    ELF elf{codeGenerator.getTextSection(), codeGenerator.getDataSection()};
    elf.compile();
  } catch (const WisniaError &ex) {
    std::cerr << ex.what() << "\n";
    return -1;
  } catch (...) {
    std::cerr << "Caught an undefined exception\n";
    return -1;
  }
}
