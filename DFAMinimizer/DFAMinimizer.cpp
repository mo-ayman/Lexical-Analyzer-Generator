#include "DFAMinimizer.h"
#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "../LexicalRules/RuleTree.h"

static std::vector<int> assignNumbers(const int num_of_states,
                               std::unordered_map<int, std::tuple<std::string, Priority, int>> final_states,
                               size_t* num_of_classes) {
    std::unordered_map<std::string, int> stringToNumber;
    std::vector<int> resultVector;

    int currentNumber = 1;
    for (int i = 0; i < num_of_states; i++) {
        if (final_states.find(i) == final_states.end()) {
            resultVector.push_back(0);
        } else {
            std::string str = std::get<0>(final_states[i]);
            if (stringToNumber.find(str) == stringToNumber.end()) {
                stringToNumber[str] = currentNumber++;
            }
            resultVector.push_back(stringToNumber[str]);
        }
    }
    *num_of_classes = currentNumber;
    return resultVector;
}

void DFAMinimizer::minimize(std::vector<std::map<char, int>> dfa, int start_state,
                            const std::unordered_map<int, std::tuple<std::string, Priority, int>>& final_states) {
    size_t num_of_classes;

    // assign each class a number (0 for NF states, then 1,2,3,... for F states)
    std::vector<int> prev = assignNumbers(static_cast<int>(dfa.size()), final_states, &num_of_classes);
    std::vector<int> next(prev.size());
    std::unordered_map<int, int> S;

    // continue partitioning until convergence
    bool converged = false;
    while (!converged) {
        S.clear();
        for (int i = 0; i < dfa.size(); ++i) {
            constexpr int MOD = 1000000007;
            long long hash = 0;
            std::string hstr = std::to_string(prev[i]) + ", ";
            for (char k: hstr) {
                hash = hash * 256 + k;
                hash %= MOD;
            }
            for (const auto& entry: dfa[i]) {
                char input = entry.first;
                int destination = entry.second;
                std::string hash_component_str =
                        std::to_string(input) + ", " + std::to_string(prev[destination]) + ", ";
                for (char k: hash_component_str) {
                    hash = hash * 256 + k;
                    hash %= MOD;
                }
            }
            next[i] = static_cast<int>(hash);
            S[static_cast<int>(hash)] = i;
        }

        // check for convergence
        converged = (S.size() == num_of_classes);

        // update num of classes
        num_of_classes = S.size();

        // swap prev with next
        swap(prev, next);
    }

    // build S_idx
    std::unordered_map<int, int> S_idx;

    int idx = static_cast<int>(S.size()) - 1;
    for (const auto& entry: S) {
        S_idx[entry.first] = idx--;
    }

    // build the minimized DFA
    std::vector<std::map<char, int>> min_dfa;
    for (const auto& entry: S) {
        int target_state_idx = entry.second;
        std::map<char, int> target_state_map = dfa[target_state_idx];

        // build a new map for the target state
        std::map<char, int> m;
        for (const auto& input_entry: target_state_map) {
            char input = input_entry.first;
            int destination = S_idx[prev[input_entry.second]];
            m[input] = destination;
        }

        min_dfa.insert(min_dfa.begin(), m);
    }
    // find the starting state for the minimum dfa.
    start = S_idx[prev[start_state]];

    // build the final states map for the minimal dfa.
    for (const auto& final_entry: final_states) {
        // assumes low index means higher priority.
        int original_fstate_idx = final_entry.first;
        if (const int new_fstate_idx = S_idx[prev[original_fstate_idx]];
            fstates.find(new_fstate_idx) == fstates.end()) {
            fstates[new_fstate_idx] = final_entry.second;
        } else {
            Priority prev = std::get<1>(fstates[new_fstate_idx]);
            Priority challenging = std::get<1>(final_entry.second);
            if (challenging < prev) {
                fstates[new_fstate_idx] = final_entry.second;
            } else if (challenging == prev) {
                int prev_ord = std::get<2>(fstates[new_fstate_idx]);
                int challenging_ord = std::get<2>(final_entry.second);
                if (challenging_ord < prev_ord) {
                    fstates[new_fstate_idx] = final_entry.second;
                }
            }
        }
    }
    table = min_dfa;
}
