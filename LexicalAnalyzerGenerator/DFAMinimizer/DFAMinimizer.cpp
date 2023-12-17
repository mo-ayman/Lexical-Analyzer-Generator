#include "DFAMinimizer.h"
#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "../LexicalRules/RuleTree.h"
static constexpr int MOD = 1000000007;

static std::vector<int> assignNumbers(const int num_of_states,
                                      std::unordered_map<int, std::tuple<std::string, Priority, int>> final_states,
                                      size_t* partitionsCount) {
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
    *partitionsCount = currentNumber;
    return resultVector;
}

long long hash(const std::string& s, const long long oldHashValue = 0){
    long long hashValue = oldHashValue;
    for (const char k: s) {
        hashValue = hashValue * 256 + k;
        hashValue %= MOD;
    }
    return hashValue;
}

void DFAMinimizer::minimize(const std::vector<std::map<char, int>>& dfa, const int start_state,
                            const std::unordered_map<int, std::tuple<std::string, Priority, int>>& final_states) {

    size_t partitionsCount;

    // assign each class a number (0 for NF states, then 1,2,3,... for F states)
    std::vector<int> currentPartition = assignNumbers(static_cast<int>(dfa.size()), final_states, &partitionsCount);
    std::vector<int> nextPartition(currentPartition.size());
    std::unordered_map<int, int> partitionToState;

    // continue partitioning until convergence
    bool converged = false;
    while (!converged) {
        partitionToState.clear();
        for (int i = 0; i < dfa.size(); ++i) {
            std::string hstr = std::to_string(currentPartition[i]) + ", ";
            long long h = hash(hstr);
            for (const auto& entry: dfa[i]) {
                const char input = entry.first;
                const int destination = entry.second;
                hstr = std::to_string(input) + ", " + std::to_string(currentPartition[destination]) + ", ";
                h = hash(hstr, h);
            }
            nextPartition[i] = static_cast<int>(h);
            partitionToState[static_cast<int>(h)] = i;
        }

        // check for convergence
        converged = (partitionToState.size() == partitionsCount);

        // update partitions count
        partitionsCount = partitionToState.size();

        // swap currentPartition with nextPartition
        swap(currentPartition, nextPartition);
    }

    // build partitionToIndex map
    std::unordered_map<int, int> partitionToIndex;

    int idx = static_cast<int>(partitionToState.size()) - 1;
    for (const auto& entry: partitionToState) {
        partitionToIndex[entry.first] = idx--;
    }

    // build the minimized DFA
    std::vector<std::map<char, int>> min_dfa;
    for (const auto& entry: partitionToState) {
        const int targetState = entry.second;
        std::map<char, int> targetStateMap = dfa[targetState];

        // build a new map for the partition of target state
        std::map<char, int> m;
        for (const auto& entry: targetStateMap) {
            char input = entry.first;
            const int destination = partitionToIndex[currentPartition[entry.second]];
            m[input] = destination;
        }

        min_dfa.insert(min_dfa.begin(), m);
    }
    // find the starting state for the minimal dfa.
    start = partitionToIndex[currentPartition[start_state]];

    // build the final states map for the minimal dfa.
    for (const auto& entry: final_states) {
        const int stateIndex = entry.first;
        const int partitionIndex = partitionToIndex[currentPartition[stateIndex]];
        if (fstates.find(partitionIndex) == fstates.end()) {
            fstates[partitionIndex] = entry.second;
        } else {
            const Priority prevPriority = std::get<1>(fstates[partitionIndex]);
            const Priority challengingPriority = std::get<1>(entry.second);
            if (challengingPriority < prevPriority) {
                fstates[partitionIndex] = entry.second;
            } else if (challengingPriority == prevPriority) {
                const int prevIdx = std::get<2>(fstates[partitionIndex]);
                const int challengingIdx = std::get<2>(entry.second);
                if (challengingIdx < prevIdx) {
                    fstates[partitionIndex] = entry.second;
                }
            }
        }
    }
    table = min_dfa;
}