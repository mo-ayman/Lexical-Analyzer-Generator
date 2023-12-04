#ifndef DFA_MINIMIZER_H
#define DFA_MINIMIZER_H

#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

#include "../LexicalRules/RuleTree.h"

class DFAMinimizer {
public:
    int start;
    std::vector<std::map<char, int>> table;
    std::unordered_map<int, std::tuple<std::string, Priority, int>> fstates;

    void minimize(std::vector<std::map<char, int>> dfa, int start_state,
                  const std::unordered_map<int, std::tuple<std::string, Priority, int>>& final_states);
};

#endif // DFA_MINIMIZER_H
