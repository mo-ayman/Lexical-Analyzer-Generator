
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
using namespace std;

class DFA {
private:
    vector<map<string, vector<int>>> TransitionTable;
    vector<int> finalStates;
    int initialState;
    vector<map<string,int>> DFA_States;

    //this for DFA
    map<vector<int>, vector<set<int>>> stateMap;
    map<vector<int>, set<string>> InputMap;
    std::queue<set<int>> QueueStates;
public:
    DFA(vector<map<string, vector<int>>>& table, vector<int>& finals, int initial) {
        TransitionTable = table;
        finalStates = finals;
        initialState = initial;

    };
    vector<map<string, int>> getDFA()
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
    void fillDFA() {
        map<vector<int>, int> stateIndx=makeEachStateIndx();
        
        for (const auto& pair : stateMap) {
            map<string, int> eachStateMap;
            int indx = 0;
            for (const auto& set : pair.second)
            {
                std::vector<int> vecMap(set.begin(), set.end());
                auto it = InputMap.find(pair.first);
                auto item = it->second.begin();
                std::advance(item, indx); // Move the iterator to the desired index
                cout<<*item<<std::endl;
                auto stateNum = stateIndx.find(vecMap);
                eachStateMap.insert(std::pair<string, int>(*item, stateNum->second));
                indx++;
            }
            DFA_States.push_back(eachStateMap);
        }
    }
    map<vector<int>,int> makeEachStateIndx() {
        map<vector<int>, int> stateIndx;
        int indx = 0;
        for (const auto& pair : stateMap) {
            
            stateIndx.insert(std::pair<vector<int>, int>(pair.first, indx));
            indx++;
        }
        
        return stateIndx;
    }
    void HandleState(set<int>& states)
    {
        
        // std::cout << "ttttttttttttt " << std::endl;
        // for (int element : states) {
        //      std::cout << "  " << element;
        // }
        // std::cout << "  enddddddddddd " << std::endl;

        std::vector<int> checkVec(states.begin(), states.end());
        auto it = stateMap.find(checkVec);
        if (it != stateMap.end()) {
            return;
        }
        map<string, int> stringIndexMap;
        set<string> inputTransition;
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
                    transitionStates[it->second].insert(mySet2.begin(),mySet2.end());
                }
                else {
                    stringIndexMap.insert(std::pair<string, int>(pair.first, index));
                    index++;
                    transitionStates.push_back(mySet);
                    inputTransition.insert(pair.first);
                }
            }
        }
        std::vector<int> mVector(states.begin(), states.end());
        stateMap.insert(std::pair<vector<int>, vector<set<int>>>(mVector, transitionStates));
        InputMap.insert(std::pair<vector<int>, set<string>>(mVector, inputTransition));
        for (const auto& set : transitionStates) {
            QueueStates.push(set);
        }
       
    }
};

int main() {
    // Example transition table as an unordered map
    vector<map<string, vector<int>>> transitionTable;
    map<string, vector<int>> myMap{
    {"a",{0,1}},
    {"b",{2}}
    };
    map<string, vector<int>> myMap2{
    {"a",{0}},
    {"b",{1}}
    };
    map<string, vector<int>> myMap3{
    {"b",{0,1}}
    };
    transitionTable.push_back(myMap);
    transitionTable.push_back(myMap2);
    transitionTable.push_back(myMap3);
    vector<int> finalStates;
    int initialState=0;
    DFA obj(transitionTable, finalStates, initialState);
    vector<map<string, int>> dfa = obj.getDFA();


    std::cout << "Elements in the map:" << std::endl;
    // Loop through the vector of maps
    int indx=0;
    for (const auto& myMap : dfa) {
        std::cout << "Elements in the map:" << std::endl;
        // Loop through each map and print its key-value pairs
        for (const auto& pair : myMap) {
            std::cout<<indx<<": "<< pair.first << " " << pair.second << std::endl;
        }
        std::cout << std::endl;
        indx++;
    }


    return 0;
}
