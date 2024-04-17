#include "src/generateLexer.h"
#include "src/lexer.h"
int main() {
    Lexer lexer("", "../patterns.txt");

    Pattern &pattern = lexer.pattern;
    // pattern.printPatterns();
    // std::cout << pattern.patternToString() << std::endl;
    // for (const auto &keyword : pattern.keywordsToRegScanner()) {
    //     std::cout << keyword << std::endl;
    // }
    // for (const auto &symbol : pattern.specialSymbolsToRegScanner()) {
    //     std::cout << symbol << std::endl;
    // }
    // std::cout << pattern.idRegexToRegScanner() << std::endl;
    // std::cout << pattern.numRegexToRegScanner() << std::endl;
    // std::cout << pattern.commentRegexToRegScanner() << std::endl;

    // keywords DFA
    // keywords: if then else end repeat until read write
    DFA *keywordDFA = lexer.dfas["keyword"].get();
    std::vector<std::string> testSuite = {"if",     "then",  "else", "end",
                                          "repeat", "until", "read", "write"};

    // for (const auto &test : testSuite) {
    //     keywordDFA->acceptString(test);
    // }

    // symbols DFA
    // symbols:  + – * / % < <> <= >= > = { } ; :=
    // DFA *symbolDFA = lexer.dfas["symbol"].get();
    // testSuite = {"+",  "-", "*", "/", "%", "<", "<>", "<=",
    //              ">=", ">", "=", "{", "}", ";", ":="};
    // for (const auto &test : testSuite) {
    //     symbolDFA->acceptString(test);
    // }
    // symbolDFA->printDFA();

    // comment DFA
    // DFA *commentDFA = lexer.dfas["comment"].get();
    // commentDFA->printDFA();
    // testSuite = {"{123}", "{123", "123}", "{123", "123}"};
    // for (const auto &test : testSuite) {
    //     commentDFA->acceptString(test);
    // }

    // id DFA
    // DFA *idDFA = lexer.dfas["id"].get();
    // testSuite = {"_abcd", "abcd", "1234", "a1234", "1234a", "a1234a"};
    // for (const auto &test : testSuite) {
    //     idDFA->acceptString(test);
    // }

    // num DFA
    // DFA *numDFA = lexer.dfas["num"].get();
    // testSuite = {"1234",           "1234.1234", ".1234",      "1234.",
    //              "1234.1234e1234", "1234e1234", "1234e+1234", "1234e-1234"};
    // for (const auto &test : testSuite) {
    //     numDFA->acceptString(test);
    // }

    // test Final DFA
    DFA *finalDFA = lexer.finalDFA.get();
    testSuite = {"if",         "then",
                 "else",       "end",
                 "repeat",     "until",
                 "read",       "write",
                 "+",          "-",
                 "*",          "/",
                 "%",          "<",
                 "<>",         "<=",
                 ">=",         ">",
                 "=",          "{",
                 "}",          ";",
                 ":=",         "_abcd",
                 "abcd",       "1234",
                 "a1234",      "1234a",
                 "a1234a",     "1234",
                 "1234.1234",  ".1234",
                 "1234.",      "1234.1234e1234",
                 "1234e1234",  "1234e+1234",
                 "1234e-1234", "{123}",
                 "{123",       "123}",
                 "{123",       "123}"};
    for (const auto &test : testSuite) {
        finalDFA->acceptString(test);
    }
    finalDFA->printStatus();
    generateLexerToFile(lexer, "lexer.cpp");
    return 0;
}