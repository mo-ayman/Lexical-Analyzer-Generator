
#ifndef PARSER_GENERATOR_PARSINGCFG_H
#define PARSER_GENERATOR_PARSINGCFG_H


#include <string>
#include <map>
#include <vector>

class ParsingCFG {
private:
    class Node {
        std::string name;
        [[maybe_unused]] bool isTerminal;
    };

    std::map<Node* , std::vector<std::vector<Node*>>> rules;


public:

    auto getRules() -> std::map<Node* , std::vector<std::vector<Node*>>>&;


};


#endif //PARSER_GENERATOR_PARSINGCFG_H
