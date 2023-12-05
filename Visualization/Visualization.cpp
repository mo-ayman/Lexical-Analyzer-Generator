#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>

#include "Visualization.h"
#include "../LexicalRules/RuleTree.h"

namespace Visualization {
    void exportDfaTable(const std::vector<std::map<char, int>>& dfaTable, int startState,
                        const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates,
                        const std::string& filePath) {
        std::ofstream outFile(filePath);

        if (!outFile.is_open()) {
            std::cerr << "Error: Unable to open the file for writing." << std::endl;
            return;
        }

        // 1. Obtain all input characters in an ordered set
        std::set<char> allInputs;
        for (const auto& state: dfaTable) {
            for (const auto& entry: state) {
                allInputs.insert(entry.first);
            }
        }

        // 2. Write the DFA table to the file
        outFile << std::setw(5) << "State" << " ";
        for (const auto& input: allInputs) {
            outFile << std::setw(5) << input;
        }
        outFile << '\n';

        for (int i = 0; i < dfaTable.size(); ++i) {
            // Mark final states with an asterisk
            if (finalStates.find(i) != finalStates.end()) {
                outFile << std::setw(5) << i << "*";
            } else {
                outFile << std::setw(5) << i << " ";
            }
            for (const auto& input: allInputs) {
                outFile << std::setw(5);

                if (const auto transition = dfaTable[i].find(input); transition != dfaTable[i].end()) {
                    outFile << transition->second;
                }
            }
            outFile << '\n';
        }

        outFile.close();
        std::cout << "DFA table written to: " << filePath << std::endl;
    }

    void exportDfaGraph(const std::vector<std::map<char, int>>& dfaTable, int startState,
                        const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates,
                        const std::string& filePath) {
        std::ofstream outFile(filePath);

        if (!outFile.is_open()) {
            std::cerr << "Error: Unable to open the file for writing." << std::endl;
            return;
        }

        // 1. Write the DOT language representation
        outFile << "digraph DFA {\n";
        outFile << "  rankdir=LR;\n";

        // Add hidden node for dangling arrow to start state
        outFile << "  hidden_node [style=invis, width=0, height=0, label=\"\"];\n";
        outFile << "  hidden_node -> " << startState << ";\n";

        // Add nodes
        for (int i = 0; i < dfaTable.size(); ++i) {
            outFile << "  " << i;
            if (i == startState) {
                outFile << " [shape=circle, color=red]";
            } else if (finalStates.find(i) != finalStates.end()) {
                outFile << " [shape=doublecircle, label=\"" << i << "\\n'" << std::get<0>(finalStates.at(i)) <<
                        "'\", color=blue]";
            } else {
                outFile << " [shape=circle]";
            }
            outFile << ";\n";
        }

        // 3. Add transitions
        for (int i = 0; i < dfaTable.size(); ++i) {
            for (const auto& entry: dfaTable[i]) {
                outFile << "  " << i << " -> " << entry.second << " [label=\"" << entry.first << "\"];\n";
            }
        }

        outFile << "}\n";
        outFile.close();

        // 4. Generate the image using Graphviz https://graphviz.gitlab.io/download/
        const std::string command = "dot -Tpng " + filePath + " -o " + filePath + ".png";

        if (system(command.c_str()) == 0) {
            std::cout << "DFA graph exported to: " << filePath << ".png" << std::endl;
        } else {
            std::cerr << "Error: Unable to generate the image. Make sure Graphviz is installed." << std::endl;
        }
    }


    void exportNFAGraph(const std::vector<std::map<char, std::vector<int>>>& nfaTable, int startState,
                        const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates,
                        const std::string& filePath) {
        std::ofstream outFile(filePath);

        if (!outFile.is_open()) {
            std::cerr << "Error: Unable to open the file for writing." << std::endl;
            return;
        }

        // 1. Write the DOT language representation
        outFile << "digraph NFA {\n";
        outFile << "  rankdir=LR;\n";

        // 2. Add nodes
        outFile << "  hidden_node [style=invis, width=0, height=0, label=\"\"];\n";
        outFile << "  hidden_node -> " << startState << ";\n";

        for (int i = 0; i < nfaTable.size(); ++i) {
            outFile << "  " << i;
            if (i == startState) {
                outFile << " [shape=circle, color=red]";
            } else if (finalStates.find(i) != finalStates.end()) {
                outFile << " [shape=doublecircle, label=\"" << i << "\\n'" << std::get<0>(finalStates.at(i)) <<
                        "'\", color=blue]";
            } else {
                outFile << " [shape=circle]";
            }
            outFile << ";\n";
        }

        // 3. Add transitions
        for (int i = 0; i < nfaTable.size(); ++i) {
            for (const auto& entry: nfaTable[i]) {
                for (const auto& targetState: entry.second) {
                    if (const char c = entry.first; c == '\0') {
                        outFile << "  " << i << " -> " << targetState << " [label=\"" << "eps" << "\"];\n";
                    } else {
                        outFile << "  " << i << " -> " << targetState << " [label=\"" << c << "\"];\n";
                    }
                }
            }
        }

        outFile << "}\n";
        outFile.close();

        // 4. Generate the image using Graphviz https://graphviz.gitlab.io/download/
        const std::string command = "dot -Tpng " + filePath + " -o " + filePath + ".png";

        if (system(command.c_str()) == 0) {
            std::cout << "NFA graph exported to: " << filePath << ".png" << std::endl;
        } else {
            std::cerr << "Error: Unable to generate the image. Make sure Graphviz is installed." << std::endl;
        }
    }
}
