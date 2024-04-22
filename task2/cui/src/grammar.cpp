/*
 * File: grammar.cpp
 * Project: Parser
 * Author: MingLLuo
 * Usage: Define the Grammar class
 * Created on December 6, 2023
 */

#include "grammar.h"
#include <string>
#include <vector>

/**
 * @brief Parse the text to get the productions
 * @param s The input text
 */
void Grammar::parseText(const std::string &s) {
  // copy content of s to productions, split by '\n'
  std::vector<std::string> lines;
  std::istringstream iss0(s);
  std::string line;
  std::unordered_map<std::string, std::string> tmpProductionsStr;
  while (std::getline(iss0, line)) {
    lines.push_back(line);
  }
  // parse productions
  for (const auto &l : lines) {
    std::istringstream iss(l);
    std::string left;
    std::string right;
    // Read left-hand side
    std::getline(iss, left, '-');
    // clean space in left
    left.erase(std::remove(left.begin(), left.end(), ' '), left.end());
    if (left.empty()) {
      continue;
    }
    // Read right-hand side
    std::getline(iss, right, '>');
    // clean space in right
    right.erase(std::remove(right.begin(), right.end(), ' '), right.end());
    if (right.empty()) {
      continue;
    }
    nonTerminals.insert(left);
    if (tmpProductionsStr.find(left) != tmpProductionsStr.end() &&
        !tmpProductionsStr[left].empty()) {
      tmpProductionsStr[left] += "|" + right;
    } else {
      tmpProductionsStr[left] = right;
    }
  }
  for (const auto &[lhs, rhs] : tmpProductionsStr) {
    std::istringstream iss(rhs);
    std::string token;
    std::vector<std::string> rhsList;
    while (std::getline(iss, token, '|')) {
      rhsList.push_back(token);
    }
    // add production
    Production p;
    p.lhs = lhs;
    for (const auto &r : rhsList) {
      auto tokens = tokensSplit(r);
      p.rhs.push_back(tokens);
    }
  }
}

/**
 * @brief Split the string to tokens
 * @param s The input string
 * @return The tokens
 */
std::vector<std::string> Grammar::tokensSplit(const std::string &s) {
  std::vector<std::string> tokens;
  std::istringstream iss(s);
  std::string token;
  while (std::getline(iss, token, ' ')) {
    if (terminals.find(token) != terminals.end() ||
        nonTerminals.find(token) != nonTerminals.end()) {
      tokens.push_back(token);
    } else if (token[0] == '(' && token[token.size() - 1] == ')') {
      // if wrap with '()', handle it
      tokens.push_back("(");
      auto sub = tokensSplit(token.substr(1, token.size() - 2));
      for (const auto &t : sub) {
        tokens.push_back(t);
      }
      tokens.push_back(")");
    } else if (isTerminal(std::to_string(token[0]))) {
      tokens.push_back(std::to_string(token[0]));
      auto sub = tokensSplit(token.substr(1));
      for (const auto &t : sub) {
        tokens.push_back(t);
      }
    } else {
      std::cout << token[0] << isTerminal(std::to_string(token[0]))
                << std::endl;
      std::cerr << "***Unknown token: " << token << std::endl;
    }
  }
  return tokens;
}

/**
 * @brief Parse the rules to get the productions
 */
void Grammar::parseRule() {
  // set terminal first
  terminals.insert(pattern.allTokens.begin(), pattern.allTokens.end());
  // set non-terminal
  for (const auto &rule : pattern.rules) {
    nonTerminals.insert(rule.first);
  }
  terminals.insert("number");
  terminals.insert("identifier");

  // set productions
  for (const auto &rule : pattern.rules) {
    Production p;
    p.lhs = rule.first;
    for (const auto &rhs : rule.second) {
      std::istringstream iss(rhs);
      std::string token;
      std::vector<std::string> rhsList = tokensSplit(rhs);
      p.rhs.push_back(rhsList);
    }
    productions.push_back(p);
  }
}

/**
 * @brief Compute the first sets
 */
void Grammar::computeFirstSets() {
  // Initialize first sets
  auto &first = this->firstSets;
  for (const auto &prod : productions) {
    first[prod.lhs] = {};
  }

  // init for terminals
  for (const auto &str : terminals) {
    first[str].insert(str);
  }

  // Iterate until no changes
  bool changed = true;
  while (changed) {
    changed = false;
    for (const auto &prod : productions) {
      // x -> y1y2y3...yn | z1z2z3...zm
      std::string lhs = prod.lhs;
      for (const auto &rhs : prod.rhs) {
        if (rhs.empty()) {
          continue;
        }
        // A -> a1 a2 a3 ... an
        // copy first set of rhs[0] to first set of lhs
        auto rhsFirstSet = first[rhs[0]];
        bool trailing = true;
        for (const auto &c : rhsFirstSet) {
          changed |= first[lhs].insert(c).second;
        }
      }
    }
  }
}

/**
 * @brief Compute the follow sets
 */
void Grammar::computeFollowSets() {
  // Initialize first sets
  auto &follow = this->followSets;
  for (const auto &nt : nonTerminals) {
    follow[nt] = {};
  }

  // find lhs is "program"
  for (const auto &prod : productions) {
    if (prod.lhs == "program") {
      follow[prod.lhs].insert("$");
    }
  }

  bool changed = true;
  while (changed) {
    changed = false;
    for (const auto &prod : productions) {
      std::string lhs = prod.lhs;
      for (const auto &rhs : prod.rhs) {
        auto trailer = follow[lhs];
        // A -> a1a2a3...an
        // rhs.length() >= 1
        for (int i = 0; i < rhs.size(); i++) {
          std::string s = rhs[i];
          if (isNonTerminal(s)) {
            for (const auto &t : trailer) {
              changed |= follow[s].insert(t).second;
            }
          } else {
            trailer = firstSets[s];
          }
        }
      }
    }
  }
}

/**
 * @brief Print the first sets
 */
void Grammar::printFirstSets() {
  std::cout << "First sets:" << std::endl;
  for (const auto &[lhs, firstSet] : this->firstSets) {
    if (!Grammar::isNonTerminal(lhs)) {
      continue;
    }
    std::cout << lhs << ": ";
    // print sorted first set
    std::vector<std::string> sortedFirstSet(firstSet.begin(), firstSet.end());
    std::sort(sortedFirstSet.begin(), sortedFirstSet.end());
    for (const auto &c : sortedFirstSet) {
      std::cout << c << " ";
    }
    std::cout << std::endl;
  }
}

/**
 * @brief Print the follow sets
 */
void Grammar::printFollowSets() {
  std::cout << "Follow sets:" << std::endl;
  for (const auto &[lhs, followSet] : this->followSets) {
    if (!Grammar::isNonTerminal(lhs)) {
      continue;
    }
    std::cout << lhs << ": ";
    // print sorted follow set
    std::vector<std::string> sortedFollowSet(followSet.begin(),
                                             followSet.end());
    std::sort(sortedFollowSet.begin(), sortedFollowSet.end());
    for (const auto &c : sortedFollowSet) {
      std::cout << c << " ";
    }
    std::cout << std::endl;
  }
}
