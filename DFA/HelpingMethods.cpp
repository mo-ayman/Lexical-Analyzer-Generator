#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>

#include "HelpingMethods.h"
#include "../LexicalRules/RuleTree.h"

namespace HelpingMethods {
    void printSet(const std::set<int>& mySet) {
        for (const auto& element: mySet) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    void printVector(const std::vector<int>& myvec) {
        for (const int elem: myvec) {
            std::cout << " " << elem << std::endl;
        }
    }

    void printvecMapSet(std::vector<std::map<char, std::set<int>>>& vec) {
        std::cout << "Elements in the map:" << std::endl;
        int indx = 0;
        for (const auto& myMap: vec) {
            std::cout << "Elements in the map:" << std::endl;
            // Loop through each map and print its key-value pairs
            for (const auto& pair: myMap) {
                std::cout << indx << ": " << pair.first << " -> ";
                for (const int elem: pair.second) {
                    std::cout << " " << elem;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            indx++;
        }
    }

    void finalMap(const std::unordered_map<int, std::tuple<std::string, Priority, int>>& map) {
        std::cout << "Elements in the map:" << std::endl;

        // Loop through each map and print its key-value pairs
        for (const auto& pair: map) {
            std::cout << ": " << pair.first << " -> ";
            std::cout << std::get<0>(pair.second) << ", " << std::get<1>(pair.second) << ", " << std::get<2>(pair.second) << std::endl;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void printvecMapInt(const std::vector<std::map<char, int>>& vec) {
        int indx = 0;
        for (const auto& myMap: vec) {
            std::cout << indx;
            std::cout << "  Elements in the map:" << std::endl;
            // Loop through each map and print its key-value pairs
            for (const auto& pair: myMap) {
                std::cout << ": " << pair.first << " -> " << pair.second;
            }
            std::cout << std::endl;
            std::cout << std::endl;
            indx++;
            if (indx > 20) { break; }
        }
    }
}
