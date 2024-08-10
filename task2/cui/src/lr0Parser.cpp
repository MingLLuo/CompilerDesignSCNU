/*
 * File: lr0Parser.cpp
 * Project: Parser
 * Author: MingLLuo
 * Usage: Define the LR0Parser class
 * Created on December 6, 2023
 */
#include "lr0Parser.h"

/**
 * @brief Add the first production to the grammar
 * @param g The grammar
 */
void LR0Parser::addFirstProduction(const Grammar &g) {
  std::string start;
  start = "Start";

  std::vector<std::string> rhs;
  rhs.push_back("program");
  SingleProduction startProduction = {start, rhs};
  productions.push_back(startProduction);
}

/**
 * @brief Generate the productions for the grammar
 * @param g The grammar
 */
void LR0Parser::genSingleProductions(const Grammar &g) {
  std::set<SingleProduction> checkSet;
  for (auto &p : g.productions) {
    std::string lhs = p.lhs;
    for (auto &rhs : p.rhs) {
      SingleProduction tmp = {lhs, rhs};
      if (checkSet.find(tmp) != checkSet.end()) {
        std::cerr << "Duplicate Production";
      } else {
        checkSet.insert(tmp);
        productions.push_back(tmp);
      }
    }
  }
}

/**
 * @brief Construct the item set for the grammar
 * @param start The start symbol
 */
void LR0Parser::constructItemSet(std::string start) {
  ItemSet initialSet;
  std::vector<std::string> rhs;
  rhs.push_back("program");
  LR0Item item = {start, rhs, 0, 0, false};
  initialSet.items.insert(item);

  closure(initialSet);
  itemSets.push_back(initialSet);
  std::map<ItemSet, int> itemSetMap;
  itemSetMap[initialSet] = 0;
  // start stateIndex will be 0

  bool changed = true;
  while (changed) {
    changed = false;
    for (int i = 0; i < itemSets.size(); i++) {
      for (auto symbol : symbols) {
        auto nextSet = gotoSet(itemSets[i], symbol);
        if (nextSet.items.empty()) {
          // invalid set
          continue;
        }
        if (itemSetMap.find(nextSet) == itemSetMap.end()) {
          auto index = itemSets.size();
          itemSetMap[nextSet] = index;
          nextSet.stateIndex = index;
          itemSets.push_back(nextSet);
          changed = true;
        }
        transitions[i][symbol] = itemSetMap[nextSet];
      }
    }
  }
}

/**
 * @brief Compute the closure of the item set
 * @param s The item set
 */
void LR0Parser::closure(ItemSet &s) {
  bool changed = true;
  while (changed) {
    changed = false;
    std::set<LR0Item> currentItems(
        s.items); // Create a snapshot of current items.

    for (const auto &item : currentItems) {
      if (item.dotPosition < item.rhs.size() &&
          g.isNonTerminal(item.rhs[item.dotPosition])) {
        std::string nextSymbol = item.rhs[item.dotPosition];
        // Add new items for each production of the non-terminal following the
        // dot.
        for (const auto &prod : productions) {
          if (prod.lhs == nextSymbol) {
            LR0Item newItem{prod.lhs, prod.rhs, 0, 0, false};
            // Add new item only if it's not already present.
            if (s.items.insert(newItem).second) {
              changed = true;
            }
          }
        }
      }
    }
  }
}

/**
 * @brief Compute the goto set for the item set
 * @param s The item set
 * @param symbol The symbol to process
 * @return The next item set
 */
ItemSet LR0Parser::gotoSet(ItemSet &s, std::string symbol) {
  ItemSet nextSet;
  for (const auto &item : s.items) {
    // Check if the dot is just before the symbol we are processing
    if (item.dotPosition < item.rhs.size() &&
        item.rhs[item.dotPosition] == symbol) {
      // Move the dot past the symbol
      LR0Item newItem{item.lhs, item.rhs, item.productionIndex,
                      item.dotPosition + 1, false};
      nextSet.items.insert(newItem);

      // If the symbol after the dot is a non-terminal, generate its items
      // immediately
      if (newItem.dotPosition < newItem.rhs.size() &&
          g.isNonTerminal(newItem.rhs[newItem.dotPosition])) {
        std::string nextNonTerminal = newItem.rhs[newItem.dotPosition];
        for (const auto &prod : productions) {
          if (prod.lhs == nextNonTerminal) {
            nextSet.items.insert({prod.lhs, prod.rhs, 0, 0, false});
          }
        }
      }
    }
  }
  // Compute the closure of the new item set to include all items necessary for
  // the next state
  closure(nextSet);
  return nextSet;
}

/**
 * @brief Print the item sets
 */
void LR0Parser::printItemSets() {
  std::cout << "ItemSet: " << std::endl;
  for (auto &itemSet : itemSets) {
    std::cout << "Id: " << itemSet.stateIndex << std::endl;
    for (auto item : itemSet.items) {
      auto p = item.str();
      if (p.second) {
        std::cout << p.first + " (reduce)" << std::endl;
      } else {
        std::cout << p.first << std::endl;
      }
    }
    std::cout << std::endl;
  }
}

/**
 * @brief Convert a string token to a Token object
 * @param tokenStr The string token
 * @return The Token object
 */
Token LR0Parser::stringToToken(std::string tokenStr) {
  // skip string Token:
  tokenStr = tokenStr.substr(7);

  // check if tokenStr[:x] is id or num
  if (tokenStr.substr(0, 2) == "id") {
    // clean the "id -> "
    tokenStr = tokenStr.substr(6);
    return Token{"identifier", tokenStr};
  } else if (tokenStr.substr(0, 3) == "num") {
    // clean the "num -> "
    tokenStr = tokenStr.substr(7);
    return Token{"number", tokenStr};
  } else {
    return Token{tokenStr, ""};
  }
}

/**
 * @brief Parse the input tokens
 * @param tokens The input tokens
 * @return The parse tree
 */
std::shared_ptr<TreeNode> LR0Parser::parse(std::vector<std::string> tokens) {
  std::vector<Token> tokenList = {};
  // tokenList.insert(tokenList.begin(), Token{"program", ""});
  for (auto &tokenStr : tokens) {
    tokenList.push_back(stringToToken(tokenStr));
  }

  std::stack<std::pair<int, std::shared_ptr<TreeNode>>> parseStack;
  auto startNode = std::make_shared<TreeNode>("program");

  parseStack.push({startState, startNode}); // Push the initial state
  std::cout << "Start parsing, initial state: " << startState << std::endl;

  int idx = 0;
  while (idx < tokenList.size() || parseStack.size() > 2) {
    int currentState = parseStack.top().first;
    Token currentToken;
    if (idx < tokenList.size()) {
      currentToken = tokenList[idx];
    } else {
      currentToken = Token{"$", ""};
    }

    // --- DEBUG ---
    {
      std::cout << "Current State: " << currentState
                << ", Current Token: " << currentToken.type << " ('"
                << currentToken.value << "')" << std::endl;
      std::cout << "Stack: \n";
      std::stack<std::pair<int, std::shared_ptr<TreeNode>>>
          tempStack; // temp stack to print the stack
      while (!parseStack.empty()) {
        tempStack.push(parseStack.top());
        parseStack.pop();
      }
      while (!tempStack.empty()) {
        parseStack.push(tempStack.top());
        std::cout << tempStack.top().first;
        if (tempStack.top().second != nullptr) {
          std::cout << " (" << tempStack.top().second->value << ")";
        }
        std::cout << std::endl;
        tempStack.pop();
      }
      std::cout << std::endl;
      // print transitions
      std::cout << "Transitions: " << std::endl;
      for (auto t : transitions[currentState]) {
        std::cout << "-- " << t.first << " -> " << t.second << std::endl;
      }
    }
    // --- DEBUG ---

    // check if the current state has a shift action
    if (transitions[currentState].find(currentToken.type) !=
        transitions[currentState].end()) {
      int nextState = transitions[currentState][currentToken.type];
      std::cout << "**Action: Shift " << nextState << std::endl;
      std::string value = currentToken.value;
      if (value == "") {
        value = currentToken.type;
      } else {
        value = currentToken.type + " -> " + value;
      }
      parseStack.push({nextState, std::make_shared<TreeNode>(value)});
      idx++;
    } else {
      bool reduced = false;
      for (auto &itemSet : itemSets) {
        if (itemSet.stateIndex != currentState) {
          continue;
        }
        for (auto item : itemSet.items) {
          auto production = item.str();
          if (item.accept) {
            reduced = true;
            std::cout << "**Action: Accept" << std::endl;
            std::cout << parseStack.size() << std::endl;
            break;
          }
          if (production.second) {
            // perform reduce
            std::cout << "**Action: Reduce " << production.first << std::endl;

            std::vector<std::shared_ptr<TreeNode>> children;
            for (int i = 0; i < item.rhs.size(); i++) {
              children.push_back(parseStack.top().second);
              parseStack.pop();
            }
            std::reverse(children.begin(), children.end());
            auto newNode = std::make_shared<TreeNode>(item.lhs);
            newNode->children = children;
            int nextState = parseStack.top().first;
            if (transitions[nextState].find(item.lhs) !=
                transitions[nextState].end()) {
              parseStack.push({transitions[nextState][item.lhs], newNode});
              reduced = true;
              break;
            }
          }
        }
      }
      if (!reduced) {
        std::cerr << "Invalid Input" << std::endl;
        return nullptr;
      }
    }
    std::cout << std::endl;
  }

  if (parseStack.size() == 2) {
    std::cout << "Parsing completed successfully." << std::endl;
    return parseStack.top().second;
  } else {
    std::cerr << "Error: Parsing did not revert to initial state correctly, "
                 "input may be incomplete."
              << std::endl;
    std::cout << "Stack: \n";
    std::stack<std::pair<int, std::shared_ptr<TreeNode>>>
        tempStack; // temp stack to print the stack
    while (!parseStack.empty()) {
      tempStack.push(parseStack.top());
      parseStack.pop();
    }
    while (!tempStack.empty()) {
      parseStack.push(tempStack.top());
      std::cout << tempStack.top().first;
      if (tempStack.top().second != nullptr) {
        std::cout << " (" << tempStack.top().second->value << ")";
      }
      std::cout << std::endl;
      tempStack.pop();
    }
    return nullptr;
  }
}

/**
 * @brief Print the parse tree
 * @param root The root of the parse tree
 * @param depth The depth of the tree
 * @param prefix The prefix for the tree
 * @return The string representation of the tree
 */
std::string LR0Parser::treeNodePrint(std::shared_ptr<TreeNode> root, int depth,
                                     std::string prefix) {
  std::stringstream ss;
  if (root == nullptr) {
    return "";
  }

  ss << prefix;

  if (depth > 0) {
    ss << "|-";
  }

  ss << root->value << std::endl;
  for (int i = 0; i < root->children.size(); i++) {
    if (i == root->children.size() - 1) {
      ss << treeNodePrint(root->children[i], depth + 1, prefix + "  ");
    } else {
      ss << treeNodePrint(root->children[i], depth + 1, prefix + "| ");
    }
  }

  return ss.str();
}

void LR0Parser::generatePseudoCode(std::shared_ptr<TreeNode> &node,
                                   std::vector<std::string> &codeList,
                                   int &tempVarCounter, int indentLevel) {
  if (!node)
    return;

  std::string currentLine;
  std::vector<std::string> childResults;
  if (node->children.size() == 1) {
    node = node->children[0];
  }
  if (!node->children.empty()) {
    // Add indent
    std::string tempVar = "t" + std::to_string(tempVarCounter++);
    currentLine = std::string((indentLevel > 1? (indentLevel * 2) : 2), ' ') + tempVar + "(" + node->value + "):= ";
    codeList.push_back(currentLine);
    int flag = codeList.size() - 1;

    // Now output the current node's assignment to its parent
    if (indentLevel == 0) {
      codeList.push_back(node->value + " = " + tempVar);
    } else {
      codeList.push_back(std::string((indentLevel - 1) * 2, ' ') + node->value +
                         " = " + tempVar);
    }
      for (auto &child : node->children) {
          if (child->children.empty()) {
              childResults.push_back(child->value);
          } else {
              tempVar = "t" + std::to_string(tempVarCounter++);
              generatePseudoCode(child, codeList, tempVarCounter, indentLevel + 1);
              childResults.push_back(tempVar);
          }
      }
      for (size_t i = 0; i < childResults.size(); ++i) {
          currentLine += childResults[i];
          if (i != childResults.size() - 1) {
              currentLine += " ";
          }
      }
      codeList[flag] = codeList[flag+1];
      codeList[flag + 1] = currentLine;
  }
}

std::string
LR0Parser::generatePseudoCodeWrapper(std::shared_ptr<TreeNode> root) {
  std::vector<std::string> codeList;
  int tempVarCounter = 1;
  int indentLevel = 0;

  generatePseudoCode(root, codeList, tempVarCounter, indentLevel);

  std::stringstream ss;
  for (const auto &line : codeList) {
    ss << line << std::endl;
  }
  return ss.str();
}

// GPU MODE!!! (beta)
// std::string LR0Parser::treeNodePrint(std::shared_ptr<TreeNode> root, int
// depth,
//                                      std::string prefix) {
//   std::stringstream ss;
//   if (root == nullptr) {
//     return "";
//   }

//   if (!g.isNonTerminal(root->value)) {
//     // only print the value
//     // prefix += "|- ";
//     auto nextValue = this->g.pattern.isKeyword(root->value)
//                          ? "<" + root->value + ">"
//                          : root->value;
//     ss << prefix << nextValue << std::endl;
//   } else {
//     // first op
//     int op_flag = -1;
//     for (int i = 0; i < root->children.size(); i++) {
//       if (root->children[i]->value.find("op") != std::string::npos) {
//         op_flag = i;
//         break;
//       }
//     }

//     auto child0 = root->children[0];
//     if (root->value.find("stmt") != std::string::npos &&
//         this->g.pattern.isKeyword(child0->value)) {
//       // give indent for i = 1 to end
//       ss << treeNodePrint(child0, 0, prefix);
//       prefix += "| ";

//       if (op_flag != -1) {
//         ss << treeNodePrint(root->children[op_flag], 0, prefix + " ");
//         prefix += " ..";
//       }
//     } else {
//       // give indent for i = 0 to end
//       if (op_flag != -1) {
//         ss << treeNodePrint(root->children[op_flag], 0, prefix + " ");
//         prefix += " ..";
//       }
//       ss << treeNodePrint(child0, 0, prefix);
//     }

//     for (int i = 1; i < root->children.size(); i++) {
//       if (i == op_flag) {
//         continue;
//       }
//       ss << treeNodePrint(root->children[i], 0, prefix);
//     }
//   }

//   return ss.str();
// }