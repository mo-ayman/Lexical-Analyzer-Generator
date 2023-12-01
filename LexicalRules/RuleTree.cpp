#include "RuleTree.h"

RuleTree::RuleTree(std::string name, Node* root, const Priority priority) {
    this->name = std::move(name);
    this->priority = priority;
    this->root = root;
}

RuleTree::~RuleTree() {
    delete root;
}

RuleTree::RuleTree(const RuleTree& other) {
    this->name = other.name;
    this->priority = other.priority;
    this->root = other.root;
}

const std::string& RuleTree::getName() const {
    return name;
}

void RuleTree::setName(const std::string& name) {
    RuleTree::name = name;
}

Node* RuleTree::getRoot() const {
    return root;
}

void RuleTree::setRoot(Node* root) {
    RuleTree::root = root;
}

RuleTree::RuleTree() {
    this->name = "";
    this->priority = NORMAL;
    this->root = nullptr;
}

void RuleTree::print() const {
    std::cout << name << " -> ";
    root->print();
    std::cout << std::endl;
}
