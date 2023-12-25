#include "ParsingCFG/ParsingCFG.h"
#include "Parser/Parser.h"

#include "Predictive_PT/PPT.h"

#include "FirstAndFollow/First.h"
#include "FirstAndFollow/Follow.h"
#include "ParsingCFG/AdaptCFG.h"



int main(const int argc, char** argv) {
    // Get dir path of the source code and concatenate with the file name
    std::string src_path = __FILE__;
    src_path = src_path.substr(0, src_path.find_last_of("\\/"));
    std::cout << src_path << std::endl;

    const auto rules_path = src_path + "/example_rules.txt"; // std::string(argv[1]);
    std::cout<<src_path<<endl;

    Definition* start;
    map<Definition *, std::vector<std::vector<Definition *>>> rules = ParsingCFG::parseFromFile(rules_path, &start);
    // Print all rules
    std::cout << "====================== Rules ======================" << std::endl;
    ParsingCFG::print(rules);
    std::cout << std::endl;

    std::cout << "====================== Adapted Rules ======================" << std::endl;
    auto adaptCFG = new AdaptCFG(rules);
    adaptCFG->adaptCFG();
    const map<Definition *, std::vector<std::vector<Definition *>>> newRules = adaptCFG->getNewRules();

    ParsingCFG::print(newRules);


    auto* first = new First(newRules);
    first->constructFirst();
    first->print();

    auto* follow = new Follow(newRules, first, start);
    follow->constructFollow();
    follow->print();

    rules = newRules;

    /*
     * PPTable(Greatly part)
     * */
    std::cout << "====================== PPTable ======================" << std::endl;
    map<Definition*, vector<pair<int, Definition*>>> first1 = first->getFirst();
    map<Definition*, vector<Definition*>> follow2=follow->getFollow();//= new map<Definition*, vector<Definition*>>();
    auto* obj=new PPT(rules,first1,follow2);
    auto table = *(obj->computePPT());     // Parsing table
    obj->print(&table);



    /* Parsing */
    // Output Files
    const std::string LeftMostDerivationFile = src_path + "/LeftMostDerivation.txt";   //TODO output
    const std::string ParseTreeGraphFile = src_path + "/ParseTreeGraph";               //TODO output

    // Lexical Analyzer Initialization
    const std::string lexical_analyzer_dfa = src_path + "/lexical_analyzer.dat";   // serialized DFA file path //TODO input
    const std::string input_program = src_path + "/input_program.txt";             // initial input file path  //TODO input
    auto lex = LexicalAnalyzer(input_program, 1024,lexical_analyzer_dfa);

    // Parser Initialization
    auto parser = Parser(table);

    // Parsing & Generating Outputs
    auto parseTreeRoot = *parser.parse(lex, start);
    // Left Most Derivation Steps
    std::ofstream outputFile(LeftMostDerivationFile);
    outputFile << parseTreeRoot;
    outputFile.close();

    parseTreeRoot.plotGraph(ParseTreeGraphFile); // Parse Tree Graph
    /* End Of Parsing */

    //TODO: Collect Heap Garbage (if any)
    return 0;
}
