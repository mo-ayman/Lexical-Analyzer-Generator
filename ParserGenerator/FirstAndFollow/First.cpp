//
// Created by rusev on 12/20/23.
//

#include <algorithm>
#include <iostream>
#include "First.h"
#include <unordered_set>


First::First(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules) {
    this->rules = rules;
    isFirstCalculated = std::unordered_set<Definition*>();
    first = std::map<Definition *, std::vector<Definition *>>();

}

void First::constructFirst() {
    for (const auto& rule : rules) {
        getFirst(rule.first);
    }

}

std::vector<Definition *> First::getFirst(Definition *definition) {
//    std::cout << "constructFirstUtils: " << definition->getName() << std::endl;
    if (definition->getIsTerminal()) {
        return std::vector<Definition *>{definition};
    }

    if(isFirstCalculated.find(definition) != isFirstCalculated.end()) {
        return first[definition];
    }
    else {
        std::unordered_set<std::string> visited; // remove dup
        for (auto def : rules[definition]) {
            if(definition == def[0]) continue;

            std::vector<Definition *> result = getFirst(def[0]);
            for (auto r : result) {
                if (visited.find(r->getName()) == visited.end()) {
                    first[definition].push_back(r);
                    visited.insert(r->getName());
                }
            }
        }
    }

    isFirstCalculated.insert(definition);
    return first[definition];

}


void First::print() const {
    std::cout << "====================== First ======================" << std::endl;
    for (const auto& rule: rules) {
        std::cout << rule.first->getName() << " -> ";
        auto f = first.find(rule.first);
        if (f != first.end()) {
            for (auto def: f->second) {
                if (def->getIsTerminal()) {
                    std::cout << "'" << def->getName() << "' ";
                } else {
                    throw std::runtime_error("First is not terminal");
                }
            }
        }
        std::cout << std::endl;
    }
}

std::map<Definition *, std::vector<Definition *>> First::getFirst() {
    return first;
}


