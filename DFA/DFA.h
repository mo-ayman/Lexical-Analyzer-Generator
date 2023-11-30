#ifndef DFA_H
#define DFA_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
using namespace std;

class DFA {
private:
    vector<map<char, set<int>>> TransitionTable;
    map<int,string> finalStates;
    int initialState;
    vector<map<char, int>> DFA_States;

    //this for DFA
    map<set<int>, vector<set<int>>> stateMap;
    map<set<int>, set<char>> InputMap;
    std::queue<set<int>> QueueStates;
    map<int, string> New_finalStates;
public:
    DFA(vector<map<char, vector<int>>>& table, map<int, string>& finals, int initial) ;
    vector<map<char, int>> getDFA();
    void fillDFA();
    map<set<int>, int> makeEachStateIndx();
    void HandleState(set<int>& states);
    map<int, string> get_finalStates();
    void UpdateFinalStates(int indx, set<int> OldState);
     
};


#endif