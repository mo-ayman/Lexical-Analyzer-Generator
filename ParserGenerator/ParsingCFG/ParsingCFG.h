#ifndef PARSER_GENERATOR_PARSINGCFG_H
#define PARSER_GENERATOR_PARSINGCFG_H

#include <string>
#include <map>
#include <vector>

#include "../Definition/Definition.h"

namespace ParsingCFG {
    std::map<Definition *, std::vector<std::vector<Definition *>>> parse(const std::string& input, Definition** start);

    void print(const std::map<Definition *, std::vector<std::vector<Definition *>>>& rules);

    std::map<Definition *, std::vector<std::vector<Definition *>>> parseFromFile(
        const std::string& filename, Definition** start);
}

#endif //PARSER_GENERATOR_PARSINGCFG_H
