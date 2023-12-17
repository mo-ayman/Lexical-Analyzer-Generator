//
// Created by rusev on 12/17/23.
//

#include "ParsingCFG.h"

auto ParsingCFG::getRules() -> std::map<Definition *, std::vector<std::vector<Definition *>>> & {
    return rules;
}
