#include "regScanner.h"

Token Scanner::getNextTokenFromStr() {
    if (pos >= str.length())
        return {TokenType::END};

    char currentChar = str[pos];

    switch (currentChar) {
    case '|':
        ++pos;
        return {TokenType::Union};
    case '*':
        ++pos;
        return {TokenType::Star};
    case '+':
        ++pos;
        return {TokenType::PLUS};
    case '?':
        ++pos;
        return {TokenType::QUES};
    case '(':
        ++pos;
        return {TokenType::LParen};
    case ')':
        ++pos;
        return {TokenType::RParen};
    default:
        if (currentChar) {
            ++pos;
            return {TokenType::Char, currentChar};
        } else {
            throw std::runtime_error("Invalid character in input");
        }
    }
}

// Function to tokenize input string
std::vector<Token> regTokenize(const std::string &str) {
    std::vector<Token> tokens;
    Scanner scanner(str);

    Token token = scanner.getNextTokenFromStr();
    while (token.type != TokenType::END) {
        tokens.push_back(token);
        token = scanner.getNextTokenFromStr();
    }

    return tokens;
}

void regTokenPrint(const std::vector<Token> &tokens) {
    for (const auto &token : tokens) {
        switch (token.type) {
        case TokenType::Char:
            std::cout << "Char: " << token.value << std::endl;
            break;
        case TokenType::Concat:
            std::cout << "Concat" << std::endl;
            break;
        case TokenType::Union:
            std::cout << "Union" << std::endl;
            break;
        case TokenType::Star:
            std::cout << "Star" << std::endl;
            break;
        case TokenType::PLUS:
            std::cout << "Plus" << std::endl;
            break;
        case TokenType::QUES:
            std::cout << "Ques" << std::endl;
            break;
        case TokenType::LParen:
            std::cout << "LParen" << std::endl;
            break;
        case TokenType::RParen:
            std::cout << "RParen" << std::endl;
            break;
        case TokenType::END:
            std::cout << "END" << std::endl;
            break;
        case TokenType::Epsilon:
            std::cout << "Epsilon" << std::endl;
            break;
        }
    }
}

bool isTypeChar(char c) {
    if (c == '|' || c == '*' || c == '+' || c == '?' || c == '(' || c == ')' ||
        c == '\0') {
        return true;
    }
    return false;
}