#include <iostream>

#include "LexicalRules/RuleParser.h"
#include "LexicalRules/RuleTree.h"

int main() {
    // get dir path of the source code and concatenate with the file name
    std::string example_path = __FILE__;
    example_path = example_path.substr(0, example_path.find_last_of("\\/"));
    example_path += "/example.txt";
    const std::vector<RuleTree *> rules = RuleParser::parseFromFile(example_path);

    // print all rules
    for (const auto& rule: rules) {
        rule->print();
    }

    std::cout << std::endl;
    return 0;
}
