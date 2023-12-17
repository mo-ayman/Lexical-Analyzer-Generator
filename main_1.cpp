#include <iostream>
#include <unordered_map>

#include "LexicalAnalyzerGenerator/LexicalRules/RuleParser.h"
#include "LexicalAnalyzerGenerator/LexicalRules/RuleTree.h"
#include "LexicalAnalyzerGenerator/NFA/NFAConstruction.h"
#include "LexicalAnalyzerGenerator/DFA/DFA.h"
#include "LexicalAnalyzerGenerator/DFA/HelpingMethods.h"
#include "LexicalAnalyzerGenerator/DFAMinimizer/DFAMinimizer.h"
#include "LexicalAnalyzerGenerator/Visualization/Visualization.h"
#include "LexicalAnalyzerGenerator/DFASerialization/DFASerialization.h"

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

int main(const int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        return 1;
    }
    // Get dir path of the source code and concatenate with the file name
    std::string src_path = __FILE__;
    src_path = src_path.substr(0, src_path.find_last_of("\\/"));
    const auto rules_path = std::string(argv[1]);
    const std::vector<RuleTree *> rules = RuleParser::parseFromFile(rules_path);

    // Print all rules
    for (const auto& rule: rules) {
        rule->print();
    }
    std::cout << std::endl;

    auto* nfaConstruction = new NFAConstruction();
    nfaConstruction->constructNFA(rules);
    const std::vector<std::map<char, std::vector<int>>> nfs = nfaConstruction->getNfs();
    const std::unordered_map<int, std::tuple<std::string, Priority, int>> finalStates = nfaConstruction->
            getFinalStates();
    const int startStateIndex = nfaConstruction->getStartStateIndex();

    // Print the NFA table
    std::cout << "NFA table:" << std::endl;
    nfaConstruction->print();
    std::cout << std::endl;

    std::cout << "finalStates:" << std::endl;
    printFinalStates(finalStates);

    Visualization::exportNFAGraph(nfs, startStateIndex, finalStates, src_path + "/nfa_graph");
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
    constexpr int dfa_start_state = 0;
    Visualization::exportDfaTable(dfa, dfa_start_state, obj.getFinalStates(), src_path + "/dfa_table.txt");
    // -------------------------------------------------
    // Minimize the obtained DFA
    DFAMinimizer minimizer;
    minimizer.minimize(dfa, dfa_start_state, obj.getFinalStates());
    const auto min_dfa = minimizer.table;
    const auto min_dfa_start = minimizer.start;
    const auto min_dfa_fstates = minimizer.fstates;
    // -------------------------------------------------
    // Export visualizations
    Visualization::exportDfaGraph(min_dfa, min_dfa_start, min_dfa_fstates, src_path + "/min_dfa_graph");
    Visualization::exportDfaTable(min_dfa, min_dfa_start, min_dfa_fstates, src_path + "/min_dfa_table.txt");

    // Export the serialized min_dfa (for part 2)
    DFASerialization::exportDFA(min_dfa, min_dfa_start, min_dfa_fstates, src_path + "/min_dfa.dat");
    return 0;
}
