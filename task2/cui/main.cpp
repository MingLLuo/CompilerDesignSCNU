// #include "src/grammar.h"
#include "src/lr0Parser.h"
#include "src/slr1Parser.h"
int main() {
  // Pattern p("../bnf.txt");
  // p.printPatterns();
  // Grammar g("","../bnf.txt");
  // g.printInfo();
  // LR0Parser lr0Parser("", "../bnf.txt");
  // lr0Parser.g.printInfo();
  // lr0Parser.printItemSets();

  SLR1Parser slr1Parser("", "../bnf.txt");
  // slr1Parser.g.printInfo();
  slr1Parser.printItemSets();
  std::cout << slr1Parser.checkValid() << std::endl;

  // read from file
  std::ifstream file("../input");
  std::vector<std::string> input;
  std::string line;
  while (std::getline(file, line)) {
    if (line == "Token: comment" || line.empty()) {
      continue;
    }
    input.push_back(line);
  }
  auto root = slr1Parser.parse(input);
  // std::cout << slr1Parser.treeNodePrint(root, 0, "") << std::endl;

  if (root) {
    auto code = slr1Parser.generatePseudoCodeWrapper(root);
    std::cout << "Generated PseudoCode:\n" << code << std::endl;
  }
}
