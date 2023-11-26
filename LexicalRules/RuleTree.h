
//
// Created by rusev on 11/26/23.
//
#ifndef SRC_RULETREE_H
#define SRC_RULETREE_H

#include "Node.h"

class RuleTree {

private:
    std::string name;
    Node *root;

public:
    // Constructor
    RuleTree();

    RuleTree(std::string name, Node *root);
    // Destructor
    ~RuleTree();

    // Copy Constructor
    RuleTree(const RuleTree &other);

    // Getters and Setters
    const std::string &getName() const;

    void setName(const std::string &name);

    Node *getRoot() const;

    void setRoot(Node *root);

    void print();
};


#endif //SRC_RULETREE_H
