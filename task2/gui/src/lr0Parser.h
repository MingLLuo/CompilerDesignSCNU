/*
 * File: lr0Parser.h
 * Project: Parser
 * Author: MingLLuo
 * Usage: Define the LR0Parser class
 * Created on December 6, 2023
 */

#ifndef SLRPARSER_LR0PARSER_H
#define SLRPARSER_LR0PARSER_H

#include "grammar.h"
#include "util.h"
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <vector>

class LR0Item {
public:
  std::string lhs;
  // std::string rhs;
  std::vector<std::string> rhs;
  int productionIndex;
  int dotPosition;
  bool accept;

  bool operator<(const LR0Item &other) const {
    return std::tie(lhs, rhs, productionIndex, dotPosition) <
           std::tie(other.lhs, other.rhs, other.productionIndex,
                    other.dotPosition);
  }

  bool operator==(const LR0Item &other) const {
    return std::tie(lhs, rhs, productionIndex, dotPosition) ==
           std::tie(other.lhs, other.rhs, other.productionIndex,
                    other.dotPosition);
  }

  std::pair<std::string, bool> str() {
    std::string s;
    bool reduced = false;
    s += lhs;
    s += " --> ";
    for (int i = 0; i < rhs.size(); i++) {
      if (i == dotPosition) {
        s += ".";
      }
      s += rhs[i] + "^";
    }
    if (dotPosition == rhs.size()) {
      s += ".";
      reduced = true;
    }
    return {s, reduced};
  }
};

class ItemSet {
public:
  int stateIndex{};
  std::set<LR0Item> items;

  bool operator<(const ItemSet &other) const {
    return std::tie(stateIndex, items) <
           std::tie(other.stateIndex, other.items);
  }

  bool operator==(const ItemSet &other) const {
    return std::tie(stateIndex, items) ==
           std::tie(other.stateIndex, other.items);
  }
};

class SingleProduction {
public:
  std::string lhs{};
  std::vector<std::string> rhs;

  bool operator==(const SingleProduction &other) const {
    return std::tie(lhs, rhs) == std::tie(other.lhs, other.rhs);
  }

  bool operator<(const SingleProduction &other) const {
    return std::tie(lhs, rhs) < std::tie(other.lhs, other.rhs);
  }
};

struct Token {
  std::string type;
  std::string value;
};

struct TreeNode {
  std::string value;
  std::vector<std::shared_ptr<TreeNode>> children;
  TreeNode(std::string v) : value(v) {}
};

class LR0Parser {
public:
  explicit LR0Parser(std::string s, std::string patternFile)
      : g(s, patternFile) {
    // insert first production
    addFirstProduction(g);
    // generate single productions
    genSingleProductions(g);
    symbols.insert(symbols.end(), g.terminals.begin(), g.terminals.end());
    symbols.insert(symbols.end(), g.nonTerminals.begin(), g.nonTerminals.end());
    symbols.push_back("$");
    // find a new start
    constructItemSet("Start");
  }

  void addFirstProduction(const Grammar &g);
  void genSingleProductions(const Grammar &g);
  void constructItemSet(std::string start);
  void closure(ItemSet &s);
  ItemSet gotoSet(ItemSet &s, std::string symbol);

  std::shared_ptr<TreeNode> parse(std::vector<std::string> tokens);

  void generatePseudoCode(std::shared_ptr<TreeNode> &node,
                          std::vector<std::string> &codeList,
                          int &tempVarCounter, int indentLevel);
  std::string generatePseudoCodeWrapper(std::shared_ptr<TreeNode> root);

  virtual void printItemSets();
  Token stringToToken(std::string tokenStr);
  std::string itemSetsStr() {
    std::stringstream ss;
    ss << "ItemSet:" << std::endl;
    for (auto &itemSet : itemSets) {
      ss << "Id: " << itemSet.stateIndex << std::endl;
      for (auto item : itemSet.items) {
        auto p = item.str();
        if (p.second) {
          ss << p.first + " (reduce)" << std::endl;
        } else {
          ss << p.first << std::endl;
        }
      }
      ss << std::endl;
    }
    return ss.str();
  }

  std::string treeNodePrint(std::shared_ptr<TreeNode> root, int depth,
                            std::string prefix);

  Grammar g;
  std::vector<SingleProduction> productions;
  std::vector<ItemSet> itemSets;
  std::vector<std::string> symbols;
  std::unordered_map<int, std::unordered_map<std::string, int>> transitions;
  // initial state
  int startState = 0;
};

#endif // SLRPARSER_LR0PARSER_H
