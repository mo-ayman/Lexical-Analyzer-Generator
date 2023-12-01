#include "Node.h"

#include <iostream>
#include <utility>

Node::Node() {
    this->terminal = '\0';
    this->op = NONE;
    this->children = std::vector<Node *>();
}

Node::Node(const Operator op) {
    this->terminal = '\0';
    this->op = op;
    this->children = std::vector<Node *>();
}

Node::Node(const Operator op, std::vector<Node *> children) {
    this->terminal = '\0';
    this->op = op;
    this->children = std::move(children);
}

Node::Node(const char terminal) {
    this->terminal = terminal;
    this->op = LEAF_NODE;
    this->children = std::vector<Node *>();
}

Node::~Node() {
    for (const auto child: children) {
        delete child;
    }

    children.clear();
}

Node::Node(const Node& other) {
    this->terminal = other.terminal;
    this->op = other.op;
    this->children = other.children;
}

char Node::getTerminal() const {
    return terminal;
}

void Node::setTerminal(const char terminal) {
    Node::terminal = terminal;
}

Operator Node::getOp() const {
    return op;
}

void Node::setOp(const Operator op) {
    Node::op = op;
}

std::vector<Node *> Node::getChildren() const {
    return children;
}

void Node::setChildren(const std::vector<Node *>& children) {
    Node::children = children;
}

void Node::add_child(Node* child) {
    children.push_back(child);
}

void Node::add_children(std::vector<Node *> children) {
    this->children.insert(this->children.end(), children.begin(), children.end());
}

void Node::print() const {
    if (op == OR) {
        std::cout << "(";
        for (const auto child: children) {
            child->print();
            if (child != children.back()) {
                std::cout << "|";
            }
        }
        std::cout << ")";
    } else if (op == CONCAT) {
        for (const auto child: children) {
            child->print();
        }
    } else if (op == STAR) {
        std::cout << "(";
        children[0]->print();
        std::cout << ")*";
    } else if (op == PLUS) {
        std::cout << "(";
        children[0]->print();
        std::cout << ")+";
    } else if (op == QUESTION) {
        std::cout << "(";
        children[0]->print();
        std::cout << ")?";
    } else if (op == LEAF_NODE) {
        if (terminal == '\0') {
            std::cout << "ε";
        } else {
            std::cout << terminal;
        }
    }
}

void Node::wrapLast(const Operator op) {
    const size_t lastIndex = children.size() - 1;
    Node* prevNode = children[lastIndex];
    children[lastIndex] = new Node(op, {prevNode});
}

void Node::replaceLastN(const int n, Node* node) {
    for (int i = 0; i < n; i++) {
        const Node* last = children.back();
        children.pop_back();
        delete last;
    }
    children.push_back(node);
}
