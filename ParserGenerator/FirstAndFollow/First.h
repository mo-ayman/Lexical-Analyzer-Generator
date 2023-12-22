//
// Created by rusev on 12/20/23.
//

#ifndef LEXICAL_ANALYZER_GENERATOR_FIRST_H
#define LEXICAL_ANALYZER_GENERATOR_FIRST_H


#include <map>
#include <vector>
#include <unordered_set>
#include "../Definition/Definition.h"

class First {

public:
    First();

    void constructFirst(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules);

    [[nodiscard]] std::vector<Definition *> constructFirstUtils(Definition *definition);

    void print() const;

private:
    std::map<Definition *, std::vector<std::vector<Definition *>>> rules;
    std::unordered_set<Definition*> isFirstCalculated;
    std::map<Definition *, std::vector<Definition *>> first;
};


#endif //LEXICAL_ANALYZER_GENERATOR_FIRST_H
