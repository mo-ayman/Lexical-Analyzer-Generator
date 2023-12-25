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
    first = std::map<Definition *, std::vector<std::pair<int, Definition *>>>();

}

void First::constructFirst() {
    // size

    for (const auto& rule : rules) {
        getFirst(rule.first);
    }
}

std::vector<std::pair<int, Definition *>> First::getFirst(Definition *definition) {
//    std::cout << "constructFirstUtils: " << definition->getName() << std::endl;
    if (definition->getIsTerminal()) {
        return std::vector<std::pair<int, Definition *>>{std::make_pair(0, definition)};
    }

    if(isFirstCalculated.find(definition) != isFirstCalculated.end()) {
        return first[definition];
    }
    else {
        std::unordered_set<std::string> visited; // remove dup
        for (int i = 0; i < rules[definition].size(); ++i) {
            if(definition->getName() == rules[definition][i][0]->getName()) {
                continue;
            }

            auto result = getFirst(rules[definition][i][0]);
            for (auto r : result) {
                if (visited.find(r.second->getName()) == visited.end()) {
                    first[definition].emplace_back(i, r.second);
                    visited.insert(r.second->getName());
                }
            }
        }
    }

    isFirstCalculated.insert(definition);
    return first[definition];

}


void First::print() const {

    std::cout << "====================== First ======================" << std::endl;
    // size
//    std::cout << "first.size(): " << rules.size() << std::endl;
    for (const auto& rule: rules) {
        std::cout << rule.first->getName() << " -> ";
        auto f = first.find(rule.first);
        if (f != first.end()) {
            for (auto def: f->second) {
                if (def.second->getIsTerminal()) {
                    std::cout << "'" << def.second->getName() << "' ";
                } else {
                    std::cout << def.second->getName() << " ";
                }
            }
        }
        std::cout << std::endl;
    }
}

std::map<Definition *, std::vector<std::pair<int, Definition *>>> First::getFirst() {
    return first;
}

