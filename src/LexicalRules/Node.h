//
// Created by rusev on 11/26/23.
//

#ifndef LEXICAL_ANALYZER_GENERATOR_NODE_H
#define LEXICAL_ANALYZER_GENERATOR_NODE_H



#include <vector>
#include <string>
#include <iostream>

enum Operator {
    CONCAT, STAR, PLUS, OR, QUESTION, LEAF_NODE, NONE
};

class Node {
private:
    char terminal;
    Operator op;
    std::vector<Node *> children;
public:
    Node();

    Node(Operator op);

    Node(char terminal, Operator op);

    Node(Operator op, std::vector<Node *> children);

    Node(char terminal, Operator op, std::vector<Node *> children);

    ~Node();

    Node(const Node &other);

    char getTerminal() const;

    void setTerminal(char terminal);

    Operator getOp() const;

    void setOp(Operator op);

    std::vector<Node *> getChildren() const;

    void setChildren(const std::vector<Node *> &children);

    void add_child(Node *child);

    void add_children(std::vector<Node *> children);

    void print();

};

#endif //LEXICAL_ANALYZER_GENERATOR_NODE_H
