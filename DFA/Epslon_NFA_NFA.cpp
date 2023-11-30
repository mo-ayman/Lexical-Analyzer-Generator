
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
#include "HelpingMethods.h"
#include "Epslon_NFA_NFA.h"
using namespace std;

Epslon_NFA_NFA::Epslon_NFA_NFA(vector<map<char, vector<int>>>& table, map<int, string>& finals, int initial) {
        int indx = 0;
        for (map<char, vector<int>> TransitionMap : table)
        {
            map<char, set<int>> map;
            for (const auto& pair : TransitionMap) {
                std::set<int> currentSet(pair.second.begin(), pair.second.end());
                map.insert(std::pair<char, set<int>>(pair.first, currentSet));

            }
            TransitionTable.push_back(map);
        }
        finalStateV = finals;
        initialState = initial;
    };
  map<int, string> Epslon_NFA_NFA::get_Final_States()
    {
        return finalStateV;
    }

    vector<map<char, set<int>>> Epslon_NFA_NFA::get_NFA()
    {
        int indx = 0;
        for (map<char, set<int>> TransitionMap : TransitionTable) {
            Handle_Epslon(TransitionMap, indx);
            indx++;
        }
        indx = 0;
        for (map<char, set<int>>& TransitionMap : TransitionTable) {
            Replace_state_Returned(TransitionMap, indx);
            indx++;
        }
        indx = 0;
        for (map<char, set<int>>& TransitionMap : TransitionTable)
        {
            for (auto& pair : TransitionMap) {
                set<int> Combined_State = Return_Epslon_loop(pair.second);
                (pair.second).insert(Combined_State.begin(), Combined_State.end());
            }

            auto& mapToModify = TransitionTable[indx]; // Access the second map
           // Find the element and remove it
            auto it = mapToModify.find('\0');
            if (it != mapToModify.end()) {
                UpdateFinalStates(TransitionMap, indx);
                mapToModify.erase(it);
            }
            indx++;
        }

        return TransitionTable;
    };
    void Epslon_NFA_NFA::UpdateFinalStates(map<char, set<int>>& TransMap, int indx_final)
    {
        auto it = TransMap.find('\0');
        if (it != TransMap.end()) {
            for (int statElem : it->second)
            {
                auto check = finalStateV.find(statElem);
                if (check != finalStateV.end()) {
                    finalStateV.insert(std::pair<int, string>(indx_final, check->second));   
                    break;
                }
            }

        }
    }
    set<int>  Epslon_NFA_NFA::Return_Epslon_loop(set<int> setOfelem)
    {
        set<int> Combined_State;
        for (int elem : setOfelem)
        {
            if (TransitionTable[elem].find('\0') != TransitionTable[elem].end()) {
                set<int> forwardSet = TransitionTable[elem].find('\0')->second;
                Combined_State.insert(forwardSet.begin(), forwardSet.end());
            }
        }
        return Combined_State;

    }
    void Epslon_NFA_NFA::Replace_state_Returned(map<char, set<int>>& TransitionMap, int indx) {
        for (auto& pair : TransitionMap) {
            for (int elem : pair.second) {
                if (elem == indx)
                {
                    auto it = TransitionMap.find('\0');
                    if (it != TransitionMap.end()) {
                        set<int>& set1 = pair.second;
                        set1.insert(it->second.begin(), it->second.end());

                    }
                }
            }

        }

    }

    void Epslon_NFA_NFA::Handle_Epslon(map<char, set<int>>& TransitionMap, int indx) {
        auto stateEpslon = TransitionMap.find('\0');
        if (stateEpslon != TransitionMap.end()) {
            std::queue<int> EpslonQueue;/////////////////////////////////
            for (int element : TransitionMap.find('\0')->second) {
                EpslonQueue.push(element);
            }
            map<int, bool> checkVisited;
            while (!EpslonQueue.empty())
            {
                int state = EpslonQueue.front();
                EpslonQueue.pop();
                auto it = checkVisited.find(state);
                if (state == indx || it != checkVisited.end()) { continue; }

                UpdateCurrent_st(state, indx);

                for (int element : TransitionMap.find('\0')->second) {
                    EpslonQueue.push(element);
                }
                checkVisited.insert(std::pair<int, bool>(state, true));
            }

        }

    }
    void Epslon_NFA_NFA::UpdateCurrent_st(int forwardState, int currentIndx)
    {

        map<char, set<int>>& currentMap = TransitionTable[currentIndx];
        map<char, set<int>>& forwardMap = TransitionTable[forwardState];
        for (const auto& pair : forwardMap) {
            if (currentMap.find(pair.first) != currentMap.end()) {
                set<int>& currentSt = currentMap.find(pair.first)->second;
                currentSt.insert(pair.second.begin(), pair.second.end());
            }
            else {
                currentMap.insert(std::pair<char, set<int>>(pair.first, pair.second));
            }

        }
    }

/*
int main() {
    // Example transition table as an unordered map
    HelpingMethods usefulMethods;
    vector<map<char, vector<int>>> Ttable;
    map<char, vector<int>> myMap{
    {'0',{0}},
    {'e',{1}}
    };
    map<char, vector<int>> myMap2{
    {'1',{1}},
    {'e',{2}}
    };
    map<char, vector<int>> myMap3{
    {'0',{2}},
    {'1',{2}}
    };
    Ttable.push_back(myMap);
    Ttable.push_back(myMap2);
    Ttable.push_back(myMap3);
    vector<int> finalStates = { 2 };
    int initialState = 0;

    Epslon_NFA_NFA obj(Ttable, finalStates, initialState);
    vector<map<char, set<int>>> nfa = obj.get_NFA();
    vector<int> finals = obj.get_Final_States();

    std::cout << "Elements in the map:" << std::endl;

    usefulMethods.printvecMapSet(nfa);

    std::cout << "Element final States:" << std::endl;
    // Loop through the vector of maps
    usefulMethods.printVector(finals);



    return 0;
}
*/