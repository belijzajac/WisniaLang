#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <bits/exception.h>
#include <string>

// Base exception
class WisniaError : public std::exception {
public:
    explicit WisniaError(const std::string &msg) : msg_{msg} {}
    virtual const char* what() const throw() { return msg_.c_str(); }

protected:
    std::string msg_;
};

// Lexer exception
class LexerError : public WisniaError {
public:
    explicit LexerError(const std::string &msg) : WisniaError("Lexical Analysis Error: " + msg) {}
    virtual const char* what() const throw() { return msg_.c_str(); }
};

// Parser and AST exception
class ParserError : public WisniaError {
public:
    explicit ParserError(const std::string &msg) : WisniaError("Syntax Analysis Error: " + msg) {}
    virtual const char* what() const throw() { return msg_.c_str(); }
};

#endif // EXCEPTION_H
