#ifndef DFA_SERIALIZATION_H
#define DFA_SERIALIZATION_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

int export_DFA(std::vector<std::map<char, int>>& table, int& start, std::map<int, std::string>& final_states, const std::string& file_path);
int import_DFA(std::vector<std::map<char, int>>& loaded_table, int& loaded_start, std::map<int, std::string>& loaded_final_states, const std::string& file_path);

#endif // DFA_SERIALIZATION_H
