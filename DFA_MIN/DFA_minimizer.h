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
     //vector<int> assignNumbers(int num_of_states, unordered_map<int, tuple<string, Priority, int>> final_states, int* num_of_classes);
     vector<int> assignNumbers(int num_of_states, unordered_map<int, tuple<string, Priority, int>> final_states, size_t* num_of_classes) {
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

    void minimize(vector<map<char, int>> dfa, int start_state, unordered_map<int, tuple<string, Priority, int>> final_states);
};

#endif // DFA_MINIMIZER_H
