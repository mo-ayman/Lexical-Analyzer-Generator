#include <iostream>
#include <unordered_map>

#include "LexicalRules/RuleParser.h"
#include "LexicalRules/RuleTree.h"
#include "NFA/NFAConstruction.h"
#include "DFA/DFA.h"
#include "DFA/HelpingMethods.h"
#include "DFAMinimizer/DFAMinimizer.h"
#include "Visualization/Visualization.h"
#include "DFASerialization/DFASerialization.h"

void printFinalStates(const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates) {
    for (const auto& entry: finalStates) {
        const int key = entry.first;
        const std::tuple<std::string, Priority, int>& value = entry.second;

        std::cout << "Key: " << key << ", Value: ("
                << std::get<0>(value) << ", "
                << std::get<1>(value) << ", "
                << std::get<2>(value) << ")\n";
    }
}

int main() {
    // get dir path of the source code and concatenate with the file name
    std::string example_path = __FILE__;
    example_path = example_path.substr(0, example_path.find_last_of("\\/"));
    example_path += "/example.txt";
    const std::vector<RuleTree *> rules = RuleParser::parseFromFile(example_path);

    // Print all rules
    for (const auto& rule: rules) {
        rule->print();
    }
    std::cout << std::endl;

    auto* nfaConstruction = new NFAConstruction();
    nfaConstruction->constructNFA({rules[0]});
    const std::vector<std::map<char, std::vector<int>>> nfs = nfaConstruction->getNfs();
    const std::unordered_map<int, std::tuple<std::string, Priority, int>> finalStates = nfaConstruction->getFinalStates();
    const int startStateIndex = nfaConstruction->getStartStateIndex();

    // Print the NFA table
    std::cout << "NFA table:" << std::endl;
    nfaConstruction->print();
    std::cout << std::endl;


    std::cout << "finalStates:" << std::endl;
    printFinalStates(finalStates);

    Visualization::exportNFAGraph(nfs, startStateIndex, finalStates, "nfa_graph_0");
    // -------------------------------------------------

    // NFA -> DFA
    DFA obj(nfs, finalStates, startStateIndex);
    const std::vector<std::map<char, int>> dfa = obj.getDFA();

    // Print DFA table and final state
    const std::unordered_map<int, std::tuple<std::string, Priority, int>> mapFinal = obj.getFinalStates();

    std::cout << "DFA Taple:" << std::endl;
    HelpingMethods::printvecMapInt(dfa);
    std::cout << "Final States :  " << std::endl;
    HelpingMethods::finalMap(mapFinal);

    std::cout << "mapFinal:" << std::endl;
    printFinalStates(mapFinal);
    constexpr int dfa_start_state = 0; // TODO: assign the correct start state index for dfa

    Visualization::exportDfaTable(dfa, dfa_start_state, obj.getFinalStates(), "dfa_table.txt");
    // -------------------------------------------------
    // Minimize the obtained DFA
    DFAMinimizer minimizer;
    minimizer.minimize(dfa, dfa_start_state, obj.getFinalStates());
    const auto min_dfa = minimizer.table;
    const auto min_dfa_start = minimizer.start;
    const auto min_dfa_fstates = minimizer.fstates;

    // Print DFA table and final state
    std::cout << "min_dfa_fstates:" << std::endl;
    printFinalStates(min_dfa_fstates);
    std::cout << "DFA_minimized Taple:" << std::endl;
    // HM.printvecMapInt(min_dfa);

    // -------------------------------------------------
    // Export visualizations
    Visualization::exportDfaGraph(min_dfa, min_dfa_start, min_dfa_fstates, "min_dfa_graph");
    Visualization::exportDfaTable(min_dfa, min_dfa_start, min_dfa_fstates, "min_dfa_table.txt");

    // Export the serialized min_dfa (for part 2)
    DFASerialization::exportDFA(min_dfa, min_dfa_start, min_dfa_fstates, "min_dfa.dat");
    return 0;
}
