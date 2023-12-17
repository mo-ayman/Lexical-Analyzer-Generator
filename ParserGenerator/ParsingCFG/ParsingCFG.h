
#ifndef PARSER_GENERATOR_PARSINGCFG_H
#define PARSER_GENERATOR_PARSINGCFG_H


#include <string>
#include <map>
#include <vector>

class ParsingCFG {
private:
    class Definition {
        std::string name;
        bool isTerminal;
    };

    std::map<Definition* , std::vector<std::vector<Definition*>>> rules;


public:

    auto getRules() -> std::map<Definition* , std::vector<std::vector<Definition*>>>&;


};


#endif //PARSER_GENERATOR_PARSINGCFG_H
