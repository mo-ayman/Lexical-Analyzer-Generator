
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
/*
This is the constructor of NFA first for loop to convert vextor im map of transition table to set to ease some 
functions in future
*/
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
        Update_Epslon_iteration(TransitionTable);
        finalStateMap = finals;
        initialState = initial;
    };
   void Epslon_NFA_NFA::Update_Epslon_iteration(vector<map<char, set<int>>>& T_table) {
       for (map<char, set<int>>& TransitionMap : T_table)
       {
           for (auto& pair : TransitionMap) {
               if (pair.first == '\0') {
                   for (int elem : pair.second) {
                       const set<int>& Epslon_state= get_epslon_states(elem);
                       (pair.second).insert(Epslon_state.begin(), Epslon_state.end());
                   }
               }
           }
       }
   }
   set<int> Epslon_NFA_NFA::get_epslon_states(int indx_TT)
   {
       auto it = TransitionTable[indx_TT].find('\0');
       if (it != TransitionTable[indx_TT].end())
       {
           return it->second;
       }
       return set<int>{};
   }
   map<int, string> Epslon_NFA_NFA::get_Final_States()
     {
        return finalStateMap;
     }
   /*
     first we loop in TransitionTable and call Handle_Epslon which in turns
     we push epslon states in queue and loop through it to add it's reach state int queue and int set vector of
     Current map indx.
     second update final states map after ending epslon transition and delete all epslons states.

   */
    vector<map<char, set<int>>> Epslon_NFA_NFA::get_NFA()
    {
        int indx = 0;
        for (map<char, set<int>> TransitionMap : TransitionTable) {
            Handle_Epslon(TransitionMap, indx);
            indx++;
        }

        indx = 0;
        for (map<char, set<int>>& TransitionMap : TransitionTable)
        {
          
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
    /*
     This function used to update final state to include all 
     states that their epslon transition go to final state.
    */
    void Epslon_NFA_NFA::UpdateFinalStates(map<char, set<int>>& TransMap, int indx_final)
    {
        auto it = TransMap.find('\0');
        if (it != TransMap.end()) {
            for (int statElem : it->second)
            {
                auto check = finalStateMap.find(statElem);
                if (check != finalStateMap.end()) {
                    finalStateMap.insert(std::pair<int, string>(indx_final, check->second));
                    break;
                }
            }

        }
    }

    /*
     First we push epslon states in queue and loop through it to add it's reach state int queue and int set vector of
     Current map indx
    */
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
    /*
    This method used to update current state according if
    it has in epsoln states for forwarding one
    */
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