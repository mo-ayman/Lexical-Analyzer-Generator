#ifndef PARSER_H
#define PARSER_H

#include "../../LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.h"
#include "ParseTree/ParseTreeNode.h"

class Definition;
using namespace std;

class Parser {
    unordered_map<Definition *, unordered_map<string, vector<Definition *>>> table;

public:
    explicit Parser(
        const unordered_map<Definition *, unordered_map<string, vector<Definition *>>>& parsingTable);

    std::shared_ptr<const ParseTreeNode> parse(LexicalAnalyzer& lexicalAnalyzer, Definition* startSymbol);
};
#endif //PARSER_H
