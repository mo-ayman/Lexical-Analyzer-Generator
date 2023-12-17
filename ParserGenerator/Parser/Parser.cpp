#include "Parser.h"

#include <stack>

#include "../Definition/Definition.h"


Parser::Parser(const unordered_map<Definition, unordered_map<string, vector<Definition>>>& parsingTable)
    : table(parsingTable) {}

ParseTreeNode Parser::parse(LexicalAnalyzer* lexicalAnalyzer, const Definition& startSymbol) {
    stack<Definition> S;
    S.push(startSymbol);
    //TODO:
}



