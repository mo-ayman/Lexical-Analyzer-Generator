#include <iostream>
#include "LexicalRules/RuleTree.h"

int main() {

    std::vector<Node *> children;
    children.push_back(new Node('a', LEAF_NODE));

    std::vector<Node *> children2;
    children2.push_back(new Node('b', LEAF_NODE));
    children2.push_back(new Node('c', LEAF_NODE));

    children.push_back(new Node(STAR, children2));

    RuleTree *ruleTree = new RuleTree("id", new Node(CONCAT, children));

    ruleTree->print();

    std::cout << std::endl;
    return 0;
}
