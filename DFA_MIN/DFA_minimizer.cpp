#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "../LexicalRules/RuleTree.h"

using namespace std;

class DFA_minimizer{
    private:
        std::vector<int> assignNumbers(int num_of_states, unordered_map<int, tuple<string, Priority, int>> final_states, int* num_of_classes) {
            std::unordered_map<std::string, int> stringToNumber;
            std::vector<int> resultVector;

            int currentNumber = 1;
            for (int i = 0; i < num_of_states; i++)
            {
                if (final_states.find(i) == final_states.end()) {
                    resultVector.push_back(0);
                }
                else{
                    string str = get<0>(final_states[i]);
                    if (stringToNumber.find(str) == stringToNumber.end()) {
                        stringToNumber[str] = currentNumber++;
                    }
                    resultVector.push_back(stringToNumber[str]);
                }
            }
            *num_of_classes = currentNumber;
            return resultVector;
        }

    public:
        vector<map<char, int>> table;
        int start;
        unordered_map<int, tuple<string, Priority, int>> fstates;

        void minimize(vector<map<char, int>> dfa, int start_state, unordered_map<int, tuple<string, Priority, int>> final_states){
            const int MOD = 1000000007;
            int num_of_classes;

            // assign each class a number (0 for NF states, then 1,2,3,... for F states)
            std::vector<int> prev = assignNumbers(dfa.size(), final_states, &num_of_classes);
            std::vector<int> next(prev.size());
            unordered_map<int, int> S;

            // continue partitioning until convergence
            bool converged = false;
            while (!converged) {
                S.clear();
                for (int i = 0; i < dfa.size(); ++i) {
                    long long hash = 0;
                    string hstr = to_string(prev[i]) + ", ";
                    for (int k = 0; k < hstr.size(); k++)
                    {
                        hash = hash*256 + hstr[k];
                        hash %= MOD;
                    }
                    for (auto const& entry : dfa[i]) {
                        char input = entry.first;
                        int destination = entry.second;
                        // hash = hash ^ (hash << 5) ^ (hash >> 2) ^ input ^ prev[destination];
                        string hash_component_str = to_string(input) + ", " + to_string(prev[destination]) + ", ";
                        for (int k = 0; k < hash_component_str.size(); k++)
                        {
                            hash = hash*256 + hash_component_str[k];
                            hash %= MOD;
                        }
                    }
                    next[i] = hash;
                    S[hash] = i;
                }

                // check for convergence
                converged = (S.size() == static_cast<size_t>(num_of_classes));

                // update num of classes
                num_of_classes = S.size();

                // swap prev with next
                swap(prev, next);
            }
            
            // build S_idx
            unordered_map<int, int> S_idx;

            int idx = S.size()-1;
            for (auto const& entry : S) {
                S_idx[entry.first] = idx--;
            }

            // build the minimized DFA
            vector<map<char, int>> min_dfa;
            for (auto const& entry : S) {
                int target_state_idx = entry.second;
                map<char, int> target_state_map = dfa[target_state_idx];

                // build a new map for the target state
                map<char, int> m;
                for (auto const& input_entry : target_state_map) {
                    char input = input_entry.first;
                    int destination = S_idx[prev[input_entry.second]];
                    m[input] = destination;
                }

                min_dfa.insert(min_dfa.begin(), m);
            }
            // find the starting state for the minimum dfa.
            start = S_idx[prev[start_state]];

            // build the final states map for the minimal dfa.
            for (auto const& final_entry : final_states) { // assumes low index means higher priority.
                int original_fstate_idx = final_entry.first;
                int new_fstate_idx = S_idx[prev[original_fstate_idx]];
                if (fstates.find(new_fstate_idx) == fstates.end()) {
                    fstates[new_fstate_idx] = final_entry.second;
                }
                else{
                    Priority prev =  get<1>(fstates[new_fstate_idx]);
                    Priority challenging =  get<1>(final_entry.second);
                    if(challenging < prev){
                        fstates[new_fstate_idx] = final_entry.second;
                    }
                    else if(challenging == prev){
                        int prev_ord = get<2>(fstates[new_fstate_idx]);
                        int challenging_ord = get<2>(final_entry.second);
                        if(challenging_ord < prev_ord){
                            fstates[new_fstate_idx] = final_entry.second;
                        }
                        else if(challenging_ord == prev_ord){
                            throw std::runtime_error("Final states priority conflict (same class, same value)");
                        }
                    } 
                }
            }
            table = min_dfa;
        }
};
int main() {
    DFA_minimizer dfaMinimizer;

    // Define the DFA transition table
    vector<map<char, int>> dfa = {
        {{'b', 2}, {'a', 1}},
        {{'a', 2}, {'b', 3}},
        {{'b', 4}, {'a', 2}},
        {{'a', 3}, {'b', 3}},
        {{'b', 4}, {'a', 4}},

        // {{'a', 6}, {'b', 7}},
        // {{'a', 5}, {'b', 5}},
        // {{'a', 8}, {'b', 9}},
        // {{'a', 8}, {'b', 9}},
        // {{'a', 9}, {'b', 9}}
    };


    cout << "Original DFA Transition Table:" << endl;
    for (int i = 0; i < dfa.size(); ++i) {
        cout << "State " << i << ": ";
        for (auto const& entry : dfa[i]) {
            cout << entry.first << " -> " << entry.second << " | ";
        }
        cout << endl;
    }
    // Define the start state and final states
    int startState = 0;
    unordered_map<int, tuple<string, Priority, int>> final_states = {
        {3, {"f", RESERVED, 2}},
        {4, {"f", NORMAL, 3}}
    };

    // Minimize the DFA
    dfaMinimizer.minimize(dfa, startState, final_states);

    // Output the minimized DFA start state
    cout << "Minimized DFA Start State: " << dfaMinimizer.start << endl;

    // Display the minimized final states
    cout << "Minimized Final States:\n";
    for (const auto& entry : dfaMinimizer.fstates) {
        cout << "State " << entry.first << ": " << get<0>(entry.second) << ", Priority: " << get<1>(entry.second) << ", Order: " << get<2>(entry.second) << endl;
    }
    cout << endl;

    // Print header with input symbols
    cout << setw(10) << "State";
    for (const auto& entry : dfaMinimizer.table[0]) {
        cout << setw(10) << entry.first;
    }
    cout << endl;

    // Print transitions for each state
    for (int i = 0; i < dfaMinimizer.table.size(); ++i) {
        cout << setw(10) << i;
        for (const auto& entry : dfaMinimizer.table[i]) {
            cout << setw(10) << entry.second;
        }
        cout << endl;
    }
    return 0;
}
