#ifndef TREENODE_H
#define TREENODE_H

#include <ostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <memory>

#include "../../Definition/Definition.h"
using namespace std;

class ParseTreeNode {
public:
    const Definition* content;
    vector<std::shared_ptr< const ParseTreeNode>> children;

    bool getIsTerminal();
    explicit ParseTreeNode(const Definition* definition);
    void insertLeft(std::shared_ptr<const ParseTreeNode> child);
    void insertRight(std::shared_ptr<const ParseTreeNode> child);
    void plotGraph(const std::string& outputPath) const;
    void printLeftmostDerivationSteps(std::ostream& os, vector<const Definition*> ignoreList) const;

    // ~ParseTreeNode() {for (auto child : children){delete child;}}
};

#endif
