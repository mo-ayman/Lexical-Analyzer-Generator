#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include "DFA.h"
#include "Epslon_NFA_NFA.cpp"
#include "../LexicalRules/RuleTree.h"
#include <unordered_map>
using namespace std;
HelpingMethods HM;
/*
  In this constructor first calling Epslon_NFA_NFA class to convert epslon_NFA states
  to it's equivalent nfa
*/

DFA::DFA(vector<map<char, vector<int>>>& table, unordered_map<int, tuple<string, Priority,int>>& finals, int initial) {
        Epslon_NFA_NFA obj(table, finals, initial);
        TransitionTable = obj.get_NFA();
        finalStates = obj.get_Final_States();
        initialState = initial;

    };
/*
  this method first push into stack the states and calling method HandleState for each state which
  in turn (This function i fill stateMap that map each state to it's transition state that state map to them
  and fill InputMap which map each state with it's input chars thers functions help me in fillDFA func).
  finall it call fillDFA func(This method fill DFA_States vector of each map by getting each state indx and replace it with set of states 
      corressponding) 

*/
    vector<map<char, int>> DFA::getDFA()
    {
        QueueStates.push({ initialState });
        std::set<int> myset = { initialState };
        HandleState(myset);
        QueueStates.pop();
        while (!QueueStates.empty())
        {
            HandleState(QueueStates.front());
            QueueStates.pop();

        }
        fillDFA();
        return DFA_States;
    }
    /*
      This method fill DFA_States vector of each map by getting each state indx and replace it with set of states 
      corressponding
    */
    void DFA::fillDFA() {
        map<set<int>, int> stateIndx = makeEachStateIndx();

        for (const auto& pair : stateMap) {
      
            map<char, int> eachStateMap;
            int indx = 0;
            for (const auto& set : pair.second)
            {
                auto it = InputMap.find(pair.first);
                auto item = it->second.begin();
                std::advance(item, indx); // Move the iterator to the desired index
                auto stateNum = stateIndx.find(set);
                eachStateMap.insert(std::pair<char, int>(*item, stateNum->second));
                indx++;
            }
            DFA_States.push_back(eachStateMap);
        }
    }
    /*
     This method used to map each set to it's indx in Transition table to used in  fillDFA() func
     when update DFA_states
    */
    map<set<int>, int> DFA::makeEachStateIndx() {
        map<set<int>, int> stateIndx;
        int indx = 0;
        for (const auto& pair : stateMap) {
            UpdateFinalStates(indx, pair.first);
            stateIndx.insert(std::pair<set<int>, int>(pair.first, indx));
            indx++;
        }

        return stateIndx;
    }
    
    /*
      This function i fill stateMap that map each state to it's transition state that state map to them
      and fill InputMap which map each state with it's input chars thers functions help me in fillDFA func
    */
    void DFA::HandleState(set<int>& states)
    {
        map<char, int> stringIndexMap;
        vector<char> inputTransition;
        vector<set<int>> transitionStates;

        //first check if state is handled before
        auto it = stateMap.find(states);
        if (it != stateMap.end()) {
            return;
        }
    
        int index = 0;
        for (int state : states) {
            
            for (const auto& pair : TransitionTable.at(state)) {
                auto it = stringIndexMap.find(pair.first);
                if (it != stringIndexMap.end()) {
                    auto it = stringIndexMap.find(pair.first);
                    transitionStates[it->second].insert((pair.second).begin(), (pair.second).end());
                }
                else {
                    if(TransitionTable.at(state).find(pair.first) != TransitionTable.at(state).end()){
                    stringIndexMap.insert(std::pair<char, int>(pair.first, index));
                    index++;
                    transitionStates.push_back(pair.second);
                    inputTransition.push_back(pair.first);
                    }
                }
            }
        }
   
        stateMap.insert(std::pair<set<int>, vector<set<int>>>(states, transitionStates));
        InputMap.insert(std::pair<set<int>, vector<char>>(states, inputTransition));
        for (const auto& set : transitionStates) {
            QueueStates.push(set);
        }

    }

    unordered_map<int, tuple<string,Priority , int>> DFA::get_finalStates() {
        return New_finalStates;

    }
    /*
       This method make new final states by applying if any
       state of old ones contain one of final state then take that one as final state
    */
    void DFA::UpdateFinalStates(int indx, set<int> OldState)
    {
        // this temp set used to store all final states that current state map to
       // to compare between them
        
        vector< tuple<string, Priority, int>> temp_final_states;
        bool flag = false;
        
        for (int elem : OldState)
        {
            auto it = finalStates.find(elem);
            if (it != finalStates.end()) {
                temp_final_states.push_back(it->second);
                flag = true;
            
            }
        }
        if (flag) {
            New_finalStates.insert(std::pair<int, tuple<string, Priority, int>>(indx, BestFinal(temp_final_states)));
         
        }
        
    }

    tuple<string, Priority, int> DFA::BestFinal(vector< tuple<string, Priority, int>>& temp_final_states) {

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

