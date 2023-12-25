#include <sstream>
#include <iomanip>

#include "../ParsingCFG/ParsingCFG.h"
#include "../Parser/Parser.h"

#include "../Predictive_PT/PPT.h"

#include "../FirstAndFollow/First.h"
#include "../FirstAndFollow/Follow.h"
#include "../ParsingCFG/AdaptCFG.h"

using namespace std;

static void printTable(const std::unordered_map<Definition*, std::unordered_map<std::string, std::vector<Definition*>>>& table) {
    // Print header
    std::cout << std::setw(20) << "Non-terminal" << std::setw(20) << "Productions" << std::endl;
    std::cout << std::setw(70) << std::setfill('-') << "" << std::setfill(' ') << std::endl;

    // Print table content
    for (const auto& entry : table) {
        const auto nonTerminal = entry.first;
        std::cout << std::setw(20) << nonTerminal->getName() << (nonTerminal->getIsTerminal() ? " T" : " NT") << std::endl;
        for (const auto& innerEntry : entry.second) {
            std::cout << std::setw(40) << innerEntry.first << " -> ";
            for (const auto& item : innerEntry.second) {
                std::cout << item->getName() << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::setw(70) << std::setfill('-') << "" << std::setfill(' ') << std::endl;
    }
}

static std::string serializeTable(unordered_map<Definition *, unordered_map<string, vector<Definition *>>>& table, const Definition* start) {
    std:: string result;
    result += start->getName() + "#" + "0";
    for (auto& entry: table) {
        result += "\n";
        auto d = entry.first;
        result += d->getName() + "#";
        result += d->getIsTerminal()? "1" : "0";
        for (auto& [fst, snd]: entry.second) {
            result += "\x1D";
            result += fst; // string
            for (const auto& item: snd) {
                result += '\x1E';
                result += item->getName() + "#";
                result += item->getIsTerminal()? "1" : "0";
            }
        }
    }
    return result;
}

int main(const int argc, char** argv) {
    // Get dir path of the source code and concatenate with the file name
    std::string src_path = __FILE__;
    src_path = src_path.substr(0, src_path.find_last_of("\\/"));
    std::cout << src_path << std::endl;

    const auto rules_path = src_path + "\\example_rules.txt"; // std::string(argv[1]);
    std::cout<<src_path<<endl;

    Definition* start;
    map<Definition *, std::vector<std::vector<Definition *>>> rules = ParsingCFG::parseFromFile(rules_path, &start);
    // Print all rules
    std::cout << "====================== Rules ======================" << std::endl;
    for (const auto& rule: rules) {
        std::cout << rule.first->getName() << " -> ";
        for (const auto& alternative: rule.second) {
            for (const auto& definition: alternative) {
                if (definition->getIsTerminal()) {
                    std::cout << "'" << definition->getName() << "' ";
                } else {
                    std::cout << definition->getName() << " ";
                }
            }
            if (alternative != rule.second.back()) {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "====================== Adapted Rules ======================" << std::endl;
    auto adaptCFG = new AdaptCFG(rules);
    adaptCFG->adaptCFG();
    map<Definition *, std::vector<std::vector<Definition *>>> newRules = adaptCFG->getNewRules();

    ParsingCFG::print(newRules);
    rules = newRules;

    std::cout << "================ Obtaining First & Follow ================" << std::endl;

    auto* first = new First(rules);
    first->constructFirst();
    first->print();

    auto* follow = new Follow(rules, first, start);
    follow->constructFollow();
    follow->print();

    /*
     * PPTable(Greatly part)
     * */
    map<Definition*, vector<pair<int, Definition*>>> first1 = first->getFirst();
    map<Definition*, vector<Definition*>> follow2=follow->getFollow();//= new map<Definition*, vector<Definition*>>();
    auto* obj=new PPT(rules,first1,follow2);
    // Parsing table
    auto table = *(obj->computePPT());
    std::cout << "====================== PPTable ======================" << std::endl;
    printTable(table);

    // Save the parsing table
    std::string serializedTable = serializeTable(table, start);
    std::string filepath = src_path + "/parsingTable.txt";   //TODO: output

    // Open the file for writing
    std::ofstream outputFile(filepath);

    // Check if the file is open
    if (!outputFile.is_open()) {
        std::cerr << "Error opening the file for writing: " << filepath << std::endl;
        return 1;
    }
    outputFile << serializedTable;
    outputFile.close();

    //TODO: Collect Heap Garbage (if any)
    return 0;
}
