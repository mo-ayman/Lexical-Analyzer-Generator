#include <iostream>
#include <unordered_map>

#include "LexicalRules/RuleParser.cpp"
#include "LexicalRules/Node.cpp"
#include "LexicalRules/RuleTree.cpp"
#include "NFA/NFAConstruction.cpp"
#include "DFA/DFA.cpp"
#include "DFA/HelpingMethods.cpp"
#include "DFA_MIN/DFA_minimizer.cpp"
#include "Visualization/visualization.cpp"
#include "DFA_Serialization/DFA_serialization.cpp"

void printFinalStates(const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates) {
    for (const auto& entry : finalStates) {
        int key = entry.first;
        const std::tuple<std::string, Priority, int>& value = entry.second;

        std::cout << "Key: " << key << ", Value: ("
                  << std::get<0>(value) << ", "
                  << static_cast<int>(std::get<1>(value)) << ", "
                  << std::get<2>(value) << ")\n";
    }
}
int main() {
    // get dir path of the source code and concatenate with the file name
    std::string example_path = __FILE__;
    example_path = example_path.substr(0, example_path.find_last_of("\\/"));
    example_path += "/example.txt";
    const std::vector<RuleTree *> rules = RuleParser::parseFromFile(example_path);
    
    auto* nfaConstruction = new NFAConstruction();
    nfaConstruction->constructNFA({rules[0]});
    std::vector<std::map<char, std::vector<int>>> nfs = nfaConstruction->getNfs();
    std::unordered_map<int, std::tuple<std::string, Priority, int>> finalStates = nfaConstruction->getFinalStates();
    int startStateIndex = nfaConstruction->getStartStateIndex();

    std::cout << "finalStates:" << std::endl;
    printFinalStates(finalStates);

    exportNFAGraph(nfs, startStateIndex, finalStates, "nfa_graph_0");
    // -------------------------------------------------


    // NFA -> DFA
    DFA obj(nfs, finalStates, startStateIndex);
    vector<map<char, int>> dfa = obj.getDFA();

    // // Print all rules
    // for (const auto& rule: rules) {
    //     rule->print();
    // }

    // // Print the NFA table
    // std::cout << "NFA table:" << std::endl;
    // nfaConstruction->print();
    // std::cout << std::endl;

    
    // Print DFA table and final state
    HelpingMethods HM;
    unordered_map<int, tuple<string, Priority, int>>  mapFinal = obj.get_finalStates();

    // std::cout << "DFA Taple:" << std::endl;
    // HM.printvecMapInt(dfa);
    // cout << "Final States :  " << endl;
    // HM.finalMap(mapFinal);
    
    std::cout << "mapFinal:" << std::endl;
    printFinalStates(mapFinal);
    int dfa_start_state = 0; //TODO: assign the correct start state index for dfa
    
    exportDfaTable(dfa, dfa_start_state, obj.get_finalStates(), "dfa_table.txt");
    // -------------------------------------------------
    // Minimize the obtained DFA
    DFA_minimizer minimizer;
    minimizer.minimize(dfa, dfa_start_state, obj.get_finalStates());
    auto min_dfa = minimizer.table;
    auto min_dfa_start = minimizer.start;
    auto min_dfa_fstates = minimizer.fstates;

    // Print DFA table and final state
    std::cout << "min_dfa_fstates:" << std::endl;
    printFinalStates(min_dfa_fstates);
    std::cout << "DFA_minimized Taple:" << std::endl;
    // HM.printvecMapInt(min_dfa);

    // -------------------------------------------------
    // Export visualizations
    exportDfaGraph(min_dfa, min_dfa_start, min_dfa_fstates, "min_dfa_graph");
    exportDfaTable(min_dfa, min_dfa_start, min_dfa_fstates, "min_dfa_table.txt");

    // Export the serialized min_dfa (for part 2)
    // export_DFA(min_dfa, min_dfa_start, min_dfa_fstates, "min_dfa.dat");
    return 0;
}

