//
// Created by rusev on 11/26/23.
//

#include "NFAConstruction.h"

#include <utility>

NFAConstruction::NFAConstruction() {
    stateCount = 0;
    nfs = std::vector< std::map<char, std::vector<int>> >();
    finalStates = std::map< int, std::string >();
}

// CONCAT, STAR, PLUS, OR, QUESTION, LEAF_NODE, NONE, EPSILON

// ------------------ public methods ------------------

std::map<int, std::string> NFAConstruction::getFinalStates() const {
    return finalStates;
}

int NFAConstruction::getStartStateIndex() const {
    return startStateIndex;
}

std::vector<std::map<char, std::vector<int>>> NFAConstruction::getNfs() const {
    return nfs;
}

// ------------------ private methods ------------------

void NFAConstruction::constructNFA(std::vector<RuleTree *> rules) {
    startStateIndex = addState();
    for (auto rule : rules) {
        std::pair< char, std::array<int,2> > curr = performOperation(rule->getRoot());
//        char transChar = curr.first;
        int firstStateIndex = curr.second[0];
        int lastStateIndex = curr.second[1];

        nfs[startStateIndex][EPS].push_back(firstStateIndex);
        finalStates[lastStateIndex] = rule->getName();
    }

}

std::pair< char, std::array<int,2> > NFAConstruction::leafOp(Node* node) {
    int stateIndex = addState();
    return {node->getTerminal(), {stateIndex, stateIndex}};
}

std::pair< char, std::array<int,2> > NFAConstruction::concatOp(std::vector<Node *> nodes) {
    // first state of the first node
    auto firstState = performOperation(nodes[0]);
    char firstTransChar = firstState.first;
    // first state and last state of the chain
    int firstStateIndex = firstState.second[0];
    int lastStateIndex = firstState.second[1];

    for (int nodeIndex = 1; nodeIndex < nodes.size(); nodeIndex++) {
        // perform operation on the next node
        auto nextState = performOperation(nodes[nodeIndex]);
        char nextTransChar = nextState.first;
        int nextFirstStateIndex = nextState.second[0];
        int nextLastStateIndex = nextState.second[1];

        nfs[lastStateIndex][nextTransChar].push_back(nextFirstStateIndex);
        // update the last state of the chain
        lastStateIndex = nextLastStateIndex;

    }
    // return the first and last state of the chain
    return {firstTransChar, {firstStateIndex, lastStateIndex}};
}

std::pair< char, std::array<int,2> > NFAConstruction::starOp(std::vector<Node *> nodes) {
    std::pair< char, std::array<int,2> > curr = plusOp(std::move(nodes));
    char transChar = curr.first;
    int firstStateIndex = curr.second[0];
    int lastStateIndex = curr.second[1];
    // epsilon transition from first state to last state (different from plus)
    nfs[firstStateIndex][EPS].push_back(lastStateIndex);

    return {transChar, {firstStateIndex, lastStateIndex}};
}

std::pair< char, std::array<int,2> > NFAConstruction::plusOp(std::vector<Node *> nodes) {
    int firstStateIndex = addState(); // epsilon transition from it
    int lastStateIndex = addState(); // epsilon transition to it
    if (nodes.size() != 1) {
        // Handle the error as needed
        throw std::runtime_error("Plus or Star operator must have only one child");
    }
    // middle states
    std::pair< char, std::array<int,2> > curr = performOperation(nodes[0]);
    char transChar = curr.first;
    int firstCurrState = curr.second[0];
    int lastCurrState = curr.second[1];

    // backward epsilon transition from last state to first state
    nfs[lastCurrState][EPS].push_back(firstCurrState);

    // epsilon transition from first state to first state of middle states
    nfs[firstStateIndex][transChar].push_back(firstCurrState);
    // epsilon transition from last state of middle states to last state
    nfs[lastCurrState][EPS].push_back(lastStateIndex);

    return {EPS, {firstStateIndex, lastStateIndex}};
}

std::pair< char, std::array<int,2> > NFAConstruction::orOp(std::vector<Node *> nodes) {
    int firstStateIndex = addState(); // epsilon transition from it
    int lastStateIndex = addState(); // epsilon transition

    for(auto node : nodes) {
        std::pair< char, std::array<int,2> > curr = performOperation(node);
        char transChar = curr.first;
        int firstCurrState = curr.second[0];
        int lastCurrState = curr.second[1];

        nfs[firstStateIndex][transChar].push_back(firstCurrState);
        nfs[lastCurrState][EPS].push_back(lastStateIndex);
    }

    return {EPS, {firstStateIndex, lastStateIndex}};
}

std::pair< char, std::array<int,2> > NFAConstruction::questionOp(std::vector<Node *> nodes) {
    int firstStateIndex = addState(); // epsilon transition from it
    int lastStateIndex = addState(); // epsilon transition to it

    if (nodes.size() != 1) {
        // Handle the error as needed
        throw std::runtime_error("Question operator must have only one child");
    }
    // middle states
    std::pair< char, std::array<int,2> > curr = performOperation(nodes[0]);
    char transChar = curr.first;
    int firstCurrState = curr.second[0];
    int lastCurrState = curr.second[1];

    // char transition from first state to first state of middle states
    nfs[firstStateIndex][transChar].push_back(firstCurrState);

    // epsilon transition from first state to last state
    nfs[firstStateIndex][EPS].push_back(lastStateIndex);

    // epsilon transition from last state of middle states to last state
    nfs[lastCurrState][EPS].push_back(lastStateIndex);

    return {EPS, {firstStateIndex, lastStateIndex}};
}

std::pair< char, std::array<int,2> > NFAConstruction::noneOp(std::vector<Node *> nodes) {
    throw std::runtime_error("None operator");
}

std::pair< char, std::array<int,2> > NFAConstruction::performOperation(Node *node) {
    switch (node->getOp()) {
        case CONCAT:
            return concatOp(node->getChildren());
        case STAR:
            return starOp(node->getChildren());
        case PLUS:
            return plusOp(node->getChildren());
        case OR:
            return orOp(node->getChildren());
        case QUESTION:
            return questionOp(node->getChildren());
        case LEAF_NODE:
            return leafOp(node);
        case NONE:
            return noneOp(node->getChildren());
        case EPSILON:
            return epsilonOp(node);
        default:
            return {EPS, {0, 0}};
    }
}

int NFAConstruction::addState() {
    nfs.emplace_back();
    stateCount++;
    return stateCount - 1;
}

void NFAConstruction::print() {
    std::cout << "NFA Construction" << std::endl;
    std::cout << "Start state: " << startStateIndex << std::endl;
    std::cout << "Final states: ";
    for (auto finalState : finalStates) {
        std::cout << finalState.first << " ";
    }
    std::cout << std::endl;
    std::cout << "States: " << std::endl;
    for (int i = 0; i < nfs.size(); i++) {
        std::cout << i << ": ";
        for (auto transition : nfs[i]) {
            if (transition.first == EPS) {
                std::cout << "EPSILON -> ";
            } else {
                std::cout << transition.first << " -> ";
            }
            for (auto state : transition.second) {
                std::cout << state << " ";
            }
            std::cout << ", ";
        }
        std::cout << std::endl;
    }

}

std::pair<char, std::array<int, 2> > NFAConstruction::epsilonOp(Node *node) {
    int stateIndex = addState();
    return {EPS, {stateIndex, stateIndex}};
}







