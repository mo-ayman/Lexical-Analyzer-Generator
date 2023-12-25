//
// Created by rusev on 12/24/23.
//

#include "AdaptCFG.h"

AdaptCFG::AdaptCFG(std::map<Definition *, std::vector<std::vector<Definition *>>> &rules) {
    this->rules = rules;
}

void AdaptCFG::adaptCFG() {
    eliminateLeftRecursion();
    eliminateLeftFactoring();
}

void AdaptCFG::eliminateLeftRecursion() {
    std::map<Definition *, std::vector<std::vector<Definition *>>> newRules;
    for (const auto &rule : rules) {
        std::vector<std::vector<Definition *>> alpha;
        std::vector<std::vector<Definition *>> beta;
        for (const auto &def : rule.second) {
            if (def[0]->getName() == rule.first->getName()) {
                alpha.push_back(std::vector<Definition *>(def.begin() + 1, def.end()));
            } else {
                beta.push_back(def);
            }
        }
        if (alpha.empty()) {
            newRules[rule.first] = beta;
        } else {
            Definition *newDef = new Definition(rule.first->getName() + "'");
            newRules[rule.first] = beta;
            newRules[newDef] = alpha;
            for (const auto &def : beta) {
                std::vector<Definition *> newDefVector = def;
                newDefVector.push_back(newDef);
                newRules[newDefVector.back()].push_back(newDefVector);
            }
        }
    }
    rules = newRules;
}

void AdaptCFG::eliminateLeftFactoring() {
    std::map<Definition *, std::vector<std::vector<Definition *>>> newRules;
    for (const auto &rule : rules) {
        std::vector<std::vector<Definition *>> alpha;
        std::vector<std::vector<Definition *>> beta;
        std::vector<std::vector<Definition *>> newBeta;
        for (const auto &def : rule.second) {
            if (def.empty()) {
                alpha.push_back(def);
            } else if (def[0]->getName() == rule.first->getName()) {
                alpha.push_back(std::vector<Definition *>(def.begin() + 1, def.end()));
            } else {
                beta.push_back(def);
            }
        }
        if (beta.empty()) {
            newRules[rule.first] = alpha;
        } else {
            for (const auto &def : beta) {
                if (def.empty()) {
                    newBeta.push_back(def);
                } else if (def[0]->getName() == rule.first->getName()) {
                    newBeta.push_back(std::vector<Definition *>(def.begin() + 1, def.end()));
                } else {
                    newBeta.push_back(def);
                }
            }
            Definition *newDef = new Definition(rule.first->getName() + "'");
            newRules[rule.first] = alpha;
            newRules[newDef] = newBeta;
            for (const auto &def : alpha) {
                std::vector<Definition *> newDefVector = def;
                newDefVector.push_back(newDef);
                newRules[newDefVector.back()].push_back(newDefVector);
            }
        }
    }
    rules = newRules;
}
