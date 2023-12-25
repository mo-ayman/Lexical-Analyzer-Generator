//
// Created by rusev on 12/24/23.
//

#include <iostream>
#include "AdaptCFG.h"

AdaptCFG::AdaptCFG(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules) {
    this->rules = rules;
}

void AdaptCFG::adaptCFG() {
    eliminateLeftRecursion();
    consistensyAchieve();
    eliminateLeftFactoring();
    consistensyAchieve();
}

void AdaptCFG::consistensyAchieve() {
    std::map<std::string, Definition *> definitions_pool;
    for (auto &rule: newRules) {
        definitions_pool[rule.first->getName()] = rule.first;
    }

    for (auto &rule: newRules) {
        for (auto &rhs: rule.second) {
            for (auto &definition: rhs) {
                if (definition->getIsTerminal()) {
                    continue;
                }
                definition = definitions_pool[definition->getName()];
            }
        }
    }
}

// then it is left recursive
// we need to eliminate it
// if rule is A -> Aa | b | c
// A -> bA' | cA'
// A' -> aA' | epsilon

void AdaptCFG::eliminateLeftRecursion() {
    for (auto &rule: rules) {
        // check if rule is left recursive
        std::vector<std::vector<Definition *>> hasLeftRecursion;
        std::vector<std::vector<Definition *>> noLeftRecursion;
        for (auto &rhs: rule.second) {
            if (rhs[0]->getName() == rule.first->getName()) {
                hasLeftRecursion.emplace_back(rhs.begin() + 1, rhs.end());
            } else {
                noLeftRecursion.push_back(rhs);
            }
        }

        // Add A -> bA' | cA'
        if(hasLeftRecursion.empty()) {
            newRules[rule.first] = noLeftRecursion;
            continue;
        } else {
            for (auto &rhs: noLeftRecursion) {
                std::vector<Definition *> newRhs = rhs;
                newRhs.push_back(new Definition(rule.first->getName() + "'", false));
                newRules[rule.first].push_back(newRhs);
            }

            // Add A' -> aA' | epsilon
            auto *newDefinition = new Definition(rule.first->getName() + "'", false);
            newRules[newDefinition] = {};
            for (auto &rhs: hasLeftRecursion) {
                std::vector<Definition *> newRhs = rhs;
                newRhs.push_back(newDefinition);
                newRules[newDefinition].push_back(newRhs);
            }
            newRules[newDefinition].push_back({Definition::getEpsilon()});

        }

    }

}

// if rule is A -> aB | aC
// then it is left factored
// we need to eliminate it
// A -> aA'
// A' -> B | C

void AdaptCFG::eliminateLeftFactoring() {
    // take copy of newRules
    // because we will add new rules to newRules
    // and we will iterate over newRules
    std::map<Definition *, std::vector<std::vector<Definition *>>> newRulesCopy;
    for (auto &rule: newRules) {
        // check if rule is left factored
        std::map<std::string, std::vector<std::vector<Definition *>>> leftFactoredRules;
        std::map<std::string, bool> leftFactoredRulesMap;
        for(auto &rhs: rule.second) {
            if (leftFactoredRules.find(rhs[0]->getName()) == leftFactoredRules.end()) {
                leftFactoredRules[rhs[0]->getName()] = {};
                leftFactoredRulesMap[rhs[0]->getName()] = rhs[0]->getIsTerminal();
            }
            leftFactoredRules[rhs[0]->getName()].emplace_back(rhs.begin() + 1, rhs.end());
        }

//         print left factored rules
//        for (auto &leftFactoredRule: leftFactoredRules) {
//            std::cout << "leftFactoredRule.first: " << leftFactoredRule.first << std::endl;
//            for (auto &rhs2: leftFactoredRule.second) {
//                for (int i = 0; i < rhs2.size(); i++) {
//                    std::cout << rhs2[i]->getName() << " ";
//                }
//                std::cout << std::endl;
//            }
//        }

        for (auto &leftFactoredRule: leftFactoredRules) {
            if (leftFactoredRule.second.size() > 1) {
                auto* newDefinition = new Definition(rule.first->getName() + "'", false);
                newRulesCopy[newDefinition] = {};
                for (auto &rhs2: leftFactoredRule.second) {
                    std::vector<Definition *> newRhs(rhs2.begin(), rhs2.end());
                    if(newRhs.empty()) {
                        newRhs.push_back(Definition::getEpsilon());
                    }
                    newRulesCopy[newDefinition].push_back(newRhs);
                }
                std::vector<Definition *> newRhs = {new Definition(leftFactoredRule.first, leftFactoredRulesMap[leftFactoredRule.first]), newDefinition};
                newRulesCopy[rule.first].push_back(newRhs);
            } else {
                // rule is not left factored
                // just copy it
                std::vector<Definition *> newRhs = {new Definition(leftFactoredRule.first, leftFactoredRulesMap[leftFactoredRule.first])};
                newRhs.insert(newRhs.end(), leftFactoredRule.second[0].begin(), leftFactoredRule.second[0].end());
                newRulesCopy[rule.first].push_back(newRhs);
            }
        }
    }

    newRules = newRulesCopy;
}



std::map<Definition *, std::vector<std::vector<Definition *>>> AdaptCFG::getNewRules() const {
    return newRules;
}
