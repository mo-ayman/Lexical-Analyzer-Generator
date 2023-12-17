#ifndef PARSER_H
#define PARSER_H
#include "../../LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.h"
#include "ParseTree/ParseTreeNode.h"

class Definition;
using namespace std;

class Parser {
    unordered_map<Definition, unordered_map<string, vector<Definition>>> table;
public:
    Parser(const unordered_map<Definition, unordered_map<string, vector<Definition>>>& parsingTable);
    static ParseTreeNode parse(LexicalAnalyzer* lexicalAnalyzer, const Definition& startSymbol);
    static ParseTreeNode parse(LexicalAnalyzer* lexicalAnalyzer, const Definition& startSymbol, const std::string& outputFilePath);

};

#endif //PARSER_H
