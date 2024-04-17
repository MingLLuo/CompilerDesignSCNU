#include "lexer.h"

void Lexer::lexerInit() {
  std::vector<std::shared_ptr<RegExp>> keywords, symbols;
  auto keywordNFA = std::make_shared<NFA>();
  auto symbolNFA = std::make_shared<NFA>();
  for (const auto &keyword : pattern.keywordsToRegScanner()) {
    if (keyword.empty()) {
      continue;
    }
    auto regExp = stringToRegExp(keyword);
    keywords.push_back(regExp);
    auto nfa = regExp->toNFA();
    nfa->setFinalStatus(keyword);
    keywordNFA = unionNFAs(keywordNFA, nfa);
  }
  for (const auto &symbol : pattern.specialSymbolsToRegScanner()) {
    // special for symbols
    if (symbol.empty()) {
      continue;
    }
    auto regExp = std::make_shared<RegExp>(symbol);
    symbols.push_back(regExp);
    auto nfa = regExp->toNFA();
    nfa->setFinalStatus(symbol);
    symbolNFA = unionNFAs(symbolNFA, nfa);
  }
  if (!keywords.empty()) {
    if (keywords.size() == 1) {
      regExps["keyword"] = keywords[0];
    } else {
      regExps["keyword"] =
          std::make_shared<RegExp>(keywords, RegExp::Type::Union);
    }
  }
  if (!symbols.empty()) {
    if (symbols.size() == 1) {
      regExps["symbol"] = symbols[0];
    } else {
      regExps["symbol"] =
          std::make_shared<RegExp>(symbols, RegExp::Type::Union);
    }
  }
  regExps["id"] = stringToRegExp(pattern.idRegexToRegScanner());
  regExps["num"] = stringToRegExp(pattern.numRegexToRegScanner());
  regExps["comment"] = stringToRegExp(pattern.commentRegexToRegScanner());

  // convert keyword NFA to DFA
  dfas["keyword"] = convertToDFA(keywordNFA);
  dfas["symbol"] = convertToDFA(symbolNFA);
  dfas["id"] = convertToDFA(regExps["id"]->toNFA())->minimizeDFA();
  dfas["id"]->setFinalStatus("id");
  dfas["num"] = convertToDFA(regExps["num"]->toNFA())->minimizeDFA();
  dfas["num"]->setFinalStatus("num");
  dfas["comment"] = convertToDFA(regExps["comment"]->toNFA())->minimizeDFA();
  dfas["comment"]->setFinalStatus("comment");

  // finalDFA
  auto finalNFA = unionNFAs(keywordNFA, symbolNFA);
  auto nfa = regExps["id"]->toNFA();
  nfa->setFinalStatus("id");
  finalNFA = unionNFAs(finalNFA, nfa);
  nfa = regExps["num"]->toNFA();
  nfa->setFinalStatus("num");
  finalNFA = unionNFAs(finalNFA, nfa);
  nfa = regExps["comment"]->toNFA();
  nfa->setFinalStatus("comment");
  finalNFA = unionNFAs(finalNFA, nfa);
  finalDFA = convertToDFA(finalNFA);
  finalDFA = finalDFA->minimizeDFA();
}