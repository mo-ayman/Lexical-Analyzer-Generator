#ifndef TREENODE_H
#define TREENODE_H

#include <vector>
#include <memory>

#include "../../Definition/Definition.h"
using namespace std;

class ParseTreeNode {
public:
    const Definition* content;
    vector<std::shared_ptr<const ParseTreeNode>> children;

    [[nodiscard]] bool getIsTerminal() const;

    explicit ParseTreeNode(const Definition* definition);

    void insertLeft(const std::shared_ptr<const ParseTreeNode>& child);

    void insertRight(const std::shared_ptr<const ParseTreeNode>& child);

    void plotGraph(const std::string& outputPath) const;

    void printLeftmostDerivationSteps(std::ostream& os, vector<const Definition *> ignoreList) const;

    // ~ParseTreeNode() {for (auto child : children){delete child;}}
};

#endif
