//
// Created by rusev on 12/17/23.
//

#include "ParsingCFG.h"

auto ParsingCFG::getRules() -> std::map<Node *, std::vector<std::vector<Node *>>> & {
    return rules;
}
