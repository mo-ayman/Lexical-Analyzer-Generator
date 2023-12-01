#ifndef SRC_NODE_H
#define SRC_NODE_H

#include <vector>

enum Operator {
    CONCAT,
    STAR,
    PLUS,
    OR,
    QUESTION,
    LEAF_NODE,
    NONE
};

class Node {
    char terminal;
    Operator op;
    std::vector<Node *> children;

public:
    Node();

    explicit Node(Operator op);

    Node(Operator op, std::vector<Node *> children);

    explicit Node(char terminal);

    ~Node();

    Node(const Node& other);

    [[nodiscard]] Operator getOp() const;

    void setOp(Operator op);

    [[nodiscard]] char getTerminal() const;

    void setTerminal(char terminal);

    [[nodiscard]] std::vector<Node *> getChildren() const;

    void setChildren(const std::vector<Node *>& children);

    void add_child(Node* child);

    void add_children(std::vector<Node *> children);

    void wrapLast(Operator op);

    void replaceLastN(int n, Node* node);

    void print() const;
};

#endif //SRC_NODE_H
