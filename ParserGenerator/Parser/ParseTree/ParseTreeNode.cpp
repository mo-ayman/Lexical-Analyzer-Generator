#include <iostream>
#include <fstream>
#include <ostream>
#include <algorithm>
#include <cstdint>
#include <queue>

#include "ParseTreeNode.h"

using namespace std;

ParseTreeNode::ParseTreeNode(Definition* definition): content(definition), children() {
}

bool ParseTreeNode::getIsTerminal() const {
    return content->getIsTerminal();
}

void ParseTreeNode::insertLeft(const std::shared_ptr<const ParseTreeNode>& child) {
    children.insert(children.begin(), child);
}

void ParseTreeNode::insertRight(const std::shared_ptr<const ParseTreeNode>& child) {
    children.push_back(child);
}

void ParseTreeNode::plotGraph(const std::string& outputPath) const {
    auto dotFile = std::ofstream(outputPath + ".dot");
    if (!dotFile.is_open()) {
        std::cerr << "Error: Unable to open DOT file for writing." << std::endl;
        return;
    }
    dotFile << "digraph ParseTree {" << std::endl;
    // BFS
    std::queue<std::shared_ptr<const ParseTreeNode>> q;
    q.push(std::make_shared<const ParseTreeNode>(*this));
    const auto pRoot = q.front().get();

    while (!q.empty()) {
        const auto current = q.front();
        q.pop();
        // Plot the current node
        string color = (current.get() == pRoot) ? "hotpink" : "lightblue";
        string name = current->content == Definition::getEpsilon() ? "ε" : current->content->getName();
        dotFile << "  " << reinterpret_cast<std::uintptr_t>(current.get())
                << " [style=filled, fillcolor=" + color + ", label=\"" << name << "\"";
        if (current->content->getIsTerminal()) {
            color = current->content == Definition::getEpsilon() ? "gold" : "yellow";
            dotFile << ", shape=triangle, style=filled, fillcolor=" + color;
        }
        dotFile << "]" << std::endl;
        // Specify edges to children
        for (const auto& child: current->children) {
            dotFile << reinterpret_cast<std::uintptr_t>(current.get()) << " -> " << reinterpret_cast<std::uintptr_t>(
                child.get()) << std::endl;
            q.push(child);
        }
        dotFile << ";" << std::endl;
    }

    dotFile << "}" << std::endl;
    dotFile.close();
    const std::string graphvizCommand = "dot -Tpdf " + outputPath + ".dot -o " + outputPath + ".pdf";
    const int result = system(graphvizCommand.c_str());
    if (result != 0) {
        std::cerr << "Error: Graphviz command failed." << std::endl;
    }
    std::cout << "Plotted successfully" << std::endl;
}


void ParseTreeNode::printLeftmostDerivationSteps(std::ostream& os) const {
    std::vector<std::shared_ptr<const ParseTreeNode>> nodes;
    nodes.push_back(std::make_shared<const ParseTreeNode>(*this));
    os << this->content->getName() << " " << std::endl;
    int nonTerminalIdx = 0;
    while (true) {
        int i = nonTerminalIdx;
        for (; i < nodes.size(); i++) {
            if (!nodes[i]->content->getIsTerminal()) {
                nonTerminalIdx = i;
                break;
            }
        }
        if (i >= nodes.size()) {
            break;
        }
        const auto removedNode = nodes[nonTerminalIdx];
        nodes.erase(nodes.begin() + nonTerminalIdx);
        for (int j = 0; j < removedNode->children.size(); j++) {
            if (removedNode->children[j]->content != Definition::getEpsilon()) {
                nodes.insert(nodes.begin() + nonTerminalIdx + j, removedNode->children[j]);
            }
        }
        for (int j = 0; j < nodes.size(); j++) {
            os << nodes[j]->content->getName();
            if (j < nodes.size() - 1) {
                os << " ";
            }
        }
        os << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const ParseTreeNode& node) {
    node.printLeftmostDerivationSteps(os);
    return os;
}
