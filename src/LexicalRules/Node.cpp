//
// Created by rusev on 11/26/23.
//

#include "Node.h"

Node::Node() {
    this->terminal = '\0';
    this->op = NONE;
    this->children = std::vector<Node*>();
}

Node::Node(Operator op, std::vector<Node *> children) {
    this->terminal = '\0';
    this->op = op;
    this->children = children;
}

Node::Node(Operator op) {
    this->terminal = '\0';
    this->op = op;
    this->children = std::vector<Node*>();

}

Node::Node(char terminal, Operator op) {
    this->terminal = terminal;
    this->op = op;
    this->children = std::vector<Node*>();
}

Node::Node(char terminal, Operator op, std::vector<Node *> children) {
    this->terminal = terminal;
    this->op = op;
    this->children = children;
}

Node::~Node() {
    for (auto child : children) {
        delete child;
    }

    children.clear();
}

Node::Node(const Node &other) {
    this->terminal = other.terminal;
    this->op = other.op;
    this->children = other.children;

}

void Node::add_child(Node *child) {
    children.push_back(child);
}

void Node::add_children(std::vector<Node *> children) {
    this->children.insert(this->children.end(), children.begin(), children.end());
}

void Node::print() {
    if (op == LEAF_NODE) {
        std::cout << terminal;
        return;
    }
    for (auto child : children) {
        child->print();
    }
}

char Node::getTerminal() const {
    return terminal;
}

void Node::setTerminal(char terminal) {
    Node::terminal = terminal;
}

Operator Node::getOp() const {
    return op;
}

void Node::setOp(Operator op) {
    Node::op = op;
}

std::vector<Node *> Node::getChildren() const {
    return children;
}

void Node::setChildren(const std::vector<Node *> &children) {
    Node::children = children;
}

