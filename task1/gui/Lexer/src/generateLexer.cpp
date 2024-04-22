/*
 * File: generateLexer.cpp
 * Project: Lexer
 * Author: MingLLuo
 * Usage: Define the generateLexer function
 */
#include "generateLexer.h"

/* charToOut: convert char to string
 * @param c: char to convert
 * @return: converted string
 */
std::string charToOut(char c) {
  if (c == '\n') {
    return "\\n";
  } else if (c == '\t') {
    return "\\t";
  } else if (c == '\r') {
    return "\\r";
  } else if (c == '\b') {
    return "\\b";
  } else if (c == '\\') {
    return "\\\\";
  } else if (c == '"') {
    return "\\\"";
  } else if (c == '\'') {
    return "\\\'";
  } else if (c == '\f') {
    return "\\f";
  } else {
    return std::string(1, c);
  }
}

/* generateLexer: generate lexer code
 * @param lexer: lexer object
 * @return: generated code
 */
std::string generateLexer(const Lexer &lexer) {
  std::shared_ptr<DFA> dfa = lexer.finalDFA;
  std::ostringstream code;

  code << "#include <iostream>\n";
  code << "#include <string>\n";
  code << "#include <map>\n";
  code << "#include <fstream>\n";
  code << "#include <functional>\n\n";

  code << "bool isspace(char c) {\n";
  code << "    return c == ' ' || c == '\\t' || c == '\\n' || c == '\\r';\n";
  code << "}\n\n";

  std::vector<std::shared_ptr<DFAState>> sorted(dfa->dfa_states.begin(),
                                                dfa->dfa_states.end());
  std::sort(sorted.begin(), sorted.end(),
            [](const std::shared_ptr<DFAState> &a,
               const std::shared_ptr<DFAState> &b) { return a->id < b->id; });
  for (const auto &state : sorted) {
    code << "void state" << state->id
         << "(char inputSymbol, int& currentState) {\n";
    std::map<char, int> transitions;
    for (const auto &transition : state->transitions) {
      if (transition.first != 0 && transition.second != nullptr) {
        transitions[transition.first] = transition.second->id;
      }
    }
    code << "    std::map<char, int> transitions = {\n";
    for (const auto &transition : transitions) {
      code << "        {'" << charToOut(transition.first) << "', "
           << transition.second << "},\n";
    }
    code << "    };\n";
    code << "    if (transitions.find(inputSymbol) != transitions.end()) "
            "{\n";
    code << "        currentState = transitions[inputSymbol];\n";
    code << "        return;\n";
    code << "    }\n";

    code << "    currentState = -1; // Invalid transition\n";
    code << "}\n\n";
  }
  code << "static std::map<int, std::function<void(char, int&)>> "
          "stateHandlers = "
          "{\n";
  for (const auto &state : sorted) {
    code << "    {" << state->id << ", state" << state->id << "},\n";
  }
  code << "};\n\n";

  code << "int acceptInput(const std::string& input) {\n";
  code << "    int currentState = " << dfa->start_state->id << ";\n";
  code << "    for (char inputSymbol : input) {\n";
  code << "        if (isspace(inputSymbol)) continue;\n";
  code << "        if (stateHandlers.find(currentState) != "
          "stateHandlers.end()) {\n";
  code << "            stateHandlers[currentState](inputSymbol, "
          "currentState);\n";
  code << "        } else {\n";
  code << "            return 0;\n";
  code << "        }\n";
  code << "        if (currentState == -1)\n";
  code << "            return 0;\n";
  code << "    }\n\n";

  std::map<std::string, std::vector<int>> finalStates;
  for (const auto &state : sorted) {
    if (state->is_final) {
      finalStates[state->final_status].push_back(state->id);
    }
  }
  for (const auto &finalState : finalStates) {
    code << "    if (";
    for (size_t i = 0; i < finalState.second.size(); i++) {
      code << "currentState == " << finalState.second[i];
      if (i != finalState.second.size() - 1) {
        code << " || ";
      }
    }
    code << ") {\n";
    code << "        std::cout << \"Token: " << finalState.first << "\";\n";
    if (finalState.first == "num" || finalState.first == "id") {
      code << "    return 2;\n";
    }
    code << "   }\n";
  }

  code << "    return 1;\n";
  code << "}\n\n";

  code << "int main() {\n";
  code << "    std::ifstream file(\"../input.txt\");\n";
  code << "    std::string input, total;\n";
  code << "    while (std::getline(file, input)) {\n";
  code << "        total += input + \"\\n\";\n";
  code << "    }\n";
  code << "    file.close();\n";
  code << "    std::vector<std::string> tokens;\n";
  code << "    std::string token;\n";
  code << "    bool insideComment = false;\n";
  code << "    for (int i = 0; i < total.size(); i++) {\n";
  if (lexer.pattern.comment == "") {
    code << "       if (total.substr(i, " << lexer.pattern.lcomment.size()
         << ") == \"" << lexer.pattern.lcomment << "\") {\n";
    code << "           insideComment = true;\n";
    code << "           token += total.substr(i, "
         << lexer.pattern.lcomment.size() << ");\n";
    code << "           i += " << lexer.pattern.lcomment.size() - 1 << ";\n";
    code << "           continue;\n";
    code << "       }\n";
    code << "       if (total.substr(i, " << lexer.pattern.rcomment.size()
         << ") == \"" << lexer.pattern.rcomment << "\") {\n";
    code << "           insideComment = false;\n";
    code << "           token += total.substr(i, "
         << lexer.pattern.rcomment.size() << ");\n";
    code << "           i += " << lexer.pattern.rcomment.size() - 1 << ";\n";
    code << "           tokens.push_back(token);\n";
    code << "           token.clear();\n";
    code << "           continue;\n";
    code << "       }\n";

    code << "       char c = total[i];\n";
    code << "       if (isspace(c)) {\n";
    code << "           if (!token.empty() && insideComment) {\n";
    code << "               token += c;\n";
    code << "           } else if (!token.empty()) {\n";
    code << "               tokens.push_back(token);\n";
    code << "               token.clear();\n";
    code << "           }\n";
    code << "       } else if (";
    for (const auto &symbol : lexer.pattern.specialSymbols) {
      for (char c : symbol.first) {
        if (lexer.pattern.lcomment.find(c) == std::string::npos &&
            lexer.pattern.rcomment.find(c) == std::string::npos) {
          code << "c == '" << c << "' || ";
        }
      }
    }
    code << "false) {\n";
    code << "           if (insideComment) {\n";
    code << "               token += c;\n";
    code << "               continue;\n";
    code << "           }\n";
    code << "           if (!token.empty()) {\n";
    code << "               tokens.push_back(token);\n";
    code << "               token.clear();\n";
    code << "           }\n";
    code << "           tokens.push_back(std::string(1, c));\n";
    code << "       } else {\n";
    code << "           token += c;\n";
    code << "       }\n";
  } else {
    code << "       if (total.substr(i, " << lexer.pattern.comment.size()
         << ") == \"" << lexer.pattern.comment << "\") {\n";
    code << "           while (i < total.size() && total[i] != '\\n') {\n";
    code << "               token += total[i];\n";
    code << "               i++;\n";
    code << "           }\n";
    code << "           tokens.push_back(token);\n";
    code << "           token.clear();\n";
    code << "           continue;\n";
    code << "       }\n";

    code << "       char c = total[i];\n";
    code << "       if (isspace(c)) {\n";
    code << "           if (!token.empty()) {\n";
    code << "               if (token.substr(0, "
         << lexer.pattern.comment.size() << ") != \"" << lexer.pattern.comment
         << "\") {\n";
    code << "                   tokens.push_back(token);\n";
    code << "                   token.clear();\n";
    code << "               }\n";
    code << "               else {\n";
    code << "                   if (c == '\\n') {\n";
    code << "                       tokens.push_back(token);\n";
    code << "                       token.clear();\n";
    code << "                   }\n";
    code << "                   else {\n";
    code << "                       token += c;\n";
    code << "                   }\n";
    code << "               }\n";
    code << "           }\n";
    code << "       } else if (";
    for (const auto &symbol : lexer.pattern.specialSymbols) {
      for (char c : symbol.first) {
        if (lexer.pattern.comment.find(c) == std::string::npos) {
          code << "c == '" << c << "' || ";
        }
      }
    }
    code << "false) {\n";
    code << "           if (!token.empty()) {\n";
    code << "               tokens.push_back(token);\n";
    code << "               token.clear();\n";
    code << "           }\n";
    code << "           tokens.push_back(std::string(1, c));\n";
    code << "       } else {\n";
    code << "           token += c;\n";
    code << "     };\n";
  }
  code << "    }\n";
  code << "    if (token.size() > 0) {\n";
  code << "        tokens.push_back(token);\n";
  code << "    }\n";
  code << "    for (const std::string &token : tokens) {\n";
  code << "     int output = acceptInput(token);\n";
  code << "        if (output == 0) {\n";
  code << "            std::cout << \"Invalid token: \" << token << "
          "std::endl;\n";
  code << "        } else if (output == 2) {\n";
  code << "            std::cout << \" -> \" << token << std::endl;\n";
  code << "        } else {\n";
  code << "            std::cout << std::endl;\n";
  code << "        }\n";
  code << "    }\n";
  code << "    return 0;\n";
  code << "}\n";

  return code.str();
}

/* generateLexerToFile: generate lexer code and write to file
 * @param lexer: lexer object
 * @param filename: output file name
 */
void generateLexerToFile(const Lexer &lexer, const std::string &filename) {
  std::ofstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file for writing: " << filename
              << std::endl;
    return;
  }

  file << generateLexer(lexer);
  file.close();
}
