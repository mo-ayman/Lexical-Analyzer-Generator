#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include "DFA.h"

using namespace std;


DFA::DFA(vector<map<char, vector<int>>>& table, vector<int>& finals, int initial) {
        TransitionTable = table;
        finalStates = finals;
        initialState = initial;

    };
    vector<map<char, int>> DFA::getDFA()
    {
        QueueStates.push({ initialState });
        std::set<int> myVector = { initialState };
        HandleState(myVector);
        QueueStates.pop();
        while (!QueueStates.empty())
        {
            HandleState(QueueStates.front());
            QueueStates.pop();

        }
        std::cout << "mohamed Elgreatly" << std::endl;
        fillDFA();
        return DFA_States;
    }
    void DFA::fillDFA() {
        map<vector<int>, int> stateIndx = makeEachStateIndx();

        for (const auto& pair : stateMap) {
            map<char, int> eachStateMap;
            int indx = 0;
            for (const auto& set : pair.second)
            {
                std::vector<int> vecMap(set.begin(), set.end());
                auto it = InputMap.find(pair.first);
                auto item = it->second.begin();
                std::advance(item, indx); // Move the iterator to the desired index
                cout << *item << std::endl;
                auto stateNum = stateIndx.find(vecMap);
                eachStateMap.insert(std::pair<char, int>(*item, stateNum->second));
                indx++;
            }
            DFA_States.push_back(eachStateMap);
        }
    }
    map<vector<int>, int> DFA::makeEachStateIndx() {
        map<vector<int>, int> stateIndx;
        int indx = 0;
        for (const auto& pair : stateMap) {

            stateIndx.insert(std::pair<vector<int>, int>(pair.first, indx));
            indx++;
        }

        return stateIndx;
    }
    void DFA::HandleState(set<int>& states)
    {
        std::vector<int> checkVec(states.begin(), states.end());
        auto it = stateMap.find(checkVec);
        if (it != stateMap.end()) {
            return;
        }
        map<char, int> stringIndexMap;
        set<char> inputTransition;
        vector<set<int>> transitionStates;
        int index = 0;
        for (int state : states) {
            // std::cout << "hamada " << state<< std::endl;
            for (const auto& pair : TransitionTable.at(state)) {
                // Convert the vector to a set
                std::set<int> mySet(pair.second.begin(), pair.second.end());

                auto it = stringIndexMap.find(pair.first);
                if (it != stringIndexMap.end()) {
                    auto it = stringIndexMap.find(pair.first);
                    std::set<int> mySet2(pair.second.begin(), pair.second.end());
                    transitionStates[it->second].insert(mySet2.begin(), mySet2.end());
                }
                else {
                    stringIndexMap.insert(std::pair<char, int>(pair.first, index));
                    index++;
                    transitionStates.push_back(mySet);
                    inputTransition.insert(pair.first);
                }
            }
        }
        std::vector<int> mVector(states.begin(), states.end());
        stateMap.insert(std::pair<vector<int>, vector<set<int>>>(mVector, transitionStates));
        InputMap.insert(std::pair<vector<int>, set<char>>(mVector, inputTransition));
        for (const auto& set : transitionStates) {
            QueueStates.push(set);
        }

    }


int main() {
    // Example transition table as an unordered map
    vector<map<char, vector<int>>> transitionTable;
    map<char, vector<int>> myMap{
    {'a',{0,1}},
    {'b',{2}}
    };
    map<char, vector<int>> myMap2{
    {'a',{0}},
    {'b',{1}}
    };
    map<char, vector<int>> myMap3{
    {'b',{0,1}}
    };
    transitionTable.push_back(myMap);
    transitionTable.push_back(myMap2);
    transitionTable.push_back(myMap3);
    vector<int> finalStates;
    int initialState = 0;
    DFA obj(transitionTable, finalStates, initialState);
    vector<map<char, int>> dfa = obj.getDFA();


    std::cout << "Elements in the map:" << std::endl;
    // Loop through the vector of maps
    int indx = 0;
    for (const auto& myMap : dfa) {
        std::cout << "Elements in the map:" << std::endl;
        // Loop through each map and print its key-value pairs
        for (const auto& pair : myMap) {
            std::cout << indx << ": " << pair.first << " " << pair.second << std::endl;
        }
        std::cout << std::endl;
        indx++;
    }


    return 0;
}
