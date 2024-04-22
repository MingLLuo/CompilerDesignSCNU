/*
 * File: slr1Parser.h
 * Project: Parser
 * Author: MingLLuo
 * Usage: Define the SLR1Parser class
 * Created on December 6, 2023
 */
#ifndef SLRPARSER_SLR1PARSER_H
#define SLRPARSER_SLR1PARSER_H
#include "lr0Parser.h"

class SLR1Parser : public LR0Parser {
public:
  SLR1Parser(const std::string &s, const std::string &patternFile)
      : LR0Parser(s, patternFile) {
    followSets = g.followSets;
  }

  void printItemSets();

  int checkValid();
  std::unordered_map<std::string, std::unordered_set<std::string>>  followSets;
};

#endif // SLRPARSER_SLR1PARSER_H
