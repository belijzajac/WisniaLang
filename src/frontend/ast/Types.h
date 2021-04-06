#ifndef AST_TYPES_H
#define AST_TYPES_H

#include <sstream>
// Wisnia
#include "Root.h"
#include "../basic/TType.h"
#include "../basic/Token.h"
#include "../../utilities/Exceptions.h"

namespace Wisnia {
    namespace AST {
        // An abstract definition for Type node
        class Type : public Root {
        protected:
            Basic::TType type_;   // enum representing type
            std::string typeStr_; // string representation of Type (for printing)

        private:
            void convertTypeToStr() {
                // Returns a string equivalent of an enum
                auto primTypeStr = [&]() -> std::string {
                    switch (type_) {
                        case Basic::TType::KW_VOID :
                            return "void";
                        case Basic::TType::KW_INT :
                            return "int";
                        case Basic::TType::KW_BOOL :
                            return "bool";
                        case Basic::TType::KW_FLOAT :
                            return "float";
                        case Basic::TType::KW_STRING :
                            return "string";
                        default:
                            throw Utils::NotImplementedError{Basic::TokenTypeToStr[type_]};
                    }
                };

                typeStr_ = primTypeStr();
            }

        public:
            Type(const std::shared_ptr<Basic::Token> &tok) {
                type_ = tok->getType();
                convertTypeToStr();
            }

            void print(size_t level) const override {
                Root::print(level);
            }
        };

        // Function Type node
        class PrimitiveType : public Type {
        public:
            explicit PrimitiveType(const std::shared_ptr<Basic::Token> &tok) : Type(tok) { token_ = tok; }

            const std::string kind() const override {
                std::stringstream ss;
                ss << "PrimitiveType" << " (" << typeStr_ << ")";
                return ss.str();
            }

            void print(size_t level) const override {
                Root::print(level);
            }
        };
    }
}

#endif // AST_TYPES_H
