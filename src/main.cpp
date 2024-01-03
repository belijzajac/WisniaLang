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
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"

using namespace Wisnia;
using namespace lyra;

constexpr std::string_view kLogo {
    " __        ___           _       _                           \n"
    " \\ \\      / (_)___ _ __ (_) __ _| |    __ _ _ __   __ _    \n"
    "  \\ \\ /\\ / /| / __| '_ \\| |/ _` | |   / _` | '_ \\ / _` |\n"
    "   \\ V  V / | \\__ \\ | | | | (_| | |__| (_| | | | | (_| |  \n"
    "    \\_/\\_/  |_|___/_| |_|_|\\__,_|_____\\__,_|_| |_|\\__, |\n"
    "                                                  |___/ "
};

int main(int argc, char *argv[]) {
  bool show_help{false};
  struct Config {
    std::string file;
    std::string dump;
  } config;

  auto cli = help(show_help)
    | arg(config.file, "file name")("File to compile.").required();
  cli.add_argument(
    opt(config.dump, "tokens|ast|ir|code")
      .name("-d")
      .name("--dump")
      .help("Dump information.")
      .choices("tokens", "ast", "ir", "code"));

  const auto result = cli.parse({ argc, argv });

  bool logoShown{false};
  auto showLogo = [&logoShown]() {
    if (!logoShown) {
      std::cout << fmt::format("{} v{}\n", kLogo, WISNIA_VERSION);
      logoShown = true;
    }
  };

  if (!result) {
    showLogo();
    std::cerr << result.message() << "\n\n";
  }
  if (show_help || !result) {
    showLogo();
    std::cout << cli << "\n";
    return 0;
  }
  if (!config.file.ends_with(".wsn")) {
    showLogo();
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
    SemanticAnalysis analysis;
    root->accept(analysis);
    if (config.dump == "ast") {
      root->print(std::cout);
    }
    IRGenerator generator;
    root->accept(generator);
    if (config.dump == "ir") {
      generator.printInstructionsAfterInstructionOptimization(std::cout);
    }
    CodeGenerator codeGenerator{};
    codeGenerator.generate(generator.getInstructionsAfterInstructionOptimization());
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
    std::cerr << "Caught an unknown exception\n";
    return -1;
  }
}
