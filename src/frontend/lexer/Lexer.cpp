#include "Lexer.h"
#include "../../utilities/Exception.h"
#include <fstream>
#include <iostream>

void Lexer::tokenize(const std::string &input) {
    // Opens the `input` file and copies its content into `data`
    std::ifstream sourceFile {input};
    std::string data {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};
    sourceFile.close();

    if (data.empty())
        throw Exception{"Empty file " + input};

    // A const iterator to `data`'s content
    std::string::const_iterator lineBegin = data.begin();

    for (char & it : data) {
        std::cout << it;
    }
}
