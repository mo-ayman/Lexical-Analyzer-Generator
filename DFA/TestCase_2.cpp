#include <iostream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include "DFA.cpp"
#include "HelpingMethods.cpp"
#include "../LexicalRules/RuleTree.h"

int main() {
    HelpingMethods HM;
    // Example transition table as an unordered map
    vector<map<char, vector<int>>> transitionTable;
    map<char, vector<int>> myMap{
    {'\0',{1,3,7}}
    };
    map<char, vector<int>> myMap1{
    {'a',{2}}
    };
    map<char, vector<int>> myMap2{
    
    };
    map<char, vector<int>> myMap3{
    {'a',{4}}
    };
    map<char, vector<int>> myMap4{
    {'b',{5}}
    };
    map<char, vector<int>> myMap5{
    {'b',{6}}
    };
    map<char, vector<int>> myMap6{
  
    };
    map<char, vector<int>> myMap7{
    {'b',{8}},
    {'a',{7}}
    };
    map<char, vector<int>> myMap8{
    {'b',{8}}
    };
   
    transitionTable.push_back(myMap);
    transitionTable.push_back(myMap1);
    transitionTable.push_back(myMap2);
    transitionTable.push_back(myMap3);
    transitionTable.push_back(myMap4);
    transitionTable.push_back(myMap5);
    transitionTable.push_back(myMap6);
    transitionTable.push_back(myMap7);
    transitionTable.push_back(myMap8);

    unordered_map<int, tuple<string, Priority, int>> finalStates{ {2,make_tuple("a",RESERVED,1)},
    {6,make_tuple("abb",RESERVED,2)},
    {8,make_tuple("a*b+",RESERVED,3)} };
    int initialState = 0;
    DFA obj(transitionTable, finalStates, initialState);
    vector<map<char, int>> dfa = obj.getDFA();

    
    std::cout << "Elements in the map:" << std::endl;
    
    // Loop through the vector of maps
    int indx = 0;
    for (const auto& myMap : dfa) {
        std::cout << "Elements in the map:" << std::endl;
        // Loop through each map and print its key-value pairs
        for (const auto& pair : myMap) {
            std::cout << indx << ": " << pair.first << " " << pair.second << std::endl;
        }
        std::cout << std::endl;
        indx++;
        
    }
    
    cout << "Final States :  " << endl;
    unordered_map<int, tuple<string, Priority, int>>  mapFinal = obj.get_finalStates();
    HM.finalMap(mapFinal);

    return 0;
}