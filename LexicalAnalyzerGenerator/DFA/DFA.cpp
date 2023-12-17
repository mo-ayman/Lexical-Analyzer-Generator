#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include <unordered_map>
#include <algorithm>

#include "DFA.h"
#include "EpsilonNFA.h"
#include "../LexicalRules/RuleTree.h"

/*
  In this constructor first calling Epsilon_NFA_NFA class to convert epsilon_NFA states
  to it's equivalent nfa
*/
DFA::DFA(const std::vector<std::map<char, std::vector<int>>>& table,
         const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finals, const int initial) {
    EpsilonNFA obj(table, finals, initial);
    TransitionTable = obj.getNFA();
    finalStates = obj.getFinalStates();
    initialState = initial;
}

/*
  this method first push into queue the states and calling method HandleState for each state which
  in turn (This function i fill stateMap that map each state to it's transition state that state map to them
  and fill InputMap which map each state with it's input chars thers functions help me in getDFA func).
  finally it call fillDFA func(This method fill DFA_States vector of each map by getting each state index and replace it with set of states
      corressponding)
*/
std::vector<std::map<char, int>> DFA::getDFA() {
    QueueStates.push({initialState});
    std::set myset = {initialState};
    handleState(myset);
    QueueStates.pop();
    while (!QueueStates.empty()) {
        handleState(QueueStates.front());
        QueueStates.pop();
    }
    // Here Calling below method
    fillDFA();
    return DFA_States;
}

/*
  This method fill DFA_States vector of each map by getting each state index and replace it with set of states
  corressponding
*/
void DFA::fillDFA() {
    std::map<std::set<int>, int> stateIndx = makeEachStateIndx();

    for (const auto& pair: stateMap) {
        std::map<char, int> eachStateMap;
        int indx = 0;
        for (const auto& set: pair.second) {
            const auto it = InputMap.find(pair.first);
            // go to the equivalent input order as set in InputMap
            auto item = it->second.begin();
            std::advance(item, indx); // Move the iterator to the desired index
            // get indx of the current set of states to replace it
            const auto stateNum = stateIndx.find(set);
            eachStateMap.insert(std::pair(*item, stateNum->second));
            indx++;
        }
        DFA_States.push_back(eachStateMap);
    }
}

/*
 This method used to map each set to it's index in Transition table to used in  fillDFA() func
 when update DFA_states
*/
std::map<std::set<int>, int> DFA::makeEachStateIndx() {
    std::map<std::set<int>, int> stateIndx;
    int indx = 0;
    for (const auto& pair: stateMap) {
        updateFinalStates(indx, pair.first);
        stateIndx.insert(std::pair(pair.first, indx));
        indx++;
    }

    return stateIndx;
}

/*
  This function i fill stateMap that map each state to it's transition state that state map to them
  and fill InputMap which map each state with it's input chars this function help me in getDFA() func
*/
void DFA::handleState(std::set<int>& states) {
    std::map<char, int> stringIndexMap;
    std::vector<char> inputTransition;
    std::vector<std::set<int>> transitionStates;

    //first check if state is handled before
    if (stateMap.find(states) != stateMap.end()) {
        return;
    }

    int index = 0;
    for (const int state: states) {
        for (const auto& pair: TransitionTable.at(state)) {
            if (stringIndexMap.find(pair.first) != stringIndexMap.end()) {
                const auto it = stringIndexMap.find(pair.first);
                transitionStates[it->second].insert((pair.second).begin(), (pair.second).end());
            } else {
                if (TransitionTable.at(state).find(pair.first) != TransitionTable.at(state).end()) {
                    stringIndexMap.insert(std::pair<char, int>(pair.first, index));
                    index++;
                    transitionStates.push_back(pair.second);
                    inputTransition.push_back(pair.first);
                }
            }
        }
    }

    stateMap.insert(std::pair(states, transitionStates));
    InputMap.insert(std::pair(states, inputTransition));
    for (const auto& set: transitionStates) {
        QueueStates.push(set);
    }
}

std::unordered_map<int, std::tuple<std::string, Priority, int>> DFA::getFinalStates() {
    return New_finalStates;
}

/*
   This method make new final states by applying if any
   state of old ones contain one of final state then take that one as final state by taking the most  priority one
*/
void DFA::updateFinalStates(int indx, const std::set<int>& OldState) {
    // this temp set used to store all final states that current state map to
    // to compare between them
    std::vector<std::tuple<std::string, Priority, int>> temp_final_states;
    bool flag = false;

    for (int elem: OldState) {
        if (const auto it = finalStates.find(elem); it != finalStates.end()) {
            temp_final_states.push_back(it->second);
            flag = true;
        }
    }
    if (flag) {
        New_finalStates.insert(std::pair(indx, bestFinal(temp_final_states)));
    }
}

std::tuple<std::string, Priority, int> DFA::bestFinal(
    std::vector<std::tuple<std::string, Priority, int>>& temp_final_states) {
    // Sort the vector based on the double value in the tuple (at index 1,2)
    std::sort(temp_final_states.begin(), temp_final_states.end(), [](const auto& a, const auto& b) {
        if (std::get<1>(a) != std::get<1>(b)) {
            return std::get<1>(a) < std::get<1>(b); // Sort based on the string value (at index 1)
        }
        // If string values are equal, sort based on double value (at index 2)
        return std::get<2>(a) < std::get<2>(b);
    });
    return temp_final_states[0];
}
