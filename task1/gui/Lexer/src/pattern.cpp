/*
 * File: pattern.cpp
 * Project: Lexer
 * Author: MingLLuo
 * Usage: Define the Pattern class
 */
#include "pattern.h"
#include <string>

/* loadPatterns: read file and call readPatterns
 * @param filePath: file path
 */
void Pattern::loadPatterns(const std::string &filePath) {
  // readfile then readPatterns
  std::ifstream file(filePath);
  std::string content;
  std::string line;
  while (std::getline(file, line)) {
    content += line + "\n";
  }
  readPatterns(content);
}

/* readPatterns: read patterns from string, if 'rules' exists, read rules till
 * EOF
 * @param s: string
 */
void Pattern::readPatterns(std::string s) {
  // split to vector
  std::vector<std::string> lines;
  std::istringstream iss(s);
  std::string line;
  while (std::getline(iss, line)) {
    lines.push_back(line);
  }

  int keywordsCount = 1;
  int symbolsCount = 1;
  // for (const auto &line : lines) {
  for (int i = 0; i < lines.size(); i++) {
    auto line = lines[i];
    std::istringstream iss(line);
    std::string type, pattern;
    while (std::getline(iss, type, ' ') && std::getline(iss, pattern)) {
      // trim space front and back of pattern
      pattern.erase(0, pattern.find_first_not_of(' '));
      pattern.erase(pattern.find_last_not_of(' ') + 1);
      if (type == "keywords:") {
        // split keyword by ' '
        std::istringstream iss(pattern);
        std::string keywordPattern;
        while (std::getline(iss, keywordPattern, ' ')) {
          keywords[keywordPattern] = keywordsCount++;
        }
      } else if (type == "symbols:") {
        std::istringstream iss(pattern);
        std::string specialSymbolPattern;
        symbolsCount = keywordsCount;
        while (std::getline(iss, specialSymbolPattern, ' ')) {
          specialSymbols[specialSymbolPattern] = symbolsCount++;
        }
      } else if (type == "lcomment:") {
        lcomment = pattern;
      } else if (type == "rcomment:") {
        rcomment = pattern;
      } else if (type == "comment:") {
        comment = pattern;
      } else if (type == "identifier:") {
        idRegex = pattern;
      } else if (type == "number:") {
        numRegex = pattern;
      } else if (type == "letters:") {
        for (char c : pattern) {
          letters.push_back(c);
        }
      } else if (type == "digits:") {
        for (char c : pattern) {
          digits.push_back(c);
        }
      } else if (type == "start:") {
        start = pattern;
      }
    }
    if (type == "rules:") {
      // read to end of file
      std::string rule;
      while (i < lines.size()) {
        i++;
        rule = lines[i];
        if (rule.empty()) {
          continue;
        }
        std::string key, value;
        // key->value , split by '->'
        key = rule.substr(0,
                          rule.find("->")); // rule.substr(0, rule.find("->"));
        value = rule.substr(rule.find("->") + 2);
        // parse value by '|'
        std::istringstream issValue(value);
        std::string subValue;
        std::vector<std::string> subValues;
        while (std::getline(issValue, subValue, '|')) {
          // trim space front and back
          subValue.erase(0, subValue.find_first_not_of(' '));
          subValue.erase(subValue.find_last_not_of(' ') + 1);
          subValues.push_back(subValue);
        }
        rules[key] = subValues;
      }
    }
  }

  // if digits and letters are not defined, use default
  if (digits.empty()) {
    for (char c = '0'; c <= '9'; c++) {
      digits.push_back(c);
    }
  }
  if (letters.empty()) {
    for (char c = 'a'; c <= 'z'; c++) {
      letters.push_back(c);
    }
  }

  for (const auto &keyword : keywords) {
    allTokens.insert(keyword.first);
  }
  for (const auto &symbol : specialSymbols) {
    allTokens.insert(symbol.first);
  }
  allTokens.insert(lcomment);
  allTokens.insert(rcomment);
  allTokens.insert(comment);
  for (char c : letters) {
    allTokens.insert(std::string(1, c));
  }
  for (char c : digits) {
    allTokens.insert(std::string(1, c));
  }
}

/* printPatterns: print patterns
 */
void Pattern::printPatterns() const {
  std::cout << "Keywords: ";
  for (const auto &keyword : keywords) {
    std::cout << keyword.first << " ";
  }
  std::cout << std::endl;

  std::cout << "Special Symbols: ";
  for (const auto &symbol : specialSymbols) {
    std::cout << symbol.first << " ";
  }
  std::cout << std::endl;

  std::cout << "Left Comment: " << lcomment << std::endl;
  std::cout << "Right Comment: " << rcomment << std::endl;

  std::cout << "Rules: " << std::endl;
  for (const auto &rule : rules) {
    std::cout << rule.first << "->";
    for (const auto &subRule : rule.second) {
      std::cout << subRule << "|";
    }
    std::cout << std::endl;
  }
}

/* patternToString: return pattern in string format
 * @return: string
 */
std::string Pattern::patternToString() const {
  std::string result;
  result += "Keywords: ";
  for (const auto &keyword : keywords) {
    result += keyword.first + " ";
  }
  result += "\n";

  result += "Special Symbols: ";
  for (const auto &symbol : specialSymbols) {
    result += symbol.first + " ";
  }
  result += "\n";

  result += "Left Comment: " + lcomment + "\n";
  result += "Right Comment: " + rcomment + "\n";

  result += "Rules: \n";
  for (const auto &rule : rules) {
    result += rule.first + "->";
    for (const auto &subRule : rule.second) {
      result += subRule + "|";
    }
    result += "\n";
  }

  result += "Letters: ";
  for (char c : letters) {
    result += c;
  }
  result += "\n";

  result += "Digits: ";
  for (char c : digits) {
    result += c;
  }
  result += "\n";

  // return table, with format
  result += "Lexeme\t\tToken\n";
  int padding = 16;
  // sort by token.second
  std::map<int, std::string> tokenToLexeme;
  for (const auto &keyword : keywords) {
    tokenToLexeme[keyword.second] = keyword.first;
  }
  for (const auto &symbol : specialSymbols) {
    tokenToLexeme[symbol.second] = symbol.first;
  }
  for (const auto &token : tokenToLexeme) {
    result += token.second + std::string(padding - token.second.size(), ' ') +
              std::to_string(token.first) + "\n";
  }
  return result;
}

std::vector<std::string> Pattern::keywordsToRegScanner() const {
  std::vector<std::string> result;
  for (const auto &keyword : keywords) {
    result.push_back(keyword.first);
  }
  return result;
}

std::vector<std::string> Pattern::specialSymbolsToRegScanner() const {
  std::vector<std::string> result;
  for (const auto &symbol : specialSymbols) {
    result.push_back(symbol.first);
  }
  return result;
}

/* vectorToRegex: convert vector to regex
 * @param vec: vector
 * @return: string
 */
std::string vectorToRegex(const std::vector<char> &vec) {
  std::string result;
  if (vec.size() > 1) {
    for (int i = 0; i < vec.size(); i++) {
      result += vec[i];
      if (i != vec.size() - 1) {
        result += "|";
      }
    }
  } else {
    result = vec[0];
  }
  return result;
}

/* idRegexToRegScanner: convert idRegex to regex
 * @return: string
 */
std::string Pattern::idRegexToRegScanner() const {
  // make l -> (letters| letters ...), d -> (digits| digits ...)
  std::string result;
  std::string letters = vectorToRegex(this->letters);
  std::string digits = vectorToRegex(this->digits);
  // change idRegex's l and d to letters and digits
  for (char c : idRegex) {
    if (c == 'l') {
      result += letters;
    } else if (c == 'd') {
      result += digits;
    } else {
      result += c;
    }
  }
  return result;
}

/* numRegexToRegScanner: convert numRegex to regex
 * @return: string
 */
std::string Pattern::numRegexToRegScanner() const {
  std::string result;
  std::string letters = vectorToRegex(this->letters);
  std::string digits = vectorToRegex(this->digits);
  for (char c : numRegex) {
    if (c == 'l') {
      result += letters;
    } else if (c == 'd') {
      result += digits;
    } else {
      result += c;
    }
  }
  return result;
}

/* isTypeChar: check if char is type char
 * @param c: char
 * @return: bool
 */
bool isspace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

/* isTypeChar: check if char is type char
 * @param c: char
 * @return: bool
 */
std::string Pattern::commentRegexToRegScanner() const {
  std::string result;
  // make all char except left and right comment to vector of char (or comment)
  std::vector<char> all;
  for (int i = 0; i < 128; i++) {
    if (comment == "") {
      if (i != lcomment[0] && i != rcomment[0]) {
        if ((std::isprint(i) || isspace(i)) && !isTypeChar(i)) {
          all.push_back(i);
        }
      }
    } else {
      // if comment is not empty, add all char except comment
      if (comment.find(i) == std::string::npos) {
        if ((std::isprint(i) || isspace(i)) && !isTypeChar(i) && i != '\n') {
          all.push_back(i);
        }
      }
    }
  }
  // std::string all = letters + "|" + digits + "|" + space;
  if (lcomment == "") {
    result = comment + vectorToRegex(all) + "*";
  } else {
    result = lcomment + vectorToRegex(all) + "*" + rcomment;
  }
  return result;
}