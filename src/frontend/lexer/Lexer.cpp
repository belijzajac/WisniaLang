#include "Lexer.h"
#include "dfa/DFA.h"
#include "token/Token.h"
#include "../../utilities/Exception.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

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
void remSingleLnCmts(std::string::iterator &it, const std::string &data, int &lineNo) {
    // It's a single-line comment opener
    if(*it == '#') {
        // Skip everything until the end of the line
        while(it != data.end() && *it != '\n')
            ++it;

        // We've just skipped a line
        //if (*it == '\n')
        //    ++lineNo;
    }
}

// Skips multiple-line comments
void remMultipleLnCmts(std::string::iterator &it, const std::string &data, int &lineNo) {
    // If it's a comment opener ("/*")
    if(*it == '/' && it+1 != data.end() && *(it+1) == '*') {
        // Skip characters as long as we find an occurrence of "*/"
        while(*it != '*' || it+1 == data.end() || *(it+1) != '/') {
            ++it;

            // Check if we've reached the following line
            if (*it == '\n')
                ++lineNo;

            // We've reached the end of the file and didn't find the "*/"
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

// Returns an appropriate TokenType per provided previous state
TokenType getTokenType(int prevState, std::string::iterator &it, std::string &buff) {
    // Determine the right TokenType according to the previous DFA state
    switch (prevState) {
        // Identifier
        case IDENT: {
            auto search {StrToTokenKw.find(buff)};
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
            if (findInMap(StrToTokenOp, buff))
                return StrToTokenOp[buff];
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
            buff += *it;
            ++it;
            return TokenType::OP_FN_ARROW;
        }
        // Unary add (++)
        case OP_PP: {
            buff += *it;
            ++it;
            return TokenType::OP_UADD;
        }
    }
    return TokenType::TOK_INVALID;
}

void Lexer::tokenize(const std::string &input) {
    // Opens the `input` file and copies its content into `data`
    std::ifstream sourceFile {input};
    std::string data {std::istreambuf_iterator<char>(sourceFile), std::istreambuf_iterator<char>()};

    if (data.empty())
        throw Exception{"Empty file " + input};

    // Add a newline at the end of the data if there's none already.
    // This comes in handy to save the last token from getting dismissed
    // when we reach the end of file and escape the condition `if(currState == MAIN)`
    if (data.back() != '\n')
        data += '\n';

    int column    = MAIN;   // column of a DFA state (a.k.a. state type)
    int currState = MAIN;   // current DFA state
    int prevState = MAIN;   // previous DFA state
    std::string tokenBuff;  // buffer in which we (gradually) store the value of a token
    int lineNo = 1;         // Current line number

    // use an DFA to parse the expression
    for (auto it = data.begin(); it != data.end(); ) {
        // Treat single-line comments
        remSingleLnCmts(it, data, lineNo);
        // Nothing left to read, so break the for-loop
        if (it == data.end())
            break;

        // Treat multiple-line comments
        remMultipleLnCmts(it, data, lineNo);
        // Nothing left to read, so break the for-loop
        if(it == data.end())
            break;

        // Column number for the current character
        column = switchState(it, prevState);

        // The current transition
        currState = transitionTbl[currState][column];

        // We have successfully parsed a token
        if (currState == MAIN) {
            if (prevState != SPACE) { // skip whitespaces
                // Add a newly found (and recognized) token
                auto pif       = std::make_unique<PositionInFile>(input, lineNo, 0);
                TokenType type = getTokenType(prevState, it, tokenBuff);
                auto token     = std::make_shared<Token>(type, tokenBuff, std::move(pif));
                tokens_.push_back(token);
            } else if (*it == '\n') {
                ++lineNo; // new line
            }
            tokenBuff.clear();
        } else { // Continue parsing
            tokenBuff += *it;
            ++it;
        }
        prevState = currState;
    }

    // Add the EOF token to mark the file's ending
    auto pif   = std::make_unique<PositionInFile>(input, lineNo, 0);
    auto token = std::make_shared<Token>(TokenType::TOK_EOF, "", std::move(pif));
    tokens_.push_back(token);
}

void Lexer::prettyPrint() {
    size_t index = 0;

    // Print place-holder
    std::cout << std::left << std::setw(5)  << "ID" << " | "
                           << std::setw(5)  << "LN" << " | "
                           << std::setw(15) << "TYPE" << " | " << "VALUE\n";
    std::cout << "------+-------+-----------------+-----------------\n";

    for(auto & token : tokens_){
        std::cout << std::left << std::setw(5)   << index << " | "
                               << std::setw(5)   << token->getFileInfo()->getLineNo() << " | "
                               << std::setw(15)  << token->getName() << " | "
                               << token->getValue() << "\n";
        ++index;
    }
}
