/*
 * File: grammar.h
 * Project: Parser
 * Author: MingLLuo
 * Usage: Define the Grammar class
 * Created on December 6, 2023
 */
#include <iostream>

#include "pattern.h"
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#ifndef SLRPARSER_GRAMMAR_H
#define SLRPARSER_GRAMMAR_H

struct Production {
  std::string lhs{};
  std::vector<std::vector<std::string>> rhs{};
};

class Grammar {
public:
  Grammar(const std::string &s, const std::string &patternFile)
      : pattern(s, patternFile) {
    parseRule();
    computeFirstSets();
    computeFollowSets();
  }

  Grammar() = default;
  void init(const std::string &s, const std::string &patternFile) {
    pattern = Pattern(s, patternFile);
    parseRule();
    computeFirstSets();
    computeFollowSets();
  }

  Pattern pattern;
  std::vector<Production> productions;
  std::unordered_set<std::string> nonTerminals;
  std::unordered_set<std::string> terminals;
  std::unordered_map<std::string, std::unordered_set<std::string>> firstSets;
  std::unordered_map<std::string, std::unordered_set<std::string>> followSets;

  void parseRule();
  void parseText(const std::string &s);
  std::vector<std::string> tokensSplit(const std::string &s);

  void computeFirstSets();

  void computeFollowSets();

  bool isNonTerminal(std::string str) {
    return nonTerminals.find(str) != nonTerminals.end();
  }
  bool isTerminal(std::string str) {
    return terminals.find(str) != terminals.end();
  }
  bool isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
  }

  void printInfo() {
    // print terminals
    std::cout << "Terminals: ";
    // sort terminals
    std::vector<std::string> sortedTerminals(terminals.begin(),
                                             terminals.end());
    std::sort(sortedTerminals.begin(), sortedTerminals.end());
    for (const auto &t : sortedTerminals) {
      std::cout << t << " ";
    }
    std::cout << std::endl;
    // print non-terminals
    std::cout << "Non-terminals: ";
    // sort non-terminals
    std::vector<std::string> sortedNonTerminals(nonTerminals.begin(),
                                                nonTerminals.end());
    std::sort(sortedNonTerminals.begin(), sortedNonTerminals.end());
    for (const auto &nt : sortedNonTerminals) {
      std::cout << nt << " ";
    }
    std::cout << std::endl;

    // print productions
    for (const auto &prod : productions) {
      std::cout << prod.lhs << " -> ";
      for (const auto &rhs : prod.rhs) {
        for (const auto &r : rhs) {
          std::cout << r << " ";
        }
        std::cout << "| ";
      }
      std::cout << std::endl;
    }

    printFirstSets();

    std::cout << std::endl;
    printFollowSets();
  }

  void printFirstSets();
  void printFollowSets();
  std::string firstSetsStr() {
    std::stringstream ss;
    ss << "First sets:" << std::endl;
    for (const auto &[lhs, firstSet] : this->firstSets) {
      if (!Grammar::isNonTerminal(lhs)) {
        continue;
      }
      ss << lhs << ": ";
      // print sorted first set
      std::vector<std::string> sortedFirstSet(firstSet.begin(), firstSet.end());
      std::sort(sortedFirstSet.begin(), sortedFirstSet.end());
      for (const auto &c : sortedFirstSet) {
        ss << c << " ";
      }
      ss << std::endl;
    }
    return ss.str();
  }
  std::string followSetsStr() {
    std::stringstream ss;
    ss << "Follow sets:" << std::endl;
    for (const auto &[lhs, followSet] : this->followSets) {
      if (!Grammar::isNonTerminal(lhs)) {
        continue;
      }
      ss << lhs << ": ";
      // print sorted follow set
      std::vector<std::string> sortedFollowSet(followSet.begin(),
                                               followSet.end());
      std::sort(sortedFollowSet.begin(), sortedFollowSet.end());
      for (const auto &c : sortedFollowSet) {
        ss << c << " ";
      }
      ss << std::endl;
    }
    return ss.str();
  }
};

#endif // SLRPARSER_GRAMMAR_H
