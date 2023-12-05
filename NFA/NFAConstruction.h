#ifndef LEXICAL_ANALYZER_GENERATOR_NFACONSTRUCTION_H
#define LEXICAL_ANALYZER_GENERATOR_NFACONSTRUCTION_H

#include <vector>
#include <map>
#include <unordered_map>

#include "../LexicalRules/RuleTree.h"

class NFAConstruction {
public:
    NFAConstruction();

    void constructNFA(const std::vector<RuleTree *>& rules);

    [[nodiscard]] std::vector<std::map<char, std::vector<int>>> getNfs() const;

    [[nodiscard]] std::unordered_map<int, std::tuple<std::string, Priority, int>> getFinalStates() const;

    [[nodiscard]] int getStartStateIndex() const;

    void print() const;

private:
    int stateCount;
    int startStateIndex;
    const char EPS = '\0';
    std::vector<std::map<char, std::vector<int>>> nfs;
    std::unordered_map<int, std::tuple<std::string, Priority, int>> finalStates;

    std::pair<char, std::array<int, 2>> performOperation(Node* node);

    // under char return first and last state index of the branch
    std::pair<char, std::array<int, 2>> concatOp(const std::vector<Node *>& nodes);

    std::pair<char, std::array<int, 2>> starOp(const std::vector<Node *>& nodes);

    std::pair<char, std::array<int, 2>> plusOp(const std::vector<Node *>& nodes);

    std::pair<char, std::array<int, 2>> orOp(const std::vector<Node *>& nodes);

    std::pair<char, std::array<int, 2>> questionOp(const std::vector<Node *>& nodes);

    std::pair<char, std::array<int, 2>> leafOp(const Node* nodes);

    static std::pair<char, std::array<int, 2>> noneOp(const std::vector<Node *>& nodes);

    std::pair<char, std::array<int, 2>> epsilonOp(Node* node);

    int addState();
};

#endif //LEXICAL_ANALYZER_GENERATOR_NFACONSTRUCTION_H
