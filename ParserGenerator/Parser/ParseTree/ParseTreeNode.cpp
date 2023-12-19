#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <queue>
#include "ParseTreeNode.h"
#include "ostream"

using namespace std;

ParseTreeNode::ParseTreeNode(const Definition* definition): content(definition),  children() {
}

bool ParseTreeNode::getIsTerminal(){
    return content->getIsTerminal();
}


void ParseTreeNode::insertLeft(std::shared_ptr<const ParseTreeNode> child) {
    children.insert(children.begin(), child);
}
void ParseTreeNode::insertRight(std::shared_ptr<const ParseTreeNode> child) {
    children.push_back(child);
}


void ParseTreeNode:: plotGraph(const std::string& outputPath) const{
    std::ofstream dotFile = std::ofstream(outputPath + ".dot");
    if (!dotFile.is_open()) {
        std::cerr << "Error: Unable to open DOT file for writing." << std::endl;
        return;
    }
    dotFile << "digraph ParseTree {" << std::endl;
    // BFS
    std::queue<std::shared_ptr<const ParseTreeNode>> q;
    q.push(std::make_shared<const ParseTreeNode>(*this));
    auto pRoot = q.front().get();

    while (!q.empty()) {
        const auto current = q.front();
        q.pop();
        // Plot the current node
        string color = (current.get() == pRoot)? "Tomato" : "lightblue";
        dotFile << "  " << reinterpret_cast<std::uintptr_t>(current.get())
                << " [style=filled, fillcolor="+color+", label=\"" + current->content->getName() + "\"";
        if (current->content->getIsTerminal()) {
            dotFile << ", shape=triangle, style=filled, fillcolor=gold";
        }
        dotFile << "]" << std::endl;
        // Specify edges to children
        for (const auto child : current->children) {
            dotFile << reinterpret_cast<std::uintptr_t>(current.get()) << " -> " << reinterpret_cast<std::uintptr_t>(child.get()) << std::endl;
            q.push(child);
        }
        dotFile << ";" << std::endl;
    }

    dotFile << "}" << std::endl;
    dotFile.close();
    std::string graphvizCommand = "dot -Tpdf " + outputPath + ".dot -o " + outputPath + ".pdf";
    int result = system(graphvizCommand.c_str());
    if (result != 0) {
        std::cerr << "Error: Graphviz command failed." << std::endl;
    }
    std::cout << "Plotted successfully" <<  std::endl;
}


void ParseTreeNode :: printLeftmostDerivationSteps(std::ostream& os, vector<const Definition*> ignoreList) const {
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
        auto removedNode = nodes[nonTerminalIdx];
        nodes.erase(nodes.begin() + nonTerminalIdx);
        for (int j = 0; j < removedNode->children.size(); j++) {
            if (std::find_if(ignoreList.begin(), ignoreList.end(),
                             [removedNode, j](const Definition* ptr) {
                                 return ptr == removedNode->children[j]->content;
                             }) == ignoreList.end()) {
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
