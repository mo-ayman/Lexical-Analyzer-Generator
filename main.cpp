#include <iostream>
#include "LexicalRules/RuleTree.h"
#include "NFA/NFAConstruction.h"

RuleTree* id();
RuleTree* num();
RuleTree* relop();
RuleTree* addop();
RuleTree* mulop();


int main() {

//
    std::vector<RuleTree*> rules;
//    rules.push_back(id());
    rules.push_back(num());
//    rules.push_back(relop());
//    rules.push_back(addop());
//    rules.push_back(mulop());

    NFAConstruction nfaConstruction;
    nfaConstruction.constructNFA(rules);
    nfaConstruction.print();
    std::cout << std::endl;
    return 0;
}

RuleTree* id() {
    std::vector<Node *> children;
    for(char c = 'a'; c <= 'z'; c++) {
        children.push_back(new Node(c, LEAF_NODE));
    }

    Node* or1 = new Node(OR, children);

    Node* concat = new Node(CONCAT);

    Node* or2 = new Node(OR);

    for(char c = 'a'; c <= 'z'; c++) {
        or2->add_child(new Node(c, LEAF_NODE));
    }
    for(char c = 'A'; c <= 'Z'; c++) {
        or2->add_child(new Node(c, LEAF_NODE));
    }

    for(char d = '0'; d <= '9'; d++) {
        or2->add_child(new Node(d, LEAF_NODE));
    }

    Node* star = new Node(STAR);
    star->add_child(or2);

    concat->add_child(or1);
    concat->add_child(star);

    auto* ruleTree = new RuleTree("id", concat);
//    ruleTree->print();
    return ruleTree;
}

RuleTree* num() {
    std::vector<Node *> children;
    for(char d = '0'; d <= '9'; d++) {
        children.push_back(new Node(d, LEAF_NODE));
    }

    Node *plus = new Node('+', PLUS);
    plus->add_child(new Node(OR, children));


    std::vector<Node *> children2;
    for(char d = '0'; d <= '9'; d++) {
        children2.push_back(new Node(d, LEAF_NODE));
    }
    Node *plus2 = new Node('+', PLUS);
    plus2->add_child(new Node(OR, children2));


    std::vector<Node *> children3;
    for(char d = '0'; d <= '9'; d++) {
        children3.push_back(new Node(d, LEAF_NODE));
    }
    Node* or1 = new Node(OR, children3);

    Node* dot = new Node('.', LEAF_NODE);

    Node* or3 = new Node(OR);
    or3->add_child(new Node(EPSILON));


    Node* concat2 = new Node(CONCAT);
    concat2->add_child(new Node('E', LEAF_NODE));
    Node* or4 = new Node(OR);
    for(char d = '0'; d <= '9'; d++) {
        or4->add_child(new Node(d, LEAF_NODE));
    }
    concat2->add_child(or4);
    or3->add_child(concat2);

    Node* concat = new Node(CONCAT);
    concat->add_child(plus2);
    concat->add_child(dot);
    concat->add_child(or1);
    concat->add_child(or3);



    Node* or2 = new Node(OR);
    or2->add_child(plus);
    or2->add_child(concat);

    auto* ruleTree = new RuleTree("num", or2);
//    ruleTree->print();
    return ruleTree;
}

RuleTree* relop() {

    std::vector<Node *> children;
    std::vector<Node *> children1;

    children1.push_back(new Node('=', LEAF_NODE));
    children1.push_back(new Node('=', LEAF_NODE));

    std::vector<Node *> children2;
    children2.push_back(new Node('!', LEAF_NODE));
    children2.push_back(new Node('=', LEAF_NODE));

    std::vector<Node *> children3;
    children3.push_back(new Node('<', LEAF_NODE));
    children3.push_back(new Node('=', LEAF_NODE));

    std::vector<Node *> children4;
    children4.push_back(new Node('>', LEAF_NODE));
    children4.push_back(new Node('=', LEAF_NODE));



    children.push_back(new Node(CONCAT, children1));
    children.push_back(new Node(CONCAT, children2));
    children.push_back(new Node(CONCAT, children3));
    children.push_back(new Node(CONCAT, children4));
    children.push_back(new Node('>', LEAF_NODE));
    children.push_back(new Node('<', LEAF_NODE));

    auto *ruleTree = new RuleTree("relop", new Node(OR, children));

//    ruleTree->print();
    return ruleTree;
}

RuleTree* addop() {
    std::vector<Node *> children;
    children.push_back(new Node('+', LEAF_NODE));
    children.push_back(new Node('-', LEAF_NODE));

    auto *ruleTree = new RuleTree("addop", new Node(OR, children));

//    ruleTree->print();
    return ruleTree;
}

RuleTree* mulop() {
    std::vector<Node *> children;
    children.push_back(new Node('*', LEAF_NODE));
    children.push_back(new Node('/', LEAF_NODE));

    auto *ruleTree = new RuleTree("mulop", new Node(OR, children));

//    ruleTree->print();
    return ruleTree;
}
