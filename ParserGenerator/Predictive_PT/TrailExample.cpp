#include <iostream>
#include <map>
#include <vector>
#include <memory> // Include for smart pointers

std::map<std::string*, std::vector<std::string*>>* createLocalStructure() {
    auto table =new  std::map<std::string*, std::vector<std::string*>>();

    // Create some local elements for insertion
    std::string* key1 = new std::string("Key1");
    std::string* key2 = new std::string("Key2");
    std::string* value1 = new std::string("Value1");
    std::string* value2 = new std::string("Value2");

    // Access the local map and insert elements
    (*table)[key1].push_back(value1);
    (*table)[key1].push_back(value2);
    (*table)[key2].push_back(value2);

    return table; // Return the unique pointer to the created structure
}

int main() {
    auto tablePtr = createLocalStructure(); // Receive the returned unique pointer

    // Accessing and printing elements
    for (auto& pair : *tablePtr) {
        std::cout << "Key: " << *(pair.first) << " Values: ";
        for (auto& value : pair.second) {
            std::cout << *(value) << " ";
        }
        std::cout << std::endl;
    }

    // Clean up memory: delete elements (keys and values)
    for (auto& pair : *tablePtr) {
        delete pair.first;
        for (auto& value : pair.second) {
            delete value;
        }
    }

    return 0;
}
