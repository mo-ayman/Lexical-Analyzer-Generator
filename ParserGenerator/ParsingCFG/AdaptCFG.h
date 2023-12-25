//
// Created by rusev on 12/24/23.
//

#ifndef LEXICAL_ANALYZER_GENERATOR_ADAPTCFG_H
#define LEXICAL_ANALYZER_GENERATOR_ADAPTCFG_H


#include <vector>
#include <map>
#include "../Definition/Definition.h"

class AdaptCFG {

public:
    AdaptCFG(std::map<Definition *, std::vector<std::vector<Definition *>>> &rules);
    void adaptCFG();

private:
    void eliminateLeftRecursion();
    void eliminateLeftFactoring();
    std::map<Definition *, std::vector<std::vector<Definition *>>> rules;
};


#endif //LEXICAL_ANALYZER_GENERATOR_ADAPTCFG_H
