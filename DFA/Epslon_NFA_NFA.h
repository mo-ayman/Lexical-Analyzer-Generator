#ifndef EPSLON_NFA_NFA_H
#define EPSLON_NFA_NFA_H
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;

class Epslon_NFA_NFA {
private:
    vector<map<char, set<int>>> TransitionTable;
    map<int, string>  finalStateMap;
    int initialState;
    vector<map<char, int>> DFA_States;


public:
    Epslon_NFA_NFA(vector<map<char, vector<int>>>& table, map<int, string>& finals, int initial);
    void Update_Epslon_iteration(vector<map<char, set<int>>>&  T_table);
    set<int> get_epslon_states(int indx_TT);
    void UpdateFinalStates(map<char, set<int>>& TransMap, int indx_final);
    map<int, string> get_Final_States();
    vector<map<char, set<int>>> get_NFA();
    void Handle_Epslon(map<char, set<int>>& TransitionMap, int indx);
    void UpdateCurrent_st(int forwardState, int currentIndx);
   
   
};
#endif
