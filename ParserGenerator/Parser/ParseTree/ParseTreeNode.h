#ifndef TREENODE_H
#define TREENODE_H

#include <ostream>
#include <queue>
#include <vector>
#include <algorithm>
#include "../../Definition/Definition.h"
using namespace std;

class ParseTreeNode {
public:
    const Definition* content;
    vector<const ParseTreeNode*> children;

    bool getIsTerminal();
    explicit ParseTreeNode(const Definition* definition);
    ~ParseTreeNode() {
        for (auto child : children) {
            delete child;
        }
    }
    void insertLeft(const ParseTreeNode* child);
    void insertRight(const ParseTreeNode* child);
    void plotGraph(const std::string& outputPath);
    void printLeftmostDerivationSteps(std::ostream& os, vector<const Definition*> ignoreList);
};

#endif
