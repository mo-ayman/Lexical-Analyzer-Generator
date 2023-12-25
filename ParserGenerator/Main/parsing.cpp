#include <sstream>
#include <string>
#include "../Parser/Parser.h"
#include <iomanip>

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

static void deserializeTable(const std::string& input, std::unordered_map<Definition*, std::unordered_map<std::string, std::vector<Definition*>>>& table, Definition** start) {
    std::istringstream stream(input);
    std::string token;
    unordered_map<string, Definition*> defPool;
    bool obtainedStart = false;
    while (std::getline(stream, token, '\n')) {
        if (token.empty()) {
            continue;
        }

        Definition* definition;
        int i = 0;
        string name;
        while(token[i]!='#') {
            name += token[i];
            i++;
        }
        i++;
        const bool isTerminal = token[i++] == '1';
        if (name == Definition::EPSILON) {
            definition = Definition::getEpsilon();
        }
        else if(name == Definition::DOLLAR) {
            definition = Definition::getDollar();
        }
        else {
            definition = (defPool.find(name) == defPool.end())? new Definition(name, isTerminal) : defPool[name];
            defPool[name] = definition;
        }
        table[definition] = unordered_map<string, vector<Definition*>>();
        if(!obtainedStart) {
            obtainedStart = true;
            *start = definition;
            continue;
        }
        while (token[i] == '\x1D') {
            string innerString;
            i++;
            while ( i<token.size() && token[i]!='\x1D' && token[i]!='\x1E') {
                innerString += token[i];
                i++;
            }

            table[definition][innerString] = std::vector<Definition*>();
            while(i<token.size() && token[i]=='\x1E'){
                Definition* innerDefinition;
                string innerName;
                i++;
                while(i<token.size()-1 && token[i]!='#') {
                    innerName += token[i];
                    i++;
                }
                i++;
                const bool innerIsTerminal = token[i++] == '1';
                if (innerName == Definition::EPSILON) {
                    innerDefinition = Definition::getEpsilon();
                }
                else if(innerName == Definition::DOLLAR) {
                    innerDefinition = Definition::getDollar();
                }
                else {
                    innerDefinition = (defPool.find(innerName) == defPool.end())? new Definition(innerName, innerIsTerminal) : defPool[innerName];
                }
                defPool[innerName] = innerDefinition;
                table[definition][innerString].push_back(innerDefinition);
            }
        }
    }
}

int main(const int argc, char** argv) {
    // Get dir path of the source code
    std::string src_path = __FILE__;
    src_path = src_path.substr(0, src_path.find_last_of("\\/"));
    std::cout << src_path << std::endl;

    // Output Files
    const std::string LeftMostDerivationFile = src_path + "\\LeftMostDerivation.txt";   //TODO output
    const std::string ParseTreeGraphFile = src_path + "\\ParseTreeGraph";               //TODO output

    // Lexical Analyzer Initialization
    const std::string lexical_analyzer_dfa = src_path + "\\lexical_analyzer.dat";   // serialized DFA file path //TODO input
    const std::string input_program = src_path + "\\input_program.txt";             // initial input file path  //TODO input
    auto lex = LexicalAnalyzer(input_program, 1024,lexical_analyzer_dfa);

    // Load parsing table
    std::string filepath = src_path+"/ParsingTable.txt";     //TODO input
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error opening the file: " << filepath << std::endl;
        return 1;
    }
    std::string serializedTable((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    cout<< serializedTable;

    file.close();

    std::unordered_map<Definition*, std::unordered_map<std::string, std::vector<Definition*>>> table;
    Definition* start;
    deserializeTable(serializedTable, table, &start);
    printTable(table);

    // Parser Initialization
    auto parser = Parser(table);

    // Generating Outputs
    auto parseTreeRoot = *parser.parse(lex, start);
    parseTreeRoot.plotGraph(ParseTreeGraphFile);

    std::ofstream outputFile(LeftMostDerivationFile);
    outputFile << parseTreeRoot;
    outputFile.close();

    //TODO: Collect Heap Garbage (definitions)
}