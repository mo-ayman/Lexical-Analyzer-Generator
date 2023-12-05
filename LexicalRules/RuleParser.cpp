#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <unordered_map>

#include "RuleParser.h"

static std::string readRules(const std::string& filename) {
    std::ifstream stream(filename);
    if (stream.is_open()) {
        std::stringstream str;
        while (stream.peek() != EOF) {
            char nextChar = static_cast<char>(stream.get());
            str << nextChar;
        }
        stream.close();
        return str.str();
    }
    throw std::runtime_error("Could not open file");
}

static bool isValidRuleNameChar(const char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isWhitespace(const char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

static std::vector<std::string> split(const std::string& input) {
    const int inputLength = static_cast<int>(input.length());
    std::vector<std::string> lines;
    std::string currentLine;
    std::string pendingWhitespace;
    bool lineStarted = false;
    for (int i = 0; i < inputLength; i++) {
        if (lineStarted) {
            if (input[i] == '\n') {
                lines.push_back(currentLine);
                currentLine = "";
                pendingWhitespace = "";
                lineStarted = false;
            } else if (isWhitespace(input[i])) {
                pendingWhitespace += input[i];
            } else {
                currentLine += pendingWhitespace;
                currentLine += input[i];
                pendingWhitespace = "";
            }
        } else if (!isWhitespace(input[i])) {
            currentLine += input[i];
            lineStarted = true;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}

static Node* parseSet(const std::string& input, const int start, const int end) {
    std::set<char> chars;
    for (int i = start; i < end; i++) {
        if (i != end - 2 && input[i + 1] == '-') {
            const char first = input[i];
            const char last = input[i + 2];
            if (first > last) {
                throw std::runtime_error("Character range must be in ascending order");
            }
            for (char j = first; j <= last; j++) {
                chars.insert(j);
            }
            i += 2;
        } else {
            chars.insert(input[i]);
        }
    }
    if (chars.empty()) {
        throw std::runtime_error("Set cannot be empty");
    }
    auto* currentOrNode = new Node(OR);
    for (const auto& item: chars) {
        currentOrNode->add_child(new Node(item));
    }
    return currentOrNode;
}

static Node* parseRule(const std::string& input, const int start, const int end,
                const std::unordered_map<std::string, Node *>& definitions) {
    Node* currentOrNode = nullptr;
    Node* lastFound = nullptr;
    std::string lastSeq;
    bool lastConcatenated = false;
    for (int i = start; i < end; i++) {
        if (input[i] == '(' || input[i] == '[') {
            const char opening = input[i];
            const char closing = opening == '(' ? ')' : ']';
            int j = i + 1;
            int count = 1;
            while (count != 0) {
                if (input[j] == opening) {
                    count++;
                } else if (input[j] == closing) {
                    count--;
                }
                j++;
            }
            Node* nested = opening == '(' ? parseRule(input, i + 1, j - 1, definitions) : parseSet(input, i + 1, j - 1);
            if (lastFound != nullptr && lastFound->getOp() == CONCAT) {
                lastFound->add_child(nested);
                lastConcatenated = true;
            } else if (lastFound != nullptr) {
                lastFound = new Node(CONCAT, {lastFound, nested});
                lastConcatenated = true;
            } else {
                lastFound = nested;
            }
            lastSeq = "";
            i = j - 1;
        } else if (input[i] == '|') {
            if (lastFound == nullptr) {
                throw std::runtime_error("OR operator must be preceded by an operand");
            }
            if (currentOrNode == nullptr) {
                currentOrNode = new Node(OR, {lastFound});
            } else {
                currentOrNode->add_child(lastFound);
            }
            if (i == end - 1) {
                currentOrNode->add_child(new Node('\0'));
            }
            lastSeq = "";
            lastFound = nullptr;
        } else if (input[i] == '*') {
            if (lastFound != nullptr) {
                if (lastConcatenated) {
                    lastFound->wrapLast(STAR);
                    lastConcatenated = false;
                } else {
                    lastFound = new Node(STAR, {lastFound});
                }
                lastSeq = "";
            } else {
                throw std::runtime_error("STAR operator must be preceded by an operand");
            }
        } else if (input[i] == '+') {
            if (lastFound != nullptr) {
                if (lastConcatenated) {
                    lastFound->wrapLast(PLUS);
                    lastConcatenated = false;
                } else {
                    lastFound = new Node(PLUS, {lastFound});
                }
                lastSeq = "";
            } else {
                throw std::runtime_error("PLUS operator must be preceded by an operand");
            }
        } else if (input[i] == '?') {
            if (lastFound != nullptr) {
                if (lastConcatenated) {
                    lastFound->wrapLast(QUESTION);
                    lastConcatenated = false;
                } else {
                    lastFound = new Node(QUESTION, {lastFound});
                }
                lastSeq = "";
            } else {
                throw std::runtime_error("QUESTION operator must be preceded by an operand");
            }
        } else if (!isWhitespace(input[i])) {
            char nextChar = input[i];
            if (input[i] == '\\') {
                i++;
                if (i == end) {
                    throw std::runtime_error("No character after escape");
                }
                nextChar = input[i];
                lastSeq += nextChar;
                if (nextChar == 'L') {
                    nextChar = '\0';
                    lastSeq = "";
                }
            } else {
                lastSeq += nextChar;
            }
            if (lastFound != nullptr && lastFound->getOp() == CONCAT) {
                lastFound->add_child(new Node(nextChar));
                lastConcatenated = true;
            } else if (lastFound != nullptr) {
                lastFound = new Node(CONCAT, {lastFound, new Node(nextChar)});
                lastConcatenated = true;
            } else {
                lastFound = new Node(nextChar);
            }
            if ((i + 1 == end || !isValidRuleNameChar(input[i + 1])) && definitions.find(lastSeq) != definitions.end()) {
                const int lastSeqLength = static_cast<int>(lastSeq.length());
                Node* definitionCopy = definitions.at(lastSeq);
                if (lastFound->getOp() == CONCAT) {
                    if (lastFound->getChildren().size() == lastSeqLength) {
                        delete lastFound;
                        lastFound = definitionCopy;
                        lastConcatenated = false;
                    } else {
                        lastFound->replaceLastN(lastSeqLength, definitionCopy);
                    }
                } else {
                    lastFound = definitionCopy;
                }
                lastSeq = "";
            }
        } else {
            lastSeq = "";
        }
    }
    if (currentOrNode != nullptr) {
        if (lastFound != nullptr) {
            currentOrNode->add_child(lastFound);
        }
        return currentOrNode;
    }
    if (lastFound != nullptr) {
        return lastFound;
    }
    throw std::runtime_error("Rule cannot be empty");
}

static void parseDefExp(const std::string& input, std::unordered_map<std::string, Node *>& definitions,
                 std::vector<RuleTree *>& rules) {
    const int inputLength = static_cast<int>(input.length());
    std::string ruleName;
    int inputIterator = 0;
    bool pendingWhitespace = false;
    while (input[inputIterator] != ':' && input[inputIterator] != '=') {
        if (isWhitespace(input[inputIterator])) {
            pendingWhitespace = true;
        } else {
            if (pendingWhitespace || !isValidRuleNameChar(input[inputIterator])) {
                throw std::runtime_error("Rule name may only contain latin letters, digits and underscores");
            }
            ruleName += input[inputIterator];
        }
        if (inputIterator == inputLength - 1) {
            throw std::runtime_error("Rule cannot be empty");
        }
        inputIterator++;
    }
    if (ruleName.empty()) {
        throw std::runtime_error("Rule must have a valid name");
    }
    const bool isDefinition = input[inputIterator] == '=';
    const int lineLength = static_cast<int>(input.length());
    inputIterator++;
    if (isDefinition) {
        definitions[ruleName] = parseRule(input, inputIterator, lineLength, definitions);
    } else {
        rules.push_back(new RuleTree(ruleName, parseRule(input, inputIterator, lineLength, definitions)));
    }
}

static void parseReversed(const std::string& input, std::vector<RuleTree *>& rules) {
    const int inputLength = static_cast<int>(input.length());
    std::string ruleName;
    Node* lastFound = nullptr;
    for (int i = 1; i < inputLength - 1; i++) {
        if (!isWhitespace(input[i])) {
            if (input[i] == '}') {
                throw std::runtime_error("Unexpected reversed set closing bracket");
            }
            if (input[i] == '\\') {
                i++;
                if (i == inputLength - 1) {
                    throw std::runtime_error("No character after escape");
                }
            }
            ruleName += input[i];
            if (lastFound != nullptr && lastFound->getOp() == CONCAT) {
                lastFound->add_child(new Node(input[i]));
            } else if (lastFound != nullptr) {
                lastFound = new Node(CONCAT, {lastFound, new Node(input[i])});
            } else {
                lastFound = new Node(input[i]);
            }
        } else if (!ruleName.empty()) {
            rules.push_back(new RuleTree(ruleName, lastFound, RESERVED));
            ruleName = "";
            lastFound = nullptr;
        }
    }
    if (!ruleName.empty()) {
        rules.push_back(new RuleTree(ruleName, lastFound, RESERVED));
    }
}

static void parsePunctuation(const std::string& input, std::vector<RuleTree *>& rules) {
    const int inputLength = static_cast<int>(input.length());
    bool symbolsFound = false;
    for (int i = 1; i < inputLength - 1; i++) {
        if (!isWhitespace(input[i])) {
            if (input[i] == ']') {
                throw std::runtime_error("Unexpected punctuation closing bracket");
            }
            if (input[i] == '\\') {
                i++;
                if (i == inputLength - 1) {
                    throw std::runtime_error("No character after escape");
                }
            }
            rules.push_back(new RuleTree({input[i]}, new Node(input[i]), PUNCTUATION));
            symbolsFound = true;
        }
    }
    if (!symbolsFound) {
        throw std::runtime_error("Punctuation set cannot be empty");
    }
}

namespace RuleParser {
    std::vector<RuleTree *> parse(const std::string& input) {
        const std::vector<std::string> lines = split(input);
        std::unordered_map<std::string, Node *> definitions;
        std::vector<RuleTree *> rules;
        for (const auto& line: lines) {
            const int lineLength = static_cast<int>(line.length());
            if (lineLength < 2) {
                throw std::runtime_error("Unexpected rule length");
            }
            if (line[0] == '[' && line[lineLength - 1] == ']') {
                parsePunctuation(line, rules);
            } else if (line[0] == '{' && line[lineLength - 1] == '}') {
                parseReversed(line, rules);
            } else {
                parseDefExp(line, definitions, rules);
            }
        }
        return rules;
    }

    std::vector<RuleTree *> parseFromFile(const std::string& filename) {
        return parse(readRules(filename));
    }
};
