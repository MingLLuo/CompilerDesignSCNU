#include "regExp.h"

std::shared_ptr<RegExp> tokensToRegExp(const std::vector<Token> &tokens) {
    std::stack<std::shared_ptr<RegExp>> regexStack;
    std::stack<TokenType> operatorStack;

    auto precedence = [](TokenType op) -> int {
        if (op == TokenType::Star || op == TokenType::PLUS ||
            op == TokenType::QUES)
            return 1;
        if (op == TokenType::Union)
            return 0;
        if (op == TokenType::LParen)
            return 2;
        return -1; // Invalid operator
    };

    auto applyOperator = [&]() {
        // without Concat
        if (regexStack.empty() || operatorStack.empty())
            throw std::runtime_error("Invalid expression");

        auto r1 = regexStack.top();
        regexStack.pop();
        TokenType op = operatorStack.top();
        operatorStack.pop();

        switch (op) {
        case TokenType::Union: {
            if (regexStack.empty())
                throw std::runtime_error("Invalid expression");
            auto r2 = regexStack.top();
            regexStack.pop();
            regexStack.push(
                std::make_shared<RegExp>(r1, r2, RegExp::Type::Union));
            break;
        }
        case TokenType::Star: {
            regexStack.push(std::make_shared<RegExp>(r1, RegExp::Type::Star));
            break;
        }
        case TokenType::PLUS: {
            regexStack.push(std::make_shared<RegExp>(r1, RegExp::Type::Plus));
            break;
        }
        case TokenType::QUES: {
            regexStack.push(std::make_shared<RegExp>(r1, RegExp::Type::Ques));
            break;
        }
        default:
            throw std::runtime_error("Invalid operator");
        }
    };

    for (const auto &token : tokens) {
        switch (token.type) {
        case TokenType::Char:
            regexStack.push(std::make_shared<RegExp>(token.value));
            while (operatorStack.size() > 1 &&
                   precedence(operatorStack.top()) == 0)
                applyOperator();
            break;
        case TokenType::Epsilon:
            regexStack.push(std::make_shared<RegExp>());
            while (operatorStack.size() > 1 &&
                   precedence(operatorStack.top()) == 0)
                applyOperator();
            break;
        case TokenType::Union:
        case TokenType::Star:
        case TokenType::PLUS:
        case TokenType::QUES: {
            if (regexStack.empty()) {
                throw std::runtime_error("Invalid expression");
            }
            while (!operatorStack.empty() &&
                   operatorStack.top() != TokenType::LParen) {
                applyOperator();
            }
            operatorStack.push(token.type);
            if (token.type != TokenType::Union)
                applyOperator();
            break;
        }
        case TokenType::LParen:
            operatorStack.push(TokenType::LParen);
            // mark for RParen combine
            regexStack.push(nullptr);
            break;
        case TokenType::RParen: {
            while (!operatorStack.empty() &&
                   operatorStack.top() != TokenType::LParen) {
                applyOperator();
            }
            if (!operatorStack.empty() &&
                operatorStack.top() == TokenType::LParen) {
                operatorStack.pop(); // Pop LParen
                std::shared_ptr<RegExp> concatExpr = nullptr;
                // Concatenate RegExp expressions within ()
                while (!regexStack.empty() && regexStack.top() != nullptr) {
                    auto expr = regexStack.top();
                    regexStack.pop();
                    if (concatExpr == nullptr) {
                        concatExpr = expr;
                    } else {
                        concatExpr = std::make_shared<RegExp>(
                            expr, concatExpr, RegExp::Type::Concat);
                    }
                }
                if (concatExpr == nullptr)
                    throw std::runtime_error(
                        "Invalid expression: Empty parentheses");
                regexStack.pop(); // Pop the nullptr (LPAREN)
                regexStack.push(concatExpr);
            } else {
                throw std::runtime_error("Mismatched parentheses");
            }
            break;
        }
        default:
            throw std::runtime_error("Invalid token type");
        }
    }

    while (!operatorStack.empty()) {
        if (operatorStack.top() == TokenType::LParen)
            //  LRaren failed to combine
            throw std::runtime_error("Mismatched parentheses");
        applyOperator();
    }

    auto result = regexStack.top();
    regexStack.pop();
    while (!regexStack.empty()) {
        auto reg1 = regexStack.top();
        regexStack.pop();
        if (reg1->type != RegExp::Type::EmptyString) {
            result =
                std::make_shared<RegExp>(reg1, result, RegExp::Type::Concat);
        }
    }

    return result;
}

std::shared_ptr<RegExp> stringToRegExp(const std::string &s) {
    std::vector<Token> tokens = regTokenize(s);
    //    regTokenPrint(tokens);
    return tokensToRegExp(tokens);
}

std::shared_ptr<NFA> RegExp::toNFA() const {
    switch (type) {
    case Type::EmptyString: {
        auto nfa = std::make_shared<NFA>();
        auto start_state = std::make_shared<NFAState>(fresh());
        auto final_state = std::make_shared<NFAState>(fresh(), true);
        start_state->transitions[0].insert(final_state);
        nfa->states.insert(start_state);
        nfa->states.insert(final_state);
        nfa->start_state = start_state;
        return nfa;
    }
    case Type::Char: {
        auto nfa = std::make_shared<NFA>();
        auto start_state = std::make_shared<NFAState>(fresh());
        auto final_state = std::make_shared<NFAState>(fresh(), true);
        start_state->transitions[c].insert(final_state);
        nfa->states.insert(start_state);
        nfa->states.insert(final_state);
        nfa->start_state = start_state;
        nfa->symbols.insert(c);
        return nfa;
    }
    case Type::Union: {
        auto nfa1 = left->toNFA();
        auto nfa2 = right->toNFA();
        auto result = unionNFAs(nfa1, nfa2);
        return result;
    }
    case Type::Concat: {
        auto nfa1 = left->toNFA();
        auto nfa2 = right->toNFA();
        auto result = concatNFAs(nfa1, nfa2);
        return result;
    }
    case Type::Star: {
        auto nfa = right->toNFA();
        auto result = starNFA(nfa);
        return result;
    }
    case Type::Plus: {
        // without minimize
        auto nfa = right->toNFA();
        auto result = plusNFA(nfa);
        return result;
    }
    case Type::Ques: {
        auto nfa = right->toNFA();
        auto result = quesNFA(nfa);
        return result;
    }

    default:
        return nullptr;
    }
}
std::string spaces(int count) { return std::string(count, ' '); }

std::string regExpToString(const std::shared_ptr<RegExp> &regExp) {
    if (regExp == nullptr)
        return "";
    switch (regExp->type) {
    case RegExp::Type::EmptyString:
        return "";
    case RegExp::Type::Char:
        return std::string(1, regExp->c);
    case RegExp::Type::Union:
        return "(" + regExpToString(regExp->right) + "|" +
               regExpToString(regExp->left) + ")";
    case RegExp::Type::Ques:
        return "(" + regExpToString(regExp->right) + ")?";
    case RegExp::Type::Concat:
        return regExpToString(regExp->left) + regExpToString(regExp->right);
    case RegExp::Type::Star:
        return "(" + regExpToString(regExp->right) + ")*";
    case RegExp::Type::Plus:
        return "(" + regExpToString(regExp->right) + ")+";
    default:
        throw std::runtime_error("Invalid RegExp type");
    }
}

// newspace = space + 2
std::string regExpToStringWithSpace(int space,
                                    const std::shared_ptr<RegExp> &regExp) {
    if (regExp == nullptr) {
        return "";
    }
    switch (regExp->type) {
    case RegExp::Type::EmptyString:
        return "[Empty]";
    case RegExp::Type::Char:
        return spaces(space) + std::string(1, regExp->c) + "\n";
    case RegExp::Type::Union:
        return spaces(space) + "op(|)\n" +
               regExpToStringWithSpace(space + 2, regExp->left) +
               regExpToStringWithSpace(space + 2, regExp->right);
    case RegExp::Type::Ques:
        return spaces(space) + "op(?)\n" +
               regExpToStringWithSpace(space + 2, regExp->right);
    case RegExp::Type::Concat:
        return spaces(space) + "op(&)\n" +
               regExpToStringWithSpace(space + 2, regExp->left) +
               regExpToStringWithSpace(space + 2, regExp->right);
    case RegExp::Type::Star:
        return spaces(space) + "op(#)\n" +
               regExpToStringWithSpace(space + 2, regExp->right);
    case RegExp::Type::Plus:
        return spaces(space) + "op(+)\n" +
               regExpToStringWithSpace(space + 2, regExp->right);
    default:
        throw std::runtime_error("Invalid RegExp type");
    }
}