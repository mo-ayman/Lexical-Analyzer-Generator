#include "ParsingCFG/ParsingCFG.h"
#include "Parser/Parser.h"
#include "ParsingCFG/ParsingCFG.h"
#include "FirstAndFollow/First.h"
#include "FirstAndFollow/Follow.h"
#include "ParsingCFG/AdaptCFG.h"

using namespace std;


int main(const int argc, char** argv) {
    // Get dir path of the source code and concatenate with the file name
    std::string src_path = __FILE__;
    src_path = src_path.substr(0, src_path.find_last_of("\\/"));
    std::cout << src_path << std::endl;
    const auto rules_path = src_path + "/example_rules.txt"; // std::string(argv[1]);
    std::map<Definition *, std::vector<std::vector<Definition *>>> rules = ParsingCFG::parseFromFile(rules_path);
    // Print all rules
    std::cout << "====================== Rules ======================" << std::endl;

    ParsingCFG::print(rules);

    std::cout << "====================== Adapt ======================" << std::endl;

    auto* adaptCFG = new AdaptCFG(rules);
    adaptCFG->adaptCFG();
    auto adaptedRule = adaptCFG->getNewRules();

    std::cout << "====================== New Rules ======================" << std::endl;
    ParsingCFG::print(adaptedRule);


    // print size of rules
    std::cout << "rules.size(): " << rules.size() << std::endl;
    std::cout << "adaptedRule.size(): " << adaptedRule.size() << std::endl;

    auto* first = new First(adaptedRule);
    first->constructFirst();
    first->print();

    auto* follow = new Follow(adaptedRule, first);
    follow->constructFollow();
    follow->print();

     /* Parsing
      *TODO: import `table`, IF we were to perform the pipeline in a different main() -> makes more sense.
      *TODO: OTHERWISE, perform the pipeline & obtain `table` here.
      *auto table = ...;
      *auto lex = LexicalAnalyzer(...);
      *auto parser = Parser(table);
      *auto root = parser.parse(lex, &S, &epsilon,eof.getName());
      *root->printLeftmostDerivationSteps(std::cout, {&epsilon});
      *root->plotGraph("Sheet5_Q2_parseTree");
     */
    return 0;
}
