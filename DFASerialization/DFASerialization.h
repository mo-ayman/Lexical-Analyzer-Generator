#ifndef DFA_SERIALIZATION_H
#define DFA_SERIALIZATION_H

#include <vector>
#include <map>

#include "../LexicalRules/RuleTree.h"

namespace DFASerialization {
    int exportDFA(const std::vector<std::map<char, int>>& table, const int& start,
                  const std::unordered_map<int, std::tuple<std::string, Priority, int>>& final_states,
                  const std::string& file_path);

    int importDFA(std::vector<std::map<char, int>>& loaded_table, int& loaded_start,
                  std::unordered_map<int, std::tuple<std::string, Priority, int>>& loaded_final_states,
                  const std::string& file_path);
}

#endif // DFA_SERIALIZATION_H
