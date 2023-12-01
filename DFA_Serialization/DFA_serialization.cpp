#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

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
void serialize(const map<int, string>& m, ostream& os) {
    size_t mapSize = m.size();
    os.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
    for (const auto& entry : m) {
        os.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));

        size_t strSize = entry.second.size();
        os.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
        os.write(entry.second.c_str(), strSize);    }
}

// Deserialization for map<int, string>
void deserialize(map<int, std::string>& m, istream& is) {
    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));

    for (size_t i = 0; i < size; ++i) {
        int key;
        is.read(reinterpret_cast<char*>(&key), sizeof(key));

        size_t strSize;
        is.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
        string value(strSize, '\0');
        is.read(&value[0], strSize);

        m[key] = value;
    }
}

// DFA Serialization
int export_DFA(vector<map<char, int>>& table, int& start, map<int, string>& final_states, string file_path){
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
int import_DFA(vector<map<char, int>>& loaded_table, int& loaded_start, map<int, string>& loaded_final_states, string file_path){
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

int main() {
    // Sample DFA {v,x,f}
    vector<map<char, int>> v = {
        {{'a', 1}, {'b', 2}},
        {{'c', 3}, {'d', 4}},
        {{'e', 5}, {'f', 6}}
    };
    int x = 42;
    map<int, string> f = {
        {1, "one"},
        {2, "two"},
        {3, "three"}
    };

    // Exporting
    export_DFA(v,x,f,"huh.dat");

    // Importing
    vector<map<char, int>> loadedV;
    int loadedX;
    map<int, string> loadedF;
    import_DFA(loadedV , loadedX , loadedF , "huh.dat");
    
    // Displaying imported DFA
    cout << "Loaded table:" << endl;
    for (const auto& m : loadedV) {
        for (const auto& entry : m) {
            cout << entry.first << ": " << entry.second << " ";
        }
        cout << endl;
    }
    cout << "Loaded start: " << loadedX << endl;
    cout << "Loaded final states:" << endl;
    for (const auto& entry : loadedF) {
        cout << entry.first << ": " << entry.second << endl;
    }

    return 0;
}