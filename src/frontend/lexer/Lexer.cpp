#include "Lexer.h"
#include "dfa/DFA.h"
#include "token/Token.h"
#include "../../utilities/Exception.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

// Searches for string `val` in the provided map, which represents a string-to-TokenType relationship
auto findInMap = [](const std::unordered_map<std::string, TokenType> &map, const std::string &val) -> bool {
    auto found {map.find(val)};
    return found != map.end();
};

// Determines the following state
int switchState(const std::string::const_iterator &it, int prevState) {
    // Whitespace
    // TODO: \b, \t, ...
    if(std::isspace(*it)) {
        return SPACE;
    }

    // Integer
    else if(std::isdigit(*it) || (*it == '-' && std::isdigit(*(it+1)))) {
        return INT;
    }

    // Real number
    else if((*it == '.' && std::isdigit(*(it+1))) || (*it == '-' && std::isdigit(*(it+1)))) {
        return FLT;
    }

    // Characters
    else if(std::isalpha(*it)) {
        return IDENT;
    }

    // TODO: lambda + std::any_of
    // Operators
    else if(std::ispunct(*it)) {
        // 1) Logical AND (&&)
        if (prevState == OP_L_AND_ST) {
            if (*it == '&')
                return OP_L_AND;
            else
                return OP_SINGLE;
        }
        else if (*it == '&')
            return OP_L_AND_ST;
        // end of Logical AND (&&)

        // 2) Logical OR (||)
        if (prevState == OP_L_OR_ST) {
            if (*it == '|')
                return OP_L_OR;
            else
                return OP_SINGLE;
        }
        else if (*it == '|')
            return OP_L_OR_ST;
        // end of Logical OR (||)

        return OP_SINGLE;
    }

    return ERR;
}

void Lexer::tokenize(const std::string &input) {
    // Opens the `input` file and copies its content into `data`
    std::ifstream sourceFile {input};
    std::string data {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};
    sourceFile.close();

    if (data.empty())
        throw Exception{"Empty file " + input};

    int column    = MAIN;   // column of a DFA state (a.k.a. state type)
    int currState = MAIN;   // current DFA state
    int prevState = MAIN;   // previous DFA state
    std::string tokenBuff;  // buffer in which we (gradually) store the value of a token

    // use an FSM to parse the expression
    for(auto it = data.begin(); it != data.end(); ){
        // Column number for the current character
        column = switchState(it, prevState);

        // The current transition
        currState = transitionTbl[currState][column];

        // We have successfully parsed a token
        if(currState == MAIN){
            if(prevState != SPACE){ // skip whitespaces

                auto getTokenType = [&]() -> TokenType {
                    // Determine the right TokenType according to the previous DFA state
                    switch (prevState) {
                        // Identifier
                        case IDENT: {
                            auto search {StrToTokenKw.find(tokenBuff)};
                            if (search != StrToTokenKw.end()) {
                                return search->second;
                            } else {
                                return TokenType::IDENT;
                            }
                        }
                        // Integer number
                        case INT: {
                            return TokenType::LIT_INT;
                        }
                        // Float number
                        case FLT: {
                            return TokenType::LIT_FLT;
                        }
                        // Single operand (',', ')', '=', ...)
                        case OP_SINGLE: {
                            if (findInMap(StrToTokenOp, tokenBuff))
                                return StrToTokenOp[tokenBuff];
                            else
                                return TokenType::TOK_INVALID;
                        }
                        // Ampersand (&)
                        case OP_L_AND_ST: {
                            return TokenType::TOK_INVALID;
                        }
                        // Logic AND (&&)
                        case OP_L_AND: {
                            return TokenType::OP_AND;
                        }
                        // Tilde (|)
                        case OP_L_OR_ST: {
                            return TokenType::TOK_INVALID;
                        }
                        // Logic OR (||)
                        case OP_L_OR: {
                            return TokenType::OP_OR;
                        }
                    }
                    return TokenType::TOK_INVALID;
                };

                // Add a newly found (and recognized) token
                auto pif   = std::make_unique<PositionInFile>(input, 0, 0);
                auto token = std::make_shared<Token>(getTokenType(), tokenBuff, std::move(pif));
                tokens_.push_back(token);
            }
            tokenBuff.clear();
        } else { // Continue parsing
            tokenBuff += *it;
            ++it;

            // TODO: Skip comments in this block?
            // TODO: skip comments every single line if '#' was found
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
