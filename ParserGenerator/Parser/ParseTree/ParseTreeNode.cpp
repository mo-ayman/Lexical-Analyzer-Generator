#include "ParseTreeNode.h"

using namespace std;

ParseTreeNode::ParseTreeNode(Definition* definition):content(definition), children() {}

// Overloading the << operator for output stream
ParseTreeNode::std::ostream& operator<<(std::ostream& os, const ParseTreeNode& obj){
    // Use a queue for breadth-first traversal
    queue<const ParseTreeNode*> q;
    q.push(&obj);
    while (!q.empty()) {
        // Get the front node
        const ParseTreeNode* current = q.front();
        q.pop();

        // Print the content of the current node
        os << (current->content)->getName() << " ";

        // Enqueue the children of the current node
        for (const auto& child : current->Children) {
            q.push(&child);
        }

        // Check if we finished printing a level
        if (q.front() == nullptr) {
            q.pop();
            os << std::endl;
        }
    }
    return os;
}