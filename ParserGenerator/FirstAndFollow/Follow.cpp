//
// Created by rusev on 12/20/23.
//
#include <algorithm>

#include <iostream>
#include "Follow.h"


Follow::Follow() {
    rules = std::map<Definition *, std::vector<std::vector<Definition *>>>();
    first = std::map<Definition *, std::vector<Definition *>>();
    follow = std::map<Definition *, std::vector<Definition *>>();
    isFollowCalculated = std::unordered_set<Definition *>();

}

void Follow::constructFollow(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules,
                             const std::map<Definition *, std::vector<Definition *>> &first) {
    this->first = first;
    this->rules = rules;


    for (const auto &rule : rules) {
        constructFollowUtils(rule.first);
    }

}

std::vector<Definition *> Follow::constructFollowUtils(Definition *definition) {
    if (definition->getIsTerminal()) {
        return std::vector<Definition *>{definition};
    }

    if (isFollowCalculated.find(definition) != isFollowCalculated.end()) {
        return follow[definition];
    }

    std::unordered_set<std::string> visited; // remove dup
    for (const auto &rule : rules) {
        for (const auto &def : rule.second) {
            for (int i = 0; i < def.size(); i++) {
                if (*def[i] == *definition) {
                    if (i == def.size() - 1) {
                        if (!(*(rule.first) == *definition)) {
                            std::vector<Definition *> result = constructFollowUtils(rule.first);
                            for (auto r : result) {
                                // use visited to remove dup
                                if (visited.find(r->getName()) == visited.end()) {
                                    follow[definition].push_back(r);
                                    visited.insert(r->getName());
                                }
                            }
                        }
                    } else {
                        std::vector<Definition *> result = first[def[i + 1]];
                        for (auto r : result) {
                            if (visited.find(r->getName()) == visited.end()) {
                                follow[definition].push_back(r);
                                visited.insert(r->getName());
                            }
                        }
                        if (std::find(result.begin(), result.end(), Definition::getEpsilon()) != result.end()) {
                            if (!(*(rule.first) == *definition)) {
                                std::vector<Definition *> result = constructFollowUtils(rule.first);
                                for (auto r : result) {
                                    if (visited.find(r->getName()) == visited.end()) {
                                        follow[definition].push_back(r);
                                        visited.insert(r->getName());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    isFollowCalculated.insert(definition);
    return follow[definition];

}

void Follow::print() const {
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
    std::cout << std::endl;
    for (auto f : follow) {
        std::cout << f.first->getName() << " : ";
        for (auto d : f.second) {
            std::cout << d->getName() << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    for (auto f : first) {
        std::cout << f.first->getName() << " : ";
        for (auto d : f.second) {
            std::cout << d->getName() << " ";
        }
        std::cout << std::endl;
    }
}
