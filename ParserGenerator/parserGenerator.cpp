#include <sstream>
#include <iomanip>

#include "ParsingCFG/ParsingCFG.h"
#include "Parser/Parser.h"

#include "Predictive_PT/PPT.h"

#include "FirstAndFollow/First.h"
#include "FirstAndFollow/Follow.h"
#include "ParsingCFG/AdaptCFG.h"

using namespace std;

static void printTable(
    const std::unordered_map<Definition *, std::unordered_map<std::string, std::vector<Definition *>>>& table) {
    // Print header
    std::cout << std::setw(20) << "Non-terminal" << std::setw(20) << "Productions" << std::endl;
    std::cout << std::setw(70) << std::setfill('-') << "" << std::setfill(' ') << std::endl;

    // Print table content
    for (const auto& entry: table) {
        const auto nonTerminal = entry.first;
        std::cout << std::setw(20) << nonTerminal->getName() << (nonTerminal->getIsTerminal() ? " T" : " NT") <<
                std::endl;
        for (const auto& innerEntry: entry.second) {
            std::cout << std::setw(40) << innerEntry.first << " -> ";
            for (const auto& item: innerEntry.second) {
                std::cout << item->getName() << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::setw(70) << std::setfill('-') << "" << std::setfill(' ') << std::endl;
    }
}

static std::string serializeTable(unordered_map<Definition *, unordered_map<string, vector<Definition *>>>& table,
                                  const Definition* start) {
    std::string result;
    result += start->getName() + "#" + "0";
    for (auto& entry: table) {
        result += "\n";
        const auto d = entry.first;
        result += d->getName() + "#";
        result += d->getIsTerminal() ? "1" : "0";
        for (auto& [fst, snd]: entry.second) {
            result += "\x1D";
            result += fst; // string
            for (const auto& item: snd) {
                result += '\x1E';
                result += item->getName() + "#";
                result += item->getIsTerminal() ? "1" : "0";
            }
        }
    }
    return result;
}

int main(const int argc, char** argv) {
    const auto rules_path = std::string(argv[1]);
    std::cout << "====================== Rules Path ======================" << std::endl;
    std::cout << rules_path << std::endl;
    std::cout << std::endl;

    std::string table_path;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 < argc) {
                table_path = argv[i + 1];
                i++;
            } else {
                std::cerr << "-o option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " <input_path> -o <table_path>" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -o <table_path>  Specify generated table path." << std::endl;
            return 0;
        }
    }

    if (table_path.empty()) {
        std::cerr << "Error: Generated table path not specified." << std::endl;
        return 1;
    }

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
    map<Definition *, vector<pair<int, Definition *>>> first1 = first->getFirst();
    map<Definition *, vector<Definition *>> follow2 = follow->getFollow();
    auto* obj = new PPT(rules, first1, follow2);
    // Parsing table
    auto table = *(obj->computePPT());
    std::cout << "====================== PPTable ======================" << std::endl;
    printTable(table);

    // Save the parsing table
    std::string serializedTable = serializeTable(table, start);
    std::string filepath = table_path; // TODO: output

    // Open the file for writing
    std::ofstream outputFile(filepath);

    // Check if the file is open
    if (!outputFile.is_open()) {
        std::cerr << "Error opening the file for writing: " << filepath << std::endl;
        return 1;
    }
    outputFile << serializedTable;
    outputFile.close();

    // TODO: Collect Heap Garbage (definitions, what else ...?)
    return 0;
}
