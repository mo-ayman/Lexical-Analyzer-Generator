#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <unordered_map>
#include "../LexicalRules/RuleTree.h"
#include <set>
using namespace std;

void exportDfaTable(const vector<map<char, int>>& dfaTable, int startState, const unordered_map<int, tuple<string, Priority, int>>& finalStates, const string& filePath) {
    ofstream outFile(filePath);

    if (!outFile.is_open()) {
        cerr << "Error: Unable to open the file for writing." << endl;
        return;
    }

    // 1. Obtain all input characters in an ordered set
    set<char> allInputs;
    for (const auto& state : dfaTable) {
        for (const auto& entry : state) {
            allInputs.insert(entry.first);
        }
    }

    // 2. Write the DFA table to the file
    outFile << setw(5) << "State";
    for (const auto& input : allInputs) {
        outFile << setw(5) << input;
    }
    outFile << "\n";

    for (int i = 0; i < dfaTable.size(); ++i) {
        outFile << setw(5) << i;
        for (const auto& input : allInputs) {
            outFile << setw(5);
            auto transition = dfaTable[i].find(input);
            if (transition != dfaTable[i].end()) {
                outFile << transition->second;
            } else {
                outFile << "";
            }
        }
        outFile << "\n";
    }

    outFile.close();
    cout << "DFA table written to: " << filePath << endl;
}

void exportDfaGraph(const vector<map<char, int>>& dfaTable, int startState, const unordered_map<int, tuple<string, Priority, int>>& finalStates, const string& filePath) {
    ofstream outFile(filePath);

    if (!outFile.is_open()) {
        cerr << "Error: Unable to open the file for writing." << endl;
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
            outFile << " [shape=doublecircle, label=\"" << get<0>(finalStates.at(i)) << "\", color=blue]";
        } else {
            outFile << " [shape=circle]";
        }
        outFile << ";\n";
    }

    // 3. Add transitions
    for (int i = 0; i < dfaTable.size(); ++i) {
        for (const auto& entry : dfaTable[i]) {
            outFile << "  " << i << " -> " << entry.second << " [label=\"" << entry.first << "\"];\n";
        }
    }

    outFile << "}\n";
    outFile.close();

    // 4. Generate the image using Graphviz https://graphviz.gitlab.io/download/
    string command = "dot -Tpng " + filePath + " -o " + filePath + ".png";
    int result = system(command.c_str());

    if (result == 0) {
        cout << "DFA graph exported to: " << filePath << ".png" << endl;
    } else {
        cerr << "Error: Unable to generate the image. Make sure Graphviz is installed." << endl;
    }
}

int main() {
    // Example DFA
    vector<map<char, int>> dfaTable = {
        {{'a', 1}, {'b', 2}},
        {{'a', 1}, {'b', 3}},
        {{'b', 2}},
        {{'a', 4}, {'b', 3}},
        {{'a', 4}, {'b', 2}}
    };
    int startState = 0;
    unordered_map<int, tuple<string, Priority, int>> finalStates = {
        {2, make_tuple("x",Priority(2),0)},
        {3, make_tuple("y",Priority(2),1)},
        {4, make_tuple("z",Priority(2),2)}
    };

    // Write DFA table to a file
    string table_filePath = "dfa_table_export.txt";
    exportDfaTable(dfaTable, startState, finalStates, table_filePath);


    // Export the DFA graph to an image
    string graph_filePath = "dfa_graph_export";
    exportDfaGraph(dfaTable, startState, finalStates, graph_filePath);

    return 0;
}
