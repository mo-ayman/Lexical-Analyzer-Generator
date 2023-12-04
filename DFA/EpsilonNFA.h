#ifndef EPSLON_NFA_NFA_H
#define EPSLON_NFA_NFA_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <unordered_map>

#include "../LexicalRules/RuleTree.h"

class EpsilonNFA {
    std::vector<std::map<char, std::set<int>>> TransitionTable;
    std::unordered_map<int, std::tuple<std::string, Priority, int>> finalStateMap;
    int initialState;
    std::vector<std::map<char, int>> DFA_States;

public:
    EpsilonNFA(const std::vector<std::map<char, std::vector<int>>>& table,
               const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finals, int initial);

    void updateEpsilonIteration(std::vector<std::map<char, std::set<int>>>& T_table);

    std::set<int> getEpsilonStates(int indx_TT);

    void updateFinalStates(std::map<char, std::set<int>>& TransMap, int indx_final);

    std::unordered_map<int, std::tuple<std::string, Priority, int>> getFinalStates();

    std::vector<std::map<char, std::set<int>>> getNFA();

    void handleEpsilon(std::map<char, std::set<int>>& TransitionMap, int indx);

    void updateCurrentSt(int forwardState, int currentIndx);

    static std::tuple<std::string, Priority, int> bestFinal(
        std::vector<std::tuple<std::string, Priority, int>>& temp_final_states);
};
#endif
