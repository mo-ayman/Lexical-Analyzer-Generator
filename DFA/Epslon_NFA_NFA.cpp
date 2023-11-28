
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
    vector<int> finalStates;
    int initialState;
    vector<map<char, int>> DFA_States;

public:
    Epslon_NFA_NFA(vector<map<char, vector<int>>>& table, vector<int>& finals, int initial) {
        int indx = 0;
        for (map<char, vector<int>> TransitionMap : table)
        {
            map<char, set<int>> map;
            for (const auto& pair : TransitionMap) {
                std::set<int> currentSet(pair.second.begin(), pair.second.end());
                map.insert(std::pair<char, set<int>>(pair.first, currentSet));

            }
            TransitionTable.push_back(map);
        }
        finalStates = finals;
        initialState = initial;

    };

    vector<map<char, set<int>>> get_NFA()
    {
        int indx = 0;
        for (map<char, set<int>> TransitionMap : TransitionTable) {
            Handle_Epslon(TransitionMap, indx);
            
            //  if(indx ==0){
            //    cout<<"beof"<<endl;
            //    if (TransitionMap.find('0') != TransitionMap.end()){
            //         printSet(TransitionMap.find('0')->second);
                    
            //    }
            //    cout<<"beof   e"<<endl;
            //    if (TransitionMap.find('e') != TransitionMap.end()){
            //         printSet(TransitionMap.find('e')->second);
                    
            //    }
            //  }
            //Replace_state_Returned(TransitionMap, indx);
            
            //  if(indx ==0){
            //     cout<<"Affff"<<endl;
            //    if (TransitionMap.find('0') != TransitionMap.end()){
            //         printSet(TransitionMap.find('0')->second);
                    
            //    }
            //  }
            indx++;
        }
        indx = 0;
        for (map<char, set<int>>& TransitionMap : TransitionTable) {
            Replace_state_Returned(TransitionMap, indx);
            auto& mapToModify = TransitionTable[indx]; // Access the second map
            // Find the element and remove it
            auto it = mapToModify.find('e');
            if (it != mapToModify.end()) {
                mapToModify.erase(it);
            }
            indx++;
        }

        //cout << "fffffff  " << std::endl;
        return TransitionTable;
    };
    void Replace_state_Returned(map<char, set<int>>& TransitionMap, int indx) {
        for (auto& pair : TransitionMap) {
            for (int elem : pair.second) {
                if (elem == indx)
                {
                    auto it = TransitionMap.find('e');
                    if (it != TransitionMap.end()) {

                        cout << "before :  " << pair.first << " : " << endl;
                        printSet(pair.second);
                        set<int>& set1 = pair.second;
                        set1.insert(it->second.begin(), it->second.end());
                        printSet(pair.second);
                    }
                }
            }

        }
        //cout << "wwwwww  " << std::endl;
    }
    
    void Handle_Epslon(map<char, set<int>>& TransitionMap, int indx) {
        auto stateEpslon = TransitionMap.find('e');
        if (stateEpslon != TransitionMap.end()) {
            //set<int> epslon_Vec = TransitionMap.find('e')->second;
            std::queue<int> EpslonQueue;/////////////////////////////////
            for (int element : TransitionMap.find('e')->second) {
               EpslonQueue.push(element);
            }
            map<int, bool> checkVisited;
            while (!EpslonQueue.empty())
            {
                int state =EpslonQueue.front();
                EpslonQueue.pop();
                auto it = checkVisited.find(state);
                if (state == indx || it != checkVisited.end()) { continue; }
                
                UpdateCurrent_st(state, indx);
                
                for (int element : TransitionMap.find('e')->second) {
                    EpslonQueue.push(element);
                 }
               // epslon_Vec = TransitionMap.find('e')->second;
                checkVisited.insert(std::pair<int, bool>(state, true));
            }
            // cout<<"hmmmmmoooo  "<<endl;
            //     if(indx==0){
            //        printSet(TransitionMap.find('e')->second);
            //     }
            // cout<<"hmmmmmoooo  "<<endl;
        }

    }
    void UpdateCurrent_st(int forwardState, int currentIndx)
    {
       // cout << "wwwwww  " << std::endl;
        map<char, set<int>>& currentMap = TransitionTable[currentIndx];
        map<char, set<int>>& forwardMap = TransitionTable[forwardState];
        for (const auto& pair : forwardMap) {
            if (currentMap.find(pair.first) != currentMap.end()) {
                set<int>& currentSt = currentMap.find(pair.first)->second;
                currentSt.insert(pair.second.begin(), pair.second.end());
            }
            else {
                currentMap.insert(std::pair<char, set<int>>(pair.first, pair.second));
            }

        }
    }
    
    void printSet(const std::set<int>& mySet) {
        //std::cout << "ABBBBooo : ";
        for (const auto& element : mySet) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

};

int main() {
    // Example transition table as an unordered map
    vector<map<char, vector<int>>> Ttable;
    map<char, vector<int>> myMap{
    {'0',{0}},
    {'e',{1}}
    };
    map<char, vector<int>> myMap2{
    {'1',{1}},
    {'e',{2}}
    };
    map<char, vector<int>> myMap3{
    {'0',{2}},
    {'1',{2}}
    };
    Ttable.push_back(myMap);
    Ttable.push_back(myMap2);
    Ttable.push_back(myMap3);
    vector<int> finalStates;
    int initialState = 0;

    Epslon_NFA_NFA obj(Ttable, finalStates, initialState);
    vector<map<char, set<int>>> dfa = obj.get_NFA();


    std::cout << "Elements in the map:" << std::endl;
    // Loop through the vector of maps
    int indx = 0;
    for (const auto& myMap : dfa) {
        std::cout << "Elements in the map:" << std::endl;
        // Loop through each map and print its key-value pairs
        for (const auto& pair : myMap) {
            std::cout << indx << ": " << pair.first<<" -> " ;
            for (int elem : pair.second)
            {
                std::cout << " " << elem;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        indx++;
    }


    return 0;
}
