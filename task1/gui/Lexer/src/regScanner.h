#ifndef TINY_REGSCANNER_H
#define TINY_REGSCANNER_H
#include <iostream>
#include <string>
#include <utility>
#include <vector>

enum class TokenType {
    Char,
    Concat,
    Epsilon,
    Union,
    Star,
    PLUS,
    QUES,
    LParen,
    RParen,
    END
};

struct Token {
    TokenType type;
    char value{};
    std::string charSet;

    Token(TokenType t) : type(t), value('\0') {}

    Token(TokenType t, char v) : type(t), value(v) {}

    Token(TokenType t, std::string set) : type(t), charSet(std::move(set)) {}
};

class Scanner {
public:
    Scanner(std::string input) : str(std::move(input)), pos(0) {}

    Token getNextTokenFromStr();

private:
    std::string str;
    size_t pos;
};

std::vector<Token> regTokenize(const std::string &str);
bool isTypeChar(char c);
void regTokenPrint(const std::vector<Token> &tokens);

#endif
