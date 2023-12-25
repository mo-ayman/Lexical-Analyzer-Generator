
#ifndef LEXICAL_ANALYZER_GENERATOR_ADAPTCFG_H
#define LEXICAL_ANALYZER_GENERATOR_ADAPTCFG_H


#include <vector>
#include <map>
#include "../Definition/Definition.h"

class AdaptCFG {

public:
    AdaptCFG(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules);
    void adaptCFG();

    [[nodiscard]] std::map<Definition *, std::vector<std::vector<Definition *>>> getNewRules() const;


private:
    void eliminateLeftRecursion();
    void eliminateLeftFactoring();
    void consistensyAchieve();
    std::map<Definition *, std::vector<std::vector<Definition *>>> rules;
    std::map<Definition *, std::vector<std::vector<Definition *>>> newRules;
    
};


#endif //LEXICAL_ANALYZER_GENERATOR_ADAPTCFG_H
