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

    // Identifier
    else if(std::isalpha(*it) || *(it) == '_') {
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
        else if (prevState == OP_L_OR_ST) {
            if (*it == '|')
                return OP_L_OR;
            else
                return OP_SINGLE;
        }
        else if (*it == '|')
            return OP_L_OR_ST;
        // end of Logical OR (||)

        // 3) Either a single operand (!, <, >, =) or (!=, <=, >=, ==)
        else if (prevState == OP_CMP_ST) {
            if (*it == '=')
                return OP_CMP_EN;
            else
                return OP_SINGLE;
        }
        else if (*it == '!' || *it == '<' || *it == '>' || *it == '=')
            return OP_CMP_ST;
        // end of 3)

        // 4) (+, -) or (++, ->)
        else if (*it == '+' && *(it+1) == '+')
            return OP_PP;
        else if (*it == '-' && *(it+1) == '>')
            return OP_ARROW;

        return OP_SINGLE;
    }

    return ERR;
}

// Skips single-line comments
void remSingleLnCmts(std::string::iterator &it, const std::string &data) {
    // It's a single-line comment opener
    if(*it == '#') {
        // Skip everything until the end of the line
        while(it != data.end() && *it != '\n')
            ++it;
    }
}

// Skips multiple-line comments
void remMultipleLnCmts(std::string::iterator &it, const std::string &data) {
    // If it's a comment opener ("/*")
    if(*it == '/' && it+1 != data.end() && *(it+1) == '*') {
        // Skip characters as long as we find an occurrence of "*/"
        while(*it != '*' || it+1 == data.end() || *(it+1) != '/') {
            ++it;

            // We reached the end of the file and didn't find the "*/"
            if (it == data.end()) {
                throw Exception{"The comment wasn't closed"};
            }
        }
        // Take care of the closing comment
        it += 2;
    }
    // Instead of "/*" we found "*/", which shouldn't have happened
    else if(*it == '*' && it+1 != data.end() && *(it+1) == '/') {
        throw Exception{"Found a comment closing that wasn't opened"};
    }
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

    // use an DFA to parse the expression
    for(auto it = data.begin(); it != data.end(); ){
        // Treat single-line comments
        remSingleLnCmts(it, data);
        // Nothing left to read, so break the for-loop
        if(it == data.end())
            break;

        // Treat multiple-line comments
        remMultipleLnCmts(it, data);
        // Nothing left to read, so break the for-loop
        if(it == data.end())
            break;

        // Column number for the current character
        column = switchState(it, prevState);

        // The current transition
        currState = transitionTbl[currState][column];

        // We have successfully parsed a token
        if(currState == MAIN){
            if(prevState != SPACE){ // skip whitespaces

                // TODO: move out of the main loop?
                // It has gotten pretty big, tbh
                // TokenType getTokenType(std::string::const_iterator &it, std::string &buff) ??
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

                        // Operands
                        case OP_SINGLE: // Single operand, e.g. (, }, >, =
                        case OP_CMP_ST: // Any of !, <, >, and =
                        case OP_CMP_EN: // Any of !=, <=, >=, ==
                        {
                            if (findInMap(StrToTokenOp, tokenBuff))
                                return StrToTokenOp[tokenBuff];
                            else
                                return TokenType::TOK_INVALID;
                        }

                        // Invalid operands for Ampersand and Tilde
                        case OP_L_AND_ST: // Ampersand (&)
                        case OP_L_OR_ST:  // Tilde (|)
                        {
                            return TokenType::TOK_INVALID;
                        }

                        // Logic AND (&&)
                        case OP_L_AND: {
                            return TokenType::OP_AND;
                        }
                        // Logic OR (||)
                        case OP_L_OR: {
                            return TokenType::OP_OR;
                        }
                        // Arrow (->)
                        case OP_ARROW: {
                            tokenBuff += *it;
                            ++it;
                            return TokenType::OP_FN_ARROW;
                        }
                        // Unary add (++)
                        case OP_PP: {
                            tokenBuff += *it;
                            ++it;
                            return TokenType::OP_UADD;
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
