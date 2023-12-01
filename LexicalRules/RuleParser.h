#ifndef RULEPARSER_H
#define RULEPARSER_H

#include <string>

#include "RuleTree.h"

namespace RuleParser {
    std::vector<RuleTree *> parse(const std::string& input);

    std::vector<RuleTree *> parseFromFile(const std::string& filename);
};

#endif //RULEPARSER_H
