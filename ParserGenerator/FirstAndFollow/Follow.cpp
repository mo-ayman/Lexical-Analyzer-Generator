//
// Created by rusev on 12/20/23.
//
#include <algorithm>

#include <iostream>
#include "Follow.h"
#include "First.h"


Follow::Follow(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules,
               First* first) {
    this->rules = rules;
    this->first = first;
    follow = std::map<Definition *, std::vector<Definition *>>();
    isFollowCalculated = std::unordered_set<Definition *>();
}

void Follow::constructFollow() {

    follow[rules.begin()->first].push_back(Definition::getDollar());

    for (const auto &rule : rules) {
//        std::cout << "constructFollow: " << rule.first->getName() << std::endl;
        getFollow(rule.first);
    }

}

std::vector<Definition *> Follow::getFollow(Definition *definition) {
//    std::cout << "constructFollowUtils: " << definition->getName() << std::endl;
    if (definition->getIsTerminal()) {
        return std::vector<Definition *>{definition};
    }

    if (isFollowCalculated.find(definition) != isFollowCalculated.end()) {
        return follow[definition];
    }

    if(inStack.find(definition) != inStack.end()) {
//        inStack.erase(definition);
        return {};
    }
    inStack.insert(definition);

    std::unordered_set<std::string> visited; // remove dup
    for (const auto &rule : rules) {
        for (const auto &def : rule.second) {
            for (int i = 0; i < def.size(); i++) {
                if (*def[i] == *definition) {
                    if (i == def.size() - 1) {
                        if (!(*(rule.first) == *definition)) {
                            std::vector<Definition *> result = getFollow(rule.first);
                            for (auto r : result) {
                                // use visited to remove dup
                                if (visited.find(r->getName()) == visited.end()) {
                                    follow[definition].push_back(r);
                                    visited.insert(r->getName());
                                }
                            }
                        }
                    } else {
                        auto result = first->getFirst(def[i + 1]);
                        bool hasEpsilon = false;
                        for (auto r : result) {
                            if(r.second->getName() == Definition::getEpsilon()->getName()) {
//                                std::cout << "epsilon found" << std::endl;
                                hasEpsilon = true;
                                continue;
                            }
                            if (visited.find(r.second->getName()) == visited.end()) {
                                follow[definition].push_back(r.second);
                                visited.insert(r.second->getName());
                            }
                        }

                        if (hasEpsilon) {
//                            std::cout << "epsilon found" << std::endl;
                            if (!(*(rule.first) == *definition)) {
                                std::vector<Definition *> result = getFollow(rule.first);
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
    if(!follow[definition].empty()) {
        isFollowCalculated.insert(definition);
    }
    inStack.erase(definition);
    return follow[definition];

}

void Follow::print() const {
    std::cout << "====================== Follow ======================" << std::endl;
    for (auto rule : rules) {
        std::cout << rule.first->getName() << " : ";
        auto f = follow.find(rule.first);
        for (auto d : f->second) {
            if(d->getIsTerminal()) {
                std::cout << "'" << d->getName() << "' ";
            }
            else {
                throw std::runtime_error("Follow is not terminal");
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

}

std::map<Definition *, std::vector<Definition *>> Follow::getFollow() {
    return follow;
}
