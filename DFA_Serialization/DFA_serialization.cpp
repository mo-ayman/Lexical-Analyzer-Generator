#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include "../LexicalRules/RuleTree.h"
using namespace std;

// Serialization for vector<map<char, int>>
void serialize(const vector<map<char, int>>& v, ostream& os) {
    size_t size = v.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& m : v) {
        size_t mapSize = m.size();
        os.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
        for (const auto& entry : m) {
            os.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
            os.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
        }
    }
}

// Deserialization for vector<map<char, int>>
void deserialize(vector<map<char, int>>& v, istream& is) {
    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));

    for (size_t i = 0; i < size; ++i) {
        size_t mapSize;
        is.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

        map<char, int> m;
        for (size_t j = 0; j < mapSize; ++j) {
            char key;
            int value;
            is.read(reinterpret_cast<char*>(&key), sizeof(key));
            is.read(reinterpret_cast<char*>(&value), sizeof(value));
            m[key] = value;
        }

        v.push_back(m);
    }
}

// Serialization for map<int, string>
void serialize(const unordered_map<int, tuple<string, Priority, int>>& m, ostream& os) {
    size_t mapSize = m.size();
    os.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
    for (const auto& entry : m) {
        os.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));

        size_t strSize = get<0>(entry.second).size();
        os.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
        os.write(get<0>(entry.second).c_str(), strSize);
        os.write(reinterpret_cast<const char*>(&get<1>(entry.second)), sizeof(get<1>(entry.second)));
        os.write(reinterpret_cast<const char*>(&get<2>(entry.second)), sizeof(get<2>(entry.second)));
    }
}

// Deserialization for map<int, string>
void deserialize(unordered_map <int, tuple<string, Priority, int>>& m, istream& is) {
    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));

    for (size_t i = 0; i < size; ++i) {
        int key;
        is.read(reinterpret_cast<char*>(&key), sizeof(key));

        size_t strSize;
        is.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
        string str(strSize, '\0');
        is.read(&str[0], strSize);
        
        Priority p;
        is.read(reinterpret_cast<char*>(&p), sizeof(p));
        
        int idx;
        is.read(reinterpret_cast<char*>(&idx), sizeof(idx));

        m[key] = make_tuple(str, p, idx) ;
    }
}

// DFA Serialization
int export_DFA(vector<map<char, int>>& table, int& start, unordered_map<int, tuple<string, Priority, int>>& final_states, string file_path){
    ofstream outFile(file_path, ios::binary);
    if (outFile.is_open()) {
        serialize(table, outFile);
        outFile.write(reinterpret_cast<const char*>(&start), sizeof(start));
        serialize(final_states, outFile);
        outFile.close();
        return 0;
    } else {
        cerr << "Error opening '" << file_path << "' for writing." << endl;
        return 1;
    }
}

// DFA Deserialization
int import_DFA(vector<map<char, int>>& loaded_table, int& loaded_start, unordered_map<int, tuple<string, Priority, int>>& loaded_final_states, string file_path){
    ifstream inFile(file_path, ios::binary);
    if (inFile.is_open()) {
        deserialize(loaded_table, inFile);
        inFile.read(reinterpret_cast<char*>(&loaded_start), sizeof(loaded_start));
        deserialize(loaded_final_states, inFile);
        inFile.close();
        return 0;
    } else {
        cerr << "Error opening '" << file_path << "' for reading." << endl;
        return 1;
    }
}

// int main() {
//     // Sample DFA {v,x,f}
//     vector<map<char, int>> v = {
//         {{'a', 4}, {'b', 1}}, // x 0
//         {{'a', 3}, {'b', 2}}, // z 1
//         {{'a', 4}, {'b', 5}}, // w 2
//         {{'a', 4}, {'b', 6}}, // u 3
//         {{'a', 4}},           // y,T 4
//         {{'a', 4}, {'b', 5}}, // k 5
//         {}, // M 6
//     };
//     int x = 0;
//     map<int, string> f = {
//         {3, "P3"},
//         {6, "P2"},
//         {2, "p1"},
//         {4, "p3"}
//     };

//     // Exporting
//     export_DFA(v,x,f,"2018_q2.dat");

//     // Importing
//     vector<map<char, int>> loadedV;
//     int loadedX;
//     map<int, string> loadedF;
//     import_DFA(loadedV , loadedX , loadedF , "huh.dat");
    
//     // Displaying imported DFA
//     cout << "Loaded table:" << endl;
//     for (const auto& m : loadedV) {
//         for (const auto& entry : m) {
//             cout << entry.first << ": " << entry.second << " ";
//         }
//         cout << endl;
//     }
//     cout << "Loaded start: " << loadedX << endl;
//     cout << "Loaded final states:" << endl;
//     for (const auto& entry : loadedF) {
//         cout << entry.first << ": " << entry.second << endl;
//     }

//     return 0;
// }