#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include "DFA.h"
#include "Epslon_NFA_NFA.h"
#include "HelpingMethods.h"

int main() {
    HelpingMethods HM;
    // Example transition table as an unordered map
    vector<map<char, vector<int>>> transitionTable;
    map<char, vector<int>> myMap{
    {'\0',{1,7}}
    };
    map<char, vector<int>> myMap1{
    {'\0',{2,4}}
    };
    map<char, vector<int>> myMap2{
    {'a',{3}}
    };
    map<char, vector<int>> myMap3{
    {'\0',{6}}
    };
    map<char, vector<int>> myMap4{
    {'b',{5}}
    };
    map<char, vector<int>> myMap5{
    {'\0',{6}}
    };
    map<char, vector<int>> myMap6{
    {'\0',{1,7}}
    };
    map<char, vector<int>> myMap7{
    {'a',{8}}
    };
    map<char, vector<int>> myMap8{
    {'b',{9}}
    };
    map<char, vector<int>> myMap9{
    {'b',{10}}
    };
    map<char, vector<int>> myMap10{

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
    transitionTable.push_back(myMap9);
    transitionTable.push_back(myMap10);
    map<int, string> finalStates{ {10,"(a|b)*abb"} };
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
    map<int, string> mapFinal = obj.get_finalStates();
    HM.finalMap(mapFinal);

    return 0;
}