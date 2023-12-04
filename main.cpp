#include <iostream>
#include <unordered_map>

#include "LexicalRules/RuleParser.h"
#include "LexicalRules/RuleTree.h"
#include "NFA/NFAConstruction.h"

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


    // Print all rules
    for (const auto& rule: rules) {
        rule->print();
    }

    // Print the NFA table
    std::cout << "NFA table:" << std::endl;
    nfaConstruction->print();

    std::cout << std::endl;

    return 0;
}

