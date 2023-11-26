//
// Created by rusev on 11/26/23.
//

#include <iostream>
#include "RuleTree.h"

// ------------------ RuleTree ------------------
RuleTree::RuleTree(std::string name, Node *root) {
    this->name = name;
    this->root = root;
}

RuleTree::~RuleTree() {
    delete root;
}

RuleTree::RuleTree(const RuleTree &other) {
    this->name = other.name;
    this->root = other.root;
}

const std::string &RuleTree::getName() const {
    return name;
}

void RuleTree::setName(const std::string &name) {
    RuleTree::name = name;
}

Node *RuleTree::getRoot() const {
    return root;
}

void RuleTree::setRoot(Node *root) {
    RuleTree::root = root;
}

RuleTree::RuleTree() {
    this->name = "";
    this->root = nullptr;
}

void RuleTree::print() {
    std::cout << "RuleTree: " << name << std::endl;
    root->print();
}



