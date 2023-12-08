#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
#include <unordered_map>

#include "EpsilonNFA.h"
#include "../LexicalRules/RuleTree.h"

/*
This is the constructor of NFA first for loop to convert vextor im map of transition table to set to ease some 
functions in future
*/
EpsilonNFA::EpsilonNFA(const std::vector<std::map<char, std::vector<int>>>& table,
                       const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finals,
                       const int initial) {
    for (const std::map<char, std::vector<int>>& TransitionMap: table) {
        std::map<char, std::set<int>> map;
        for (const auto& pair: TransitionMap) {
            std::set currentSet(pair.second.begin(), pair.second.end());
            map.insert(std::pair(pair.first, currentSet));
        }
        TransitionTable.push_back(map);
    }
    updateEpsilonIteration(TransitionTable);
    finalStateMap = finals;
    initialState = initial;
}
/*
This method used to map each states to all states with epslon transition like if 1 under epslon
got to state 2 and 2 under epslon got to 3 then 1 go to 3 under epslon
*/
void EpsilonNFA::updateEpsilonIteration(std::vector<std::map<char, std::set<int>>>& T_table) {
    for (std::map<char, std::set<int>>& TransitionMap: T_table) {
        for (auto& pair: TransitionMap) {
            if (pair.first == '\0') {
                for (const int elem: pair.second) {
                    const std::set<int>& Epsilon_state = getEpsilonStates(elem);
                    pair.second.insert(Epsilon_state.begin(), Epsilon_state.end());
                }
            }
        }
    }
}

std::set<int> EpsilonNFA::getEpsilonStates(int indx_TT) {
    if (const auto it = TransitionTable[indx_TT].find('\0'); it != TransitionTable[indx_TT].end()) {
        return it->second;
    }
    return std::set<int>{};
}

std::unordered_map<int, std::tuple<std::string, Priority, int>> EpsilonNFA::getFinalStates() {
    return finalStateMap;
}

/*
  first we loop in TransitionTable and call Handle_Epsilon which in turns
  we push epsilon states in queue and loop through it to add it's reach state int queue and int set vector of
  Current map indx.
  second update final states map after ending epsilon transition and delete all epsilons states.
*/
std::vector<std::map<char, std::set<int>>> EpsilonNFA::getNFA() {
    int indx = 0;
    for (std::map<char, std::set<int>> TransitionMap: TransitionTable) {
        handleEpsilon(TransitionMap, indx);
        indx++;
    }

    indx = 0;
    for (std::map<char, std::set<int>>& TransitionMap: TransitionTable) {
        auto& mapToModify = TransitionTable[indx]; // Access the second map
        // Find the element and remove it
        if (auto it = mapToModify.find('\0'); it != mapToModify.end()) {
            updateFinalStates(TransitionMap, indx);
            mapToModify.erase(it);
        }
        indx++;
    }

    return TransitionTable;
}

/*
 This function used to update final state to include all
 states that their epsilon transition go to final state.
*/
void EpsilonNFA::updateFinalStates(std::map<char, std::set<int>>& TransMap, int indx_final) {
    if (const auto it = TransMap.find('\0'); it != TransMap.end()) {
        // this temp set used to store all final states that current state map to
        // to compare between them
        std::vector<std::tuple<std::string, Priority, int>> temp_final_states;
        bool flag = false;
        for (int statElem: it->second) {
            if (auto check = finalStateMap.find(statElem); check != finalStateMap.end()) {
                temp_final_states.push_back(check->second);
                flag = true;
                break;
            }
        }
        if (flag) {
            finalStateMap.insert(std::pair(indx_final, bestFinal(temp_final_states)));
        }
    }
}

std::tuple<std::string, Priority, int> EpsilonNFA::bestFinal(
    std::vector<std::tuple<std::string, Priority, int>>& temp_final_states) {
    // Sort the vector based on the double value in the tuple (at index 1,2)
    std::sort(temp_final_states.begin(), temp_final_states.end(), [](const auto& a, const auto& b) {
        if (std::get<1>(a) != std::get<1>(b)) {
            // Sort based on the string value (at index 1)
            return std::get<1>(a) < std::get<1>(b);
        }
        // If string values are equal, sort based on double value (at index 2)
        return std::get<2>(a) < std::get<2>(b);
    });
    return temp_final_states[0];
}

/*
 First we push epsilon states in queue and loop through it to add it's reach state int queue and int set vector of
 Current map indx
*/
void EpsilonNFA::handleEpsilon(std::map<char, std::set<int>>& TransitionMap, const int indx) {
    if (const auto stateEpsilon = TransitionMap.find('\0'); stateEpsilon != TransitionMap.end()) {
        std::queue<int> EpsilonQueue;
        for (int element: TransitionMap.find('\0')->second) {
            EpsilonQueue.push(element);
        }
        std::map<int, bool> checkVisited;
        while (!EpsilonQueue.empty()) {
            int state = EpsilonQueue.front();
            EpsilonQueue.pop();
            auto it = checkVisited.find(state);
            if (state == indx || it != checkVisited.end()) { continue; }

            updateCurrentSt(state, indx);

            for (int element: TransitionMap.find('\0')->second) {
                EpsilonQueue.push(element);
            }
            checkVisited.insert(std::pair(state, true));
        }
    }
}

/*
This method used to update current state according if
it has in epsoln states for forwarding one
*/
void EpsilonNFA::updateCurrentSt(const int forwardState, const int currentIndx) {
    std::map<char, std::set<int>>& currentMap = TransitionTable[currentIndx];
    const std::map<char, std::set<int>>& forwardMap = TransitionTable[forwardState];
    for (const auto& pair: forwardMap) {
        if (currentMap.find(pair.first) != currentMap.end()) {
            std::set<int>& currentSt = currentMap.find(pair.first)->second;
            currentSt.insert(pair.second.begin(), pair.second.end());
        } else {
            currentMap.insert(std::pair(pair.first, pair.second));
        }
    }
}
