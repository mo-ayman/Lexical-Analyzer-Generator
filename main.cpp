#include <iostream>
#include <unordered_map>

#include "LexicalRules/RuleParser.h"
#include "LexicalRules/RuleTree.h"
#include "NFA/NFAConstruction.h"
#include "DFA/DFA.h"
#include "DFA/HelpingMethods.h"

int main() {
    // get dir path of the source code and concatenate with the file name
    std::string example_path = __FILE__;
    example_path = example_path.substr(0, example_path.find_last_of("\\/"));
    example_path += "/example.txt";
    const std::vector<RuleTree *> rules = RuleParser::parseFromFile(example_path);
    auto* nfaConstruction = new NFAConstruction();
    nfaConstruction->constructNFA(rules);
    std::vector<std::map<char, std::vector<int>>> nfs = nfaConstruction->getNfs();
    std::unordered_map<int, std::tuple<std::string, Priority, int>> finalStates = nfaConstruction->getFinalStates();
    int startStateIndex = nfaConstruction->getStartStateIndex();
    DFA obj(nfs, finalStates, startStateIndex);
    vector<map<char, int>> dfa = obj.getDFA();

    // Print all rules
    for (const auto& rule: rules) {
        rule->print();
    }

    // Print the NFA table
    std::cout << "NFA table:" << std::endl;
    nfaConstruction->print();

    std::cout << std::endl;


    // Print DFA table and final state
     std::cout << "DFA Taple:" << std::endl;
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
    HelpingMethods HM;
    cout << "Final States :  " << endl;
    unordered_map<int, tuple<string, Priority, int>>  mapFinal = obj.get_finalStates();
    HM.finalMap(mapFinal);




    return 0;
}

