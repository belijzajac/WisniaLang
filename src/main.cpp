#include <iostream>
#include "utilities/Exception.h"
#include "frontend/lexer/Lexer.h"
#include "frontend/parser/Parser.h"

int main(int argc, char *argv[]) {
    try {
        if (argc < 2)
            throw Exception{"No arguments provided"};

        // Tokenize a single file
        auto &lexer = Lexer::get();
        lexer.tokenize(argv[1]);

        // Output the tokens in STDOUT
        //lexer.prettyPrint();

        // Pass tokens to the parser
        auto parser = std::make_unique<Parser>(lexer);
        
    } catch (const Exception &ex) {
        std::cerr << ex.what() << "\n";
        return -1;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
        return -1;
    } catch (...) {
        std::cerr << "Caught an undefined exception\n";
        return -1;
    }

    return 0;
}
