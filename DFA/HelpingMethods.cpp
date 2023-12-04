#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
#include "HelpingMethods.h"
#include "../LexicalRules/RuleTree.h"
#include <unordered_map>
using namespace std;

void HelpingMethods::printSet(const std::set<int>& mySet) {

    for (const auto& element : mySet) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}
void HelpingMethods::printVector(const std::vector<int>& myvec) {
    for (int elem : myvec)
    {
        std::cout << " " << elem << endl;
    }
}
void HelpingMethods::printvecMapSet(vector<map<char, set<int>>>& vec) {
    std::cout << "Elements in the map:" << std::endl;
    int indx = 0;
    for (const auto& myMap : vec) {
        std::cout << "Elements in the map:" << std::endl;
        // Loop through each map and print its key-value pairs
        for (const auto& pair : myMap) {
            std::cout << indx << ": " << pair.first << " -> ";
            for (int elem : pair.second)
            {
                std::cout << " " << elem;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        indx++;
    }
}
void HelpingMethods::finalMap(unordered_map<int, tuple<string, Priority, int>>& map) {
    std::cout << "Elements in the map:" << std::endl;

        // Loop through each map and print its key-value pairs
        for (const auto& pair : map) {
            std::cout << ": " << pair.first << " -> ";
            std::cout << std::get<0>(pair.second) << ", " << std::get<1>(pair.second) << ", " << std::get<2>(pair.second) << std::endl;
            
            std::cout << std::endl;
        }
        std::cout << std::endl;
   
}
// void HelpingMethods::printvecMapVec(vector<map<char, vector<int>>>& vec){

// }
