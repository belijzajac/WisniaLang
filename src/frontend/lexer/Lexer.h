#ifndef LEXER_H
#define LEXER_H

#include <memory>
#include <string>
#include <vector>

namespace Wisnia {
namespace Basic {
class Token;
enum class TType;
}  // namespace Basic

// TODO: hide the implementation of private parts with the PIMPL idiom
class Lexer {
  enum class State {
    START,             // Start state
    IDENT,             // Identifier
    OP_COMPARE,        // Either a single operand (!, <, >, =) or (!=, <=, >=, ==)
    STRING,            // String literal
    INTEGER,           // Integer literal
    FLOAT,             // Float literal
    ERRONEOUS_NUMBER,  // Invalid number
    LOGIC_AND,         // Logical AND: &&
    LOGIC_OR,          // Logical OR: ||
    OP_PP,             // Unary prefix: ++
    OP_MM,             // Unary prefix: --

    ESCAPE_SEQ,        // Escapes \t, \n, etc.
    CMT_SINGLE,        // Single line comment: #

    // Escape multi-line comments
    CMT_I,
    CMT_II,
    CMT_III,
  };

  struct TokenState {
    // Info needed to construct a token and to tokenize a letter
    State state_{State::START};
    std::string buff_;

    // Accessors to the actual data of the source file
    std::string data_;
    std::string::iterator it_;

    // Vague info about the source file
    std::string fileName_;
    int lineNo{1};

    // String info
    int stringStart{0};

    // Temp info
    std::string erroneousType_;
  };

  // Having provided the TType, it constructs and returns a token
  std::shared_ptr<Basic::Token> finishTok(const Basic::TType &type_, bool backtrack = false);

  // From an existing token buffer constructs and returns a token of identifier
  // (or keyword) type
  std::shared_ptr<Basic::Token> finishIdent();

  // Continues to tokenize the next letter
  std::shared_ptr<Basic::Token> tokNext(char ch);

  // Tokenizes whatever was passed to the tokenize function
  void tokenizeInput();

  // Preps up tokenization
  void tokenize(const std::string &filename);
  void tokenize(std::istringstream &sstream);

 public:
  explicit Lexer(const std::string &filename);
  explicit Lexer(std::istringstream &sstream);
  ~Lexer() = default;

  // Returns tokens
  std::vector<std::shared_ptr<Basic::Token>> getTokens() const { return tokens_; }

  // Prints out tokens in a pretty table
  void prettyPrint();

 private:
  std::vector<std::shared_ptr<Basic::Token>> tokens_;
  TokenState tokenState_;
};

}  // namespace Wisnia

#endif  // LEXER_H
