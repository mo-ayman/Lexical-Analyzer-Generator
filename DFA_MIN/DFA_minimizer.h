#ifndef DFA_MINIMIZER_H
#define DFA_MINIMIZER_H

#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "../LexicalRules/RuleTree.h"

using namespace std;

class DFA_minimizer {
private:
    std::vector<int> assignNumbers(int num_of_states, unordered_map<int, tuple<string, Priority, int>> final_states, int* num_of_classes);

public:
    vector<map<char, int>> table;
    int start;
    unordered_map<int, tuple<string, Priority, int>> fstates;

    void minimize(vector<map<char, int>> dfa, int start_state, unordered_map<int, tuple<string, Priority, int>> final_states);
};

#endif // DFA_MINIMIZER_H
