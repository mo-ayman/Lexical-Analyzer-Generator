//
// Created by rusev on 12/20/23.
//

#ifndef LEXICAL_ANALYZER_GENERATOR_FOLLOW_H
#define LEXICAL_ANALYZER_GENERATOR_FOLLOW_H


#include <vector>
#include <unordered_set>
#include <map>
#include "../Definition/Definition.h"
#include "First.h"

class Follow {
public:
    Follow(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules,
           First* first1);

    void constructFollow();

    std::vector<Definition *> getFollow(Definition *definition);

    void print() const;


private:
    std::map<Definition *, std::vector<std::vector<Definition *>>> rules;
    First* first{};
    std::map<Definition *, std::vector<Definition *>> follow;
    std::unordered_set<Definition *> isFollowCalculated;

};


#endif //LEXICAL_ANALYZER_GENERATOR_FOLLOW_H
