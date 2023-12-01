#ifndef SRC_RULETREE_H
#define SRC_RULETREE_H

#include <iostream>

#include "Node.h"

enum Priority {
    PUNCTUATION,
    RESERVED,
    NORMAL
};

class RuleTree {
    std::string name;
    Priority priority;
    Node* root;

public:
    // Constructor
    RuleTree();

    RuleTree(std::string name, Node* root, Priority priority = NORMAL);

    // Destructor
    ~RuleTree();

    // Copy Constructor
    RuleTree(const RuleTree& other);

    // Getters and Setters
    [[nodiscard]] const std::string& getName() const;

    void setName(const std::string& name);

    [[nodiscard]] Node* getRoot() const;

    void setRoot(Node* root);

    void print() const;
};

#endif //SRC_RULETREE_H
