//
// Created by rusev on 11/26/23.
//

#ifndef LEXICAL_ANALYZER_GENERATOR_NFACONSTRUCTION_H
#define LEXICAL_ANALYZER_GENERATOR_NFACONSTRUCTION_H
#include <vector>
#include <map>
#include <unordered_map>
#include "../LexicalRules/RuleTree.h"

class NFAConstruction {
public:
    NFAConstruction();
    void constructNFA(std::vector<RuleTree*> rules);
    [[nodiscard]] std::vector< std::map<char, std::vector<int>> > getNfs() const;
    [[nodiscard]] std::unordered_map<int, std::tuple<std::string, Priority, int>> getFinalStates() const;
    [[nodiscard]] int getStartStateIndex() const;
    void print();

private:
    int stateCount;
    std::vector< std::map<char, std::vector<int>> > nfs;
    std::unordered_map<int, std::tuple<std::string, Priority, int>> finalStates;
    const char EPS = '\0';
    int startStateIndex;
    std::pair< char, std::array<int,2> > performOperation(Node *node);

    // under char return first and last state index of the branch
    std::pair< char, std::array<int,2> > concatOp(std::vector<Node*> nodes);

    std::pair< char, std::array<int,2> > starOp(std::vector<Node*> nodes);

    std::pair< char, std::array<int,2> > plusOp(std::vector<Node*> nodes);

    std::pair< char, std::array<int,2> > orOp(std::vector<Node*> nodes);

    std::pair< char, std::array<int,2> > questionOp(std::vector<Node*> nodes);

    std::pair< char, std::array<int,2> > leafOp(Node* nodes);

    std::pair< char, std::array<int,2> > noneOp(std::vector<Node*> nodes);

    std::pair< char, std::array<int,2> > epsilonOp(Node* node);

    int addState();
};


#endif //LEXICAL_ANALYZER_GENERATOR_NFACONSTRUCTION_H
