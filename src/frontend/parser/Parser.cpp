#include "Parser.h"
#include "../lexer/Lexer.h"
#include "../lexer/token/Token.h"
#include "../lexer/token/TokenType.h"
#include "../../utilities/Exception.h"

Parser::Parser(const Lexer &lexer) {
    tokens_ = lexer.getTokens();
}

bool Parser::has(const TokenType &token) {
    return peek()->getType() == token;
}

// TODO: Instead of throwing exceptions, make an option to instead print error msg to STDERR
void Parser::expect(const TokenType &token) {
    if (peek()->getType() != token) {
        throw Exception{"Expected " + TokenTypeToStr[token] + " but found " + TokenTypeToStr[peek()->getType()]};
    } else {
        consume();
    }
}
