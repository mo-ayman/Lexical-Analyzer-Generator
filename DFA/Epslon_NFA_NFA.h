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
    map<int, bool> finalStates;
    vector<int> finalStateV;
    int initialState;
    vector<map<char, int>> DFA_States;


public:
    Epslon_NFA_NFA(vector<map<char, vector<int>>>& table, vector<int>& finals, int initial);
    void UpdateFinalStates(map<char, set<int>>& TransMap, int indx_final);
    vector<int> get_Final_States();
    vector<map<char, set<int>>> get_NFA();
    set<int>  Return_Epslon_loop(set<int> setOfelem);
    void Replace_state_Returned(map<char, set<int>>& TransitionMap, int indx);
    void Handle_Epslon(map<char, set<int>>& TransitionMap, int indx);
    void UpdateCurrent_st(int forwardState, int currentIndx);
   
   
};
#endif
