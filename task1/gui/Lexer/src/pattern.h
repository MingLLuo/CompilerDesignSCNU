/*
 * File: pattern.h
 * Project: Lexer
 * Author: MingLLuo
 * Usage: Define the Pattern class
 */
#ifndef PATTERN_H
#define PATTERN_H
#include "regScanner.h"
#include <fstream>
#include <map>
#include <set>
#include <sstream>

class Pattern {
public:
  std::map<std::string, int> keywords;
  std::map<std::string, int> specialSymbols;
  std::string lcomment, rcomment, comment;
  std::string start;
  std::map<std::string, std::vector<std::string>> rules;
  std::vector<char> letters;
  std::vector<char> digits;
  std::string idRegex;
  std::string numRegex;
  std::set<std::string> allTokens;

  Pattern() = default;
  Pattern(std::string s, const std::string &filePath) {
    if (s.empty()) {
      loadPatterns(filePath);
    } else {
      readPatterns(s);
    }
  }

  void loadPatterns(const std::string &filePath);
  void readPatterns(std::string s);
  void printPatterns() const;
  std::string patternToString() const;
  std::vector<std::string> keywordsToRegScanner() const;
  std::vector<std::string> specialSymbolsToRegScanner() const;
  std::string idRegexToRegScanner() const;
  std::string numRegexToRegScanner() const;
  std::string commentRegexToRegScanner() const;

  bool isKeyword(const std::string &lexeme) const {
    return keywords.find(lexeme) != keywords.end();
  }

  bool isSpecialSymbol(const std::string &lexeme) const {
    return specialSymbols.find(lexeme) != specialSymbols.end();
  }

  bool isComment(const std::string &lexeme) const {
    return lexeme == lcomment || lexeme == rcomment;
  }

  bool isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
  }

  bool isDigit(char c) const {
    return digits.end() != std::find(digits.begin(), digits.end(), c);
  }

  bool isAlpha(char c) const {
    return letters.end() !=
           std::find(letters.begin(), letters.end(), std::tolower(c));
  }

  bool isAlphaNumeric(char c) const { return isAlpha(c) || isDigit(c); }
};
#endif // PATTERN_H