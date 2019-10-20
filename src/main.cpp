#include <iostream>
#include "utilities/Exception.h"
#include "frontend/lexer/Lexer.h"

int main(int argc, char *argv[]) {
    try {
        if (argc < 2)
            throw Exception{"No arguments provided"};

        // Tokenize a single file
        auto &lexer = Lexer::get();
        lexer.tokenize(argv[1]);

        // Output the tokens in STDOUT
        lexer.prettyPrint();
        
    } catch (const Exception &ex) {
        std::cerr << ex.what();
        return -1;
    } catch (const std::exception &ex) {
        std::cerr << ex.what();
        return -1;
    } catch (...) {
        std::cerr << "Caught an undefined exception";
        return -1;
    }

    return 0;
}
