#include "ParsingCFG/ParsingCFG.h"
#include "Parser/Parser.h"

#include "Predictive_PT/PPT.h"

#include "FirstAndFollow/First.h"
#include "FirstAndFollow/Follow.h"


using namespace std;


int main(const int argc, char** argv) {
    // Get dir path of the source code and concatenate with the file name
    std::string src_path = __FILE__;
    src_path = src_path.substr(0, src_path.find_last_of("\\/"));
    std::cout << src_path << std::endl;
    const auto rules_path = src_path + "\\example_rules4.txt"; // std::string(argv[1]);
    std::cout<<src_path<<endl;
    const std::map<Definition *, std::vector<std::vector<Definition *>>> rules = ParsingCFG::parseFromFile(rules_path);
//    // Print all rules
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




    auto* first = new First(rules);
    first->constructFirst();
    first->print();

    auto* follow = new Follow(rules, first);
    follow->constructFollow();
    follow->print();

    /*
     * PPTable(Greatly part)
     * */
    std::cout << "====================== PPTable ======================" << std::endl;
    map<Definition*, vector<pair<int, Definition*>>> first1 = first->getFirst();
    map<Definition*, vector<Definition*>> follow2=follow->getFollow();//= new map<Definition*, vector<Definition*>>();
    auto* obj=new PPT(rules,first1,follow2);
    obj->print(obj->get_PPT());



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
