
#ifndef PARSER_GENERATOR_PARSINGCFG_H
#define PARSER_GENERATOR_PARSINGCFG_H


#include <string>
#include <map>
#include <vector>
#include "../Definition.h"

class ParsingCFG {
private:
    std::map<Definition* , std::vector<std::vector<Definition*>>> rules;

public:
    auto getRules() -> std::map<Definition* , std::vector<std::vector<Definition*>>>&;

};


#endif //PARSER_GENERATOR_PARSINGCFG_H
