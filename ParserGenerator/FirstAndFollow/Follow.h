//
// Created by rusev on 12/20/23.
//

#ifndef LEXICAL_ANALYZER_GENERATOR_FOLLOW_H
#define LEXICAL_ANALYZER_GENERATOR_FOLLOW_H


#include <vector>
#include <unordered_set>
#include <map>
#include "../Definition/Definition.h"

class Follow {
public:
    Follow();

    void constructFollow(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules
    , const std::map<Definition *, std::vector<Definition *>> &first);

    [[nodiscard]] std::vector<Definition *> constructFollowUtils(Definition *definition);

    void print() const;


private:
    std::map<Definition *, std::vector<std::vector<Definition *>>> rules;
    std::map<Definition *, std::vector<Definition *>> first;
    std::map<Definition *, std::vector<Definition *>> follow;
    std::unordered_set<Definition *> isFollowCalculated;

};


#endif //LEXICAL_ANALYZER_GENERATOR_FOLLOW_H
