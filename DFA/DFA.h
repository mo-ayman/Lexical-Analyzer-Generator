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
    vector<map<char, vector<int>>> TransitionTable;
    vector<int> finalStates;
    int initialState;
    vector<map<char, int>> DFA_States;

    //this for DFA
    map<vector<int>, vector<set<int>>> stateMap;
    map<vector<int>, set<char>> InputMap;
    std::queue<set<int>> QueueStates;
public:
    DFA(vector<map<char, vector<int>>>& table, vector<int>& finals, int initial) ;
    vector<map<char, int>> getDFA();
    void fillDFA();
    map<vector<int>, int> makeEachStateIndx();
    void HandleState(set<int>& states);
   
};


#endif