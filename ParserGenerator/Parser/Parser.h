#ifndef PARSER_H
#define PARSER_H

#include "../../LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.h"
#include "ParseTree/ParseTreeNode.h"

class Definition;
using namespace std;

class Parser {
    unordered_map<const Definition *, unordered_map<string, vector<const Definition *>>> table;

public:
    explicit Parser(
        const unordered_map<const Definition *, unordered_map<string, vector<const Definition *>>>& parsingTable);

    std::shared_ptr<const ParseTreeNode> parse(LexicalAnalyzer& lexicalAnalyzer, const Definition* startSymbol,
                                               const Definition* epsilonSymbol, const string& eofString);
};
#endif //PARSER_H
