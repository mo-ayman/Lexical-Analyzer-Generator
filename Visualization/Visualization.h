#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "../LexicalRules/RuleTree.h"

namespace Visualization {
    void exportDfaTable(const std::vector<std::map<char, int>>& dfaTable, int startState,
                        const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates,
                        const std::string& filePath);

    void exportDfaGraph(const std::vector<std::map<char, int>>& dfaTable, int startState,
                        const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates,
                        const std::string& filePath);

    void exportNFAGraph(const std::vector<std::map<char, std::vector<int>>>& nfaTable, int startState,
                        const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finalStates,
                        const std::string& filePath);
}

#endif //VISUALIZATION_H
