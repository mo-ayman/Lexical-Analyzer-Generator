#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "DFASerialization.h"
#include "../LexicalRules/RuleTree.h"

// Serialization for vector<map<char, int>>
static void serialize(const std::vector<std::map<char, int>>& v, std::ostream& os) {
    const size_t size = v.size();
    os.write(reinterpret_cast<const char *>(&size), sizeof(size));

    for (const auto& m: v) {
        size_t mapSize = m.size();
        os.write(reinterpret_cast<const char *>(&mapSize), sizeof(mapSize));
        for (const auto& entry: m) {
            os.write(&entry.first, sizeof(entry.first));
            os.write(reinterpret_cast<const char *>(&entry.second), sizeof(entry.second));
        }
    }
}

// Deserialization for vector<map<char, int>>
static void deserialize(std::vector<std::map<char, int>>& v, std::istream& is) {
    size_t size;
    is.read(reinterpret_cast<char *>(&size), sizeof(size));

    for (size_t i = 0; i < size; ++i) {
        size_t mapSize;
        is.read(reinterpret_cast<char *>(&mapSize), sizeof(mapSize));

        std::map<char, int> m;
        for (size_t j = 0; j < mapSize; ++j) {
            char key;
            int value;
            is.read(&key, sizeof(key));
            is.read(reinterpret_cast<char *>(&value), sizeof(value));
            m[key] = value;
        }

        v.push_back(m);
    }
}

// Serialization for map<int, string>
static void serialize(const std::unordered_map<int, std::tuple<std::string, Priority, int>>& m, std::ostream& os) {
    const size_t mapSize = m.size();
    os.write(reinterpret_cast<const char *>(&mapSize), sizeof(mapSize));
    for (const auto& entry: m) {
        os.write(reinterpret_cast<const char *>(&entry.first), sizeof(entry.first));

        size_t strSize = std::get<0>(entry.second).size();
        os.write(reinterpret_cast<const char *>(&strSize), sizeof(strSize));
        os.write(std::get<0>(entry.second).c_str(), static_cast<long long>(strSize));
        os.write(reinterpret_cast<const char *>(&std::get<1>(entry.second)), sizeof(std::get<1>(entry.second)));
        os.write(reinterpret_cast<const char *>(&std::get<2>(entry.second)), sizeof(std::get<2>(entry.second)));
    }
}

// Deserialization for map<int, string>
static void deserialize(std::unordered_map<int, std::tuple<std::string, Priority, int>>& m, std::istream& is) {
    size_t size;
    is.read(reinterpret_cast<char *>(&size), sizeof(size));

    for (size_t i = 0; i < size; ++i) {
        int key;
        is.read(reinterpret_cast<char *>(&key), sizeof(key));

        size_t strSize;
        is.read(reinterpret_cast<char *>(&strSize), sizeof(strSize));
        std::string str(strSize, '\0');
        is.read(&str[0], static_cast<long long>(strSize));

        Priority p;
        is.read(reinterpret_cast<char *>(&p), sizeof(p));

        int idx;
        is.read(reinterpret_cast<char *>(&idx), sizeof(idx));

        m[key] = std::make_tuple(str, p, idx);
    }
}

namespace DFASerialization {
    // DFA Serialization
    int exportDFA(const std::vector<std::map<char, int>>& table, const int& start,
                  const std::unordered_map<int, std::tuple<std::string, Priority, int>>& final_states,
                  const std::string& file_path) {
        if (std::ofstream outFile(file_path, std::ios::binary); outFile.is_open()) {
            serialize(table, outFile);
            outFile.write(reinterpret_cast<const char *>(&start), sizeof(start));
            serialize(final_states, outFile);
            outFile.close();
            return 0;
        }
        std::cerr << "Error opening '" << file_path << "' for writing." << std::endl;
        return 1;
    }

    // DFA Deserialization
    int importDFA(std::vector<std::map<char, int>>& loaded_table, int& loaded_start,
                  std::unordered_map<int, std::tuple<std::string, Priority, int>>& loaded_final_states,
                  const std::string& file_path) {
        std::ifstream inFile(file_path, std::ios::binary);
        if (inFile.is_open()) {
            deserialize(loaded_table, inFile);
            inFile.read(reinterpret_cast<char *>(&loaded_start), sizeof(loaded_start));
            deserialize(loaded_final_states, inFile);
            inFile.close();
            return 0;
        }
        std::cerr << "Error opening '" << file_path << "' for reading." << std::endl;
        return 1;
    }
}
