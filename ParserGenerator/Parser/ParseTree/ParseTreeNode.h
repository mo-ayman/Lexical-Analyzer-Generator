#ifndef TREENODE_H
#define TREENODE_H
#include <ostream>
#include <queue>
#include <vector>

#include "../Definition.h"

using namespace std;

class ParseTreeNode {
private:
    Definition* content;
    vector<ParseTreeNode> children;

public:
    explicit ParseTreeNode(Definition* content);

    // Overloading the << operator for output stream
    friend std::ostream& operator<<(std::ostream& os, const ParseTreeNode& obj);

};


#endif
