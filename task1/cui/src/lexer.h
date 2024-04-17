#ifndef LEXER_H
#define LEXER_H
#include "dfa.h"
#include "pattern.h"
#include "regExp.h"
class Lexer {
public:
  void lexerInit();
  // Lexer(const std::string &filePath) : pattern(filePath) { lexerInit(); }
  Lexer(std::string s, const std::string &filePath) : pattern(s, filePath) {
    lexerInit();
  }
  Lexer() = default;
  void setPattern(std::string s, const std::string &filePath) {
    pattern = Pattern(s, filePath);
    lexerInit();
  }

  std::string generateLexer();
  Pattern pattern;

  std::map<std::string, std::shared_ptr<RegExp>> regExps;
  std::map<std::string, std::shared_ptr<DFA>> dfas;
  std::shared_ptr<DFA> finalDFA;
};

#endif // LEXER_H