
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
#include "Epslon_NFA_NFA.h"
#include "../LexicalRules/RuleTree.h"
using namespace std;


/*
This is the constructor of NFA first for loop to convert vextor im map of transition table to set to ease some 
functions in future
*/
Epslon_NFA_NFA::Epslon_NFA_NFA(vector<map<char, vector<int>>>& table, map<int, tuple<string, Priority, int>>& finals, int initial) {
        
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
   map<int, tuple<string, Priority, int>> Epslon_NFA_NFA::get_Final_States()
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
        // this temp set used to store all final states that current state map to
        // to compare between them
        vector< tuple<string, Priority, int>> temp_final_states;
        auto it = TransMap.find('\0');
        bool flag=false;
        if (it != TransMap.end()) {
            for (int statElem : it->second)
            {
                auto check = finalStateMap.find(statElem);
                if (check != finalStateMap.end()) {
                    temp_final_states.push_back(check->second);
                    flag = true;
                    //finalStateMap.insert(std::pair<int, tuple<string, Priority, int>>(indx_final, check->second));
                    break;
                }
            }
            if (flag) {
                  finalStateMap.insert(std::pair<int, tuple<string, Priority, int>>(indx_final, BestFinal(temp_final_states)));
            }
        }
    }
    tuple<string, Priority, int> Epslon_NFA_NFA::BestFinal(vector< tuple<string, Priority, int>>& temp_final_states) {
        
           // Sort the vector based on the double value in the tuple (at index 1,2)
           std::sort(temp_final_states.begin(), temp_final_states.end(), [](const auto& a, const auto& b) {
            if (std::get<1>(a) != std::get<1>(b)) {
                return std::get<1>(a) < std::get<1>(b); // Sort based on the string value (at index 1)
            }
            else {
                return std::get<2>(a) < std::get<2>(b); // If string values are equal, sort based on double value (at index 2)
            }
            });
           return temp_final_states[0];
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
