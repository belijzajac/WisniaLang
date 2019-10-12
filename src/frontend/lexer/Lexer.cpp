#include "Lexer.h"
#include "dfa/DFA.h"
#include "token/Token.h"
#include "../../utilities/Exception.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

// Determines the following state
int switchState(char currentChar) {
    // Whitespace
    if(std::isspace(currentChar)) {
        return SPACE;
    }

    // Integer
    else if(std::isdigit(currentChar)) {
        return INTEGER;
    }

    // Real number
    else if(currentChar == '.') {
        return FLOAT;
    }

    // Characters
    else if(std::isalpha(currentChar)) {
        return IDENT;
    }

    // Operators
    else if(std::ispunct(currentChar)){
        return OPERATOR;
    }

    return UNKNOWN;
}

void Lexer::tokenize(const std::string &input) {
    // Opens the `input` file and copies its content into `data`
    std::ifstream sourceFile {input};
    std::string data {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};
    sourceFile.close();

    if (data.empty())
        throw Exception{"Empty file " + input};

    char currentChar;       // the current character of the input that is to be analyzed
    int column    = MAIN;   // column of a DFA state (a.k.a. state type)
    int currState = MAIN;   // current DFA state
    int prevState = MAIN;   // previous DFA state
    std::string tokenBuff;  // buffer in which we (gradually) store the value of a token

    // A const iterator to `data`'s content
    std::string::const_iterator lineBegin = data.begin();

    // use an FSM to parse the expression
    for(std::size_t index = 0; index < data.length(); ){
        currentChar = data.at(index);

        // Column number for the current character
        column = switchState(currentChar);

        // The current transition
        currState = transitionTbl[currState][column];

        // We have successfully parsed a token.
        if(currState == MAIN){
            if(prevState != SPACE){ // we don't care about whitespaces
                TokenType tokenType {TokenType::TOK_INVALID};

                // Determine the right TokenType according to the previous DFA state
                switch (prevState) {
                    case IDENT:
                        auto search = StrToTokenKw.find(tokenBuff);
                        if (search != StrToTokenKw.end()) {
                            tokenType = search->second;
                        } else {
                            tokenType = TokenType::IDENT;
                        }
                        break;
                    //case ...:
                }

                // Add a newly found (and recognized) token
                auto pif   = std::make_unique<PositionInFile>(input, 0, 0);
                auto token = std::make_shared<Token>(tokenType, tokenBuff, std::move(pif));
                tokens_.push_back(token);
            }
            tokenBuff.clear();
        } else { // Continue parsing
            tokenBuff += currentChar;
            ++index;
        }
        prevState = currState;
    }

    // Save the last token
    if(currState != SPACE && !tokenBuff.empty()){
        auto pif   = std::make_unique<PositionInFile>(input, 0, 0);
        auto token = std::make_shared<Token>(TokenType::TOK_INVALID, tokenBuff, std::move(pif));
        tokens_.push_back(token);
    }

    // Display the tokens
    for(auto & token : tokens_){
        std::cout << token->getName() << " " << token->getValue() << "\n";
    }
}
