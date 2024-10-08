/*
 * File: slr1Parser.cpp
 * Project: Parser
 * Author: MingLLuo
 * Usage: Define the SLR1Parser class
 * Created on December 6, 2023
 */
#include "slr1Parser.h"

/**
 * @brief Print the item sets
 */
void SLR1Parser::printItemSets() {
  if (checkValid() != 0) {
    std::cout << "invalid" << std::endl;
    return;
  }
  std::cout << "ItemSet: " << std::endl;
  for (auto &itemSet : itemSets) {
    std::cout << "Id: " << itemSet.stateIndex << std::endl;
    for (auto t : transitions[itemSet.stateIndex]) {
      std::cout << "-- " << t.first << " -> " << t.second << std::endl;
    }

    for (auto item : itemSet.items) {
      auto p = item.str();
      if (p.second) {
        std::cout << p.first + " (reduce)" << std::endl;
        for (auto follow : followSets[item.lhs]) {
          std::cout << "  ~~" << follow << " -> reduced" << std::endl;
        }
      } else {
        std::cout << p.first << std::endl;
      }
    }
    std::cout << std::endl;
  }
}

/**
 * @brief Check if the grammar is valid
 * @return 0 if valid, 1 if shift-reduce conflict, 2 if reduce-reduce conflict
 */
int SLR1Parser::checkValid() {
  for (auto &itemSet : itemSets) {
    for (auto item : itemSet.items) {
      auto p = item.str();
      if (p.second) {
        for (auto item1 : itemSet.items) {
          if (item1 == item) {
            continue;
          }
          auto p1 = item1.str();
          if (p1.second) {
            auto follow = followSets[item.lhs];
            for (auto f : follow) {
              if (followSets[item1.lhs].find(f) !=
                  followSets[item1.lhs].end()) {
                // reduce-reduce conflict
                return 2;
              }
            }
          } else {
            auto next = item1.rhs[item1.dotPosition];
            if (g.isNonTerminal(next)) {
              if (followSets[item.lhs].find(next) !=
                  followSets[item.lhs].end()) {
                // shift-reduce conflict
                return 1;
              }
            }
          }
        }
      }
    }
  }
  return 0;
}
