#ifndef EPSLON_NFA_NFA_H
#define EPSLON_NFA_NFA_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
#include "../LexicalRules/RuleTree.h"
#include <unordered_map>

using namespace std;

class Epslon_NFA_NFA {
private:
    vector<map<char, set<int>>> TransitionTable;
    unordered_map<int, tuple<string, Priority, int>> finalStateMap;
    int initialState;
    vector<map<char, int>> DFA_States;

public:
    Epslon_NFA_NFA(vector<map<char, vector<int>>>& table, unordered_map<int, tuple<string, Priority, int>>& finals, int initial);
    void Update_Epslon_iteration(vector<map<char, set<int>>>&  T_table);
    set<int> get_epslon_states(int indx_TT);
    void UpdateFinalStates(map<char, set<int>>& TransMap, int indx_final);
    unordered_map<int, tuple<string, Priority, int>>  get_Final_States();
    vector<map<char, set<int>>> get_NFA();
    void Handle_Epslon(map<char, set<int>>& TransitionMap, int indx);
    void UpdateCurrent_st(int forwardState, int currentIndx);
    tuple<string, Priority, int> BestFinal(vector< tuple<string, Priority, int>>& temp_final_states);
   
   
};
#endif
