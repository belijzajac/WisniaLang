#include "Lexer.h"
#include "dfa/DFA.h"
#include "token/Token.h"
#include "../../utilities/Exception.h"
#include <fstream>
#include <iostream>

#include <cctype>
#include <string>
#include <vector>
using namespace std;

// struct to hold token information
/*struct TokenType {
    string token;
    int lexeme;
    string lexemeName;
};*/

// function prototypes
int Get_FSM_Col(char currentChar);
string GetLexemeName(int lexeme);

// Determines the state of the type of character being examined.
int Get_FSM_Col(char currentChar)
{
    // Whitespace
    if(isspace(currentChar))
        return SPACE;

    // Integer
    else if(isdigit(currentChar))
        return INTEGER;

    // Real number
    else if(currentChar == '.')
        return REAL;

    // Characters
    else if(isalpha(currentChar))
        return IDENT;

    // Operators
    else if(ispunct(currentChar))
        return OPERATOR;

    return UNKNOWN;
}

// Returns the string equivalent of an integer lexeme token type
string GetLexemeName(int lexeme) {
    switch(lexeme) {
        case INTEGER:
            return "INTEGER";
        case REAL:
            return "REAL";
        case OPERATOR:
            return "OPERATOR";
        case IDENT:
            return "IDENT";
        case UNKNOWN:
            return "UNKNOWN";
        case SPACE:
            return "SPACE";
        default:
            return "ERROR";
    }
}

void Lexer::tokenize(const std::string &input) {
    // Opens the `input` file and copies its content into `data`
    std::ifstream sourceFile {input};
    std::string data {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};
    sourceFile.close();

    if (data.empty())
        throw Exception{"Empty file " + input};

    // Extracts tokens from the given source file
    //TokenType access;
    //vector<TokenType> tokens;
    char currentChar;
    int col = REJECT;
    int currentTransition = REJECT;
    int prevState = REJECT;
    string currentToken;

    // A const iterator to `data`'s content
    std::string::const_iterator lineBegin = data.begin();

    // use an FSM to parse the expression
    for(unsigned index = 0; index < data.length(); ){
        currentChar = data[index];

        // Column number for the current character
        col = Get_FSM_Col(currentChar);

        // The current transition
        currentTransition = transitionTbl[currentTransition][col];

        // We have successfully parsed a token.
        if(currentTransition == REJECT){
            if(prevState != SPACE){ // we don't care about whitespaces
                /*access.token = currentToken;
                access.lexeme = prevState;
                access.lexemeName = GetLexemeName(access.lexeme);
                tokens.push_back(access);*/
            }
            currentToken.clear();
        } else { // Continue parsing
            currentToken += currentChar;
            ++index;
        }
        prevState = currentTransition;
    }

    // this ensures the last token gets saved when
    // we reach the end of the loop (if a valid token exists)
    if(currentTransition != SPACE && !currentToken.empty()){
        // ^^ we dont care about whitespace
        /*access.token = currentToken;
        access.lexeme = currentTransition;
        access.lexemeName = GetLexemeName(access.lexeme);
        tokens.push_back(access);*/
    }

    // Display the tokens
    for(auto & token : tokens_){
        /*cout << token.lexemeName << " ==> " << token.token << endl;*/
    }
}
