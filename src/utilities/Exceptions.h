#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <bits/exception.h>
#include <string>

namespace Wisnia::Utils {
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

    // Parser exception
    class ParserError : public WisniaError {
    public:
        explicit ParserError(const std::string &msg) : WisniaError("Syntax Analysis Error: " + msg) {}
        virtual const char* what() const throw() { return msg_.c_str(); }
    };

    // Not-Implemented exception
    class NotImplementedError : public WisniaError {
    public:
        explicit NotImplementedError(const std::string &msg) : WisniaError("Not Implemented Error: " + msg) {}
        virtual const char* what() const throw() { return msg_.c_str(); }
    };
} // Wisnia::Utils

#endif // EXCEPTION_H
