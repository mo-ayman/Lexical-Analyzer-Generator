//
// Created by rusev on 12/20/23.
//

#include <algorithm>
#include <iostream>
#include "First.h"
#include <unordered_set>


First::First() {
    rules = std::map<Definition *, std::vector<std::vector<Definition *>>>();
    isFirstCalculated = std::unordered_set<Definition*>();
    first = std::map<Definition *, std::vector<Definition *>>();
}

void First::constructFirst(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules) {
    this->rules = rules;

    for (const auto& rule : rules) {
        constructFirstUtils(rule.first);
    }

}

std::vector<Definition *> First::constructFirstUtils(Definition *definition) {
    if (definition->getIsTerminal()) {
        return std::vector<Definition *>{definition};
    }

    if(isFirstCalculated.find(definition) != isFirstCalculated.end()) {
        return first[definition];
    }
    else {
        std::unordered_set<std::string> visited; // remove dup
        for (auto def : rules[definition]) {
            std::vector<Definition *> result = constructFirstUtils(def[0]);
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
    for (auto rule : rules) {
        std::cout << rule.first->getName() << " : ";
        for (auto def : rule.second) {
            for (auto d : def) {
                std::cout << d->getName() << " ";
            }
            std::cout << "| ";
        }
        std::cout << std::endl;
    }
}


