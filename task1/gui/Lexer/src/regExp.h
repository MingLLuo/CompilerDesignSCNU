#ifndef LEXICAL_REGEXP_H
#define LEXICAL_REGEXP_H
#include "nfa.h"
#include "regScanner.h"
#include <stack>

class RegExp {
  public:
    enum class Type {
        EmptyString, // Epsilon
        Char,
        Union,
        Ques,
        Concat,
        Star,
        Plus,
    };

    RegExp()
        : type(Type::EmptyString), c('\0'), left(nullptr), right(nullptr) {}

    RegExp(char ch) : type(Type::Char), c(ch), left(nullptr), right(nullptr) {}

    RegExp(const std::string str)
        : type(Type::Char), c(str[0]), left(nullptr), right(nullptr) {
        if (str.size() > 1) {
            std::shared_ptr<RegExp> r = std::make_shared<RegExp>(str.substr(1));
            right = r;
            left = std::make_shared<RegExp>(c);
            c = '\0';
            type = Type::Concat;
        }
    }

    RegExp(Type t) : type(t), c('\0'), left(nullptr), right(nullptr) {}

    RegExp(std::shared_ptr<RegExp> l, std::shared_ptr<RegExp> r, Type t)
        : type(t), c('\0'), left(std::move(l)), right(std::move(r)) {}

    RegExp(std::shared_ptr<RegExp> r, Type t)
        : type(t), c('\0'), left(nullptr), right(std::move(r)) {}
    RegExp(std::vector<std::shared_ptr<RegExp>> &regExps, Type t)
        : type(t), c('\0'), left(nullptr), right(nullptr) {
        if (t == Type::Union) {
            if (regExps.size() < 2) {
                throw std::runtime_error(
                    "Union should have at least 2 operands");
            }
            left = regExps[0];
            right = regExps[1];
            for (size_t i = 2; i < regExps.size(); ++i) {
                left = std::make_shared<RegExp>(left, regExps[i], Type::Union);
            }
        } else if (t == Type::Concat) {
            if (regExps.size() < 2) {
                throw std::runtime_error(
                    "Concat should have at least 2 operands");
            }
            left = regExps[0];
            right = regExps[1];
            for (size_t i = 2; i < regExps.size(); ++i) {
                left = std::make_shared<RegExp>(left, regExps[i], Type::Concat);
            }
        }
    }

    // Destructor
    ~RegExp() = default;

    [[nodiscard]] std::shared_ptr<NFA> toNFA() const;

    Type type;
    char c;
    std::shared_ptr<RegExp> left;
    std::shared_ptr<RegExp> right;
};

std::shared_ptr<RegExp> tokensToRegExp(const std::vector<Token> &tokens);

std::shared_ptr<RegExp> stringToRegExp(const std::string &str);

std::string regExpToString(const std::shared_ptr<RegExp> &regExp);

std::string regExpToStringWithSpace(int space,
                                    const std::shared_ptr<RegExp> &regExp);

#endif // LEXICAL_REGEXP_H
