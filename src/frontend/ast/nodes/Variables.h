#ifndef AST_VARIABLES_H
#define AST_VARIABLES_H

// Wisnia
#include "Expressions.h"

namespace Wisnia {
namespace Basic {
class Token;
}

namespace AST {
// Variable node
class Var : public Expr {
 public:
  explicit Var(const std::shared_ptr<Basic::Token> &tok) { token_ = tok; }
  Var() = default;

  std::string kind() const override {
    std::stringstream ss;
    ss << "Var" << " (" << token_->getValueStr() << ")";
    return ss.str();
  }

  void print(size_t level) const override {
    Root::print(level);
  }
};

}  // namespace AST
}  // namespace Wisnia

#endif  // AST_VARIABLES_H
