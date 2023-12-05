#ifndef HELPINGMETHODS_H
#define HELPINGMETHODS_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>

#include "../LexicalRules/RuleTree.h"

namespace HelpingMethods {
    void printSet(const std::set<int>& mySet);

    void printVector(const std::vector<int>& myvec);

    void printvecMapSet(std::vector<std::map<char, std::set<int>>>& vec);

    void finalMap(const std::unordered_map<int, std::tuple<std::string, Priority, int>>& map);

    void printvecMapInt(const std::vector<std::map<char, int>>& vec);
};

#endif
