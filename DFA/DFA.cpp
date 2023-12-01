#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include "DFA.h"
#include "Epslon_NFA_NFA.h"
#include "HelpingMethods.h"
using namespace std;

/*
  In this constructor first calling Epslon_NFA_NFA class to convert epslon_NFA states
  to it's equivalent nfa
*/

DFA::DFA(vector<map<char, vector<int>>>& table, map<int, string>& finals, int initial) {
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
        std::cout << "mohamed Elgreatly" << std::endl;
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
                //cout << *item << std::endl;
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
        set<char> inputTransition;
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
                    stringIndexMap.insert(std::pair<char, int>(pair.first, index));
                    index++;
                    transitionStates.push_back(pair.second);
                    inputTransition.insert(pair.first);
                }
            }
        }

        stateMap.insert(std::pair<set<int>, vector<set<int>>>(states, transitionStates));
        InputMap.insert(std::pair<set<int>, set<char>>(states, inputTransition));
        for (const auto& set : transitionStates) {
            QueueStates.push(set);
        }

    }

    map<int, string> DFA::get_finalStates() {
        return New_finalStates;

    }
    /*
       This method make new final states by applying if any
       state of old ones contain one of final state then take that one as final state
    */
    void DFA::UpdateFinalStates(int indx, set<int> OldState)
    {
        HelpingMethods HM;
        for (int elem : OldState)
        {
            auto it = finalStates.find(elem);
            if (it != finalStates.end()) {
                New_finalStates.insert(std::pair<int, string>(indx, it->second));
                break;
            }
        }
        
    }
  /*
int main() {
    HelpingMethods HM;
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
    map<int, string> finalStates{ {2,"id"} };
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
    cout << "Final States :  " << endl;
    map<int, string> mapFinal = obj.get_finalStates();
    HM.finalMap(mapFinal);

    return 0;
}
*/