#include <iostream>
#include <unordered_map>

#include "LexicalRules/RuleParser.cpp"
#include "LexicalRules/Node.cpp"
#include "LexicalRules/RuleTree.cpp"
#include "NFA/NFAConstruction.cpp"
#include "DFA/DFA.cpp"
#include "DFA/HelpingMethods.cpp"
#include "DFA_MIN/DFA_minimizer.cpp"

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
    // -------------------------------------------------
    // NFA -> DFA
    DFA obj(nfs, finalStates, startStateIndex);
    vector<map<char, int>> dfa = obj.getDFA();
    HelpingMethods HM;
    // Print all rules
    for (const auto& rule: rules) {
        rule->print();
    }

    //Print the NFA table
    std::cout << "NFA table:" << std::endl;
    nfaConstruction->print();
    HM.finalMap(finalStates);
    std::cout << std::endl;

    
    // // Print DFA table and final state
    // std::cout << "DFA Taple:" << std::endl;
    
    // HM.printvecMapInt(dfa);
    // cout << "Final States :  " << endl;
    // unordered_map<int, tuple<string, Priority, int>>  mapFinal = obj.get_finalStates();
    // HM.finalMap(mapFinal);

    // int dfa_start_state = 0; //TODO: assign the correct start state index for dfa
    // // -------------------------------------------------
    // // Minimize the obtained DFA
    // DFA_minimizer minimizer;
    // minimizer.minimize(dfa, dfa_start_state, obj.get_finalStates());
    // auto min_dfa = minimizer.table;
    // auto min_dfa_start = minimizer.start;
    // auto min_dfa_fstates = minimizer.fstates;

    // // Print DFA table and final state
    // std::cout << "DFA_minimized Taple:" << std::endl;
    // HM.printvecMapInt(min_dfa);

    //-------------------------------------------------
    // TODO: export visualizations
    // TODO: export the serialized min_dfa (for part 2)
    return 0;
}

