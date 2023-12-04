#ifndef DFA_H
#define DFA_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include "HelpingMethods.h"
#include "../LexicalRules/RuleTree.h"
#include <unordered_map>
using namespace std;

class DFA {
private:
   
    vector<map<char, set<int>>> TransitionTable;
    unordered_map<int, tuple<string, Priority, int>> finalStates;
    int initialState;
    vector<map<char, int>> DFA_States;

    //this for DFA
    map<set<int>, vector<set<int>>> stateMap;
    map<set<int>, vector<char>> InputMap;
    std::queue<set<int>> QueueStates;
    unordered_map<int, tuple<string, Priority, int>>  New_finalStates;
    
public:
    DFA(vector<map<char, vector<int>>>& table, unordered_map<int,tuple<string, Priority, int>>& finals, int initial) ;
    vector<map<char, int>> getDFA();
    void fillDFA();
    map<set<int>, int> makeEachStateIndx();
    void HandleState(set<int>& states);
    unordered_map<int, tuple<string, Priority, int>> get_finalStates();
    void UpdateFinalStates(int indx, set<int> OldState);
    tuple<string, Priority, int> BestFinal(vector< tuple<string, Priority, int>>& temp_final_states);
     
};


#endif