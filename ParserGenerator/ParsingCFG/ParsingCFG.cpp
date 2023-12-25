#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "ParsingCFG.h"

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
    bool isPendingLineBreak = false;
    for (int i = 0; i < inputLength; i++) {
        if (lineStarted) {
            if (input[i] == '\n') {
                if (!isPendingLineBreak) {
                    pendingWhitespace += " ";
                    isPendingLineBreak = true;
                }
            } else if (isWhitespace(input[i])) {
                pendingWhitespace += input[i];
            } else if (isPendingLineBreak && input[i] == '#') {
                lines.push_back(currentLine);
                currentLine = "";
                pendingWhitespace = "";
                lineStarted = false;
                isPendingLineBreak = false;
            } else {
                currentLine += pendingWhitespace;
                currentLine += input[i];
                pendingWhitespace = "";
                isPendingLineBreak = false;
            }
        } else if (!isWhitespace(input[i]) && input[i] != '#') {
            currentLine += input[i];
            lineStarted = true;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}

static std::vector<std::vector<Definition *>> parseRule(const std::string& input, const int start,
                                                        std::unordered_map<std::string, Definition *>&
                                                        definitions_pool) {
    const int end = static_cast<int>(input.length());
    bool isInsideQuotes = false;
    std::string lastSeq;
    std::vector<std::vector<Definition *>> rule;
    rule.emplace_back();
    for (int i = start; i < end; i++) {
        if (input[i] == '\'') {
            if (isInsideQuotes && !lastSeq.empty()) {
                if (definitions_pool.find(lastSeq) == definitions_pool.end()) {
                    definitions_pool[lastSeq] = new Definition(lastSeq, true);
                }
                rule.back().push_back(definitions_pool.at(lastSeq));
                lastSeq = "";
            } else if (!isInsideQuotes && !lastSeq.empty()) {
                Definition* definition;
                if (definitions_pool.find(lastSeq) != definitions_pool.end()) {
                    definition = definitions_pool.at(lastSeq);
                } else {
                    definition = new Definition(lastSeq, false);
                    definitions_pool[lastSeq] = definition;
                }
                rule.back().push_back(definition);
                lastSeq = "";
            }
            isInsideQuotes = !isInsideQuotes;
        } else if (isInsideQuotes) {
            if (input[i] == '\\') {
                i++;
                if (i == end) {
                    throw std::runtime_error("No character after escape");
                }
            }
            lastSeq += input[i];
        } else if (input[i] == '|') {
            if (!lastSeq.empty()) {
                Definition* definition;
                if (definitions_pool.find(lastSeq) != definitions_pool.end()) {
                    definition = definitions_pool.at(lastSeq);
                } else {
                    definition = new Definition(lastSeq, false);
                    definitions_pool[lastSeq] = definition;
                }
                rule.back().push_back(definition);
                lastSeq = "";
            }
            if (rule.back().empty()) {
                throw std::runtime_error("OR operator must be preceded by an operand");
            }
            rule.emplace_back();
        } else if (!isWhitespace(input[i])) {
            if (lastSeq.empty() && input[i] == '\\') {
                i++;
                if (i == end) {
                    throw std::runtime_error("No character after escape");
                }
                if (input[i] == 'L') {
                    rule.back().push_back(Definition::getEpsilon());
                }
            } else if (!isValidRuleNameChar(input[i])) {
                throw std::runtime_error("Unexpected token");
            } else {
                lastSeq += input[i];
            }
        } else if (!lastSeq.empty()) {
            Definition* definition;
            if (definitions_pool.find(lastSeq) != definitions_pool.end()) {
                definition = definitions_pool.at(lastSeq);
            } else {
                definition = new Definition(lastSeq, false);
                definitions_pool[lastSeq] = definition;
            }
            rule.back().push_back(definition);
            lastSeq = "";
        }
    }
    if (!lastSeq.empty()) {
        Definition* definition;
        if (definitions_pool.find(lastSeq) != definitions_pool.end()) {
            definition = definitions_pool.at(lastSeq);
        } else {
            definition = new Definition(lastSeq, false);
            definitions_pool[lastSeq] = definition;
        }
        rule.back().push_back(definition);
    }
    if (rule.back().empty() && rule.size() == 1) {
        throw std::runtime_error("Rule cannot be empty");
    }
    if (rule.back().empty()) {
        rule.pop_back();
    }
    return std::move(rule);
}

static Definition* parseDefExp(const std::string& input,
                               std::unordered_map<std::string, Definition *>& definitions_pool,
                               std::map<Definition *, std::vector<std::vector<Definition *>>>& rules) {
    const int inputLength = static_cast<int>(input.length());
    std::string ruleName;
    int inputIterator = 0;
    bool pendingWhitespace = false;
    while (input[inputIterator] != '=') {
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
    Definition* defintion;
    if (definitions_pool.find(ruleName) != definitions_pool.end()) {
        defintion = definitions_pool.at(ruleName);
    } else {
        defintion = new Definition(ruleName, false);
        definitions_pool[ruleName] = defintion;
    }
    inputIterator++;
    rules[defintion] = parseRule(input, inputIterator, definitions_pool);
    return defintion;
}

namespace ParsingCFG {
    std::map<Definition *, std::vector<std::vector<Definition *>>> parse(const std::string& input, Definition** start) {
        const std::vector<std::string> lines = split(input);
        std::unordered_map<std::string, Definition *> definitions_pool;
        std::map<Definition *, std::vector<std::vector<Definition *>>> rules;
        int i = 0;
        for (const auto& line: lines) {
            const int lineLength = static_cast<int>(line.length());
            if (lineLength < 2) {
                throw std::runtime_error("Unexpected rule length");
            }
            const auto definition = parseDefExp(line, definitions_pool, rules);
            if (i == 0) {
                *start = definition;
            }
            i++;
        }
        for (const auto& definition: definitions_pool) {
            if (!definition.second->getIsTerminal() && rules.find(definition.second) == rules.end()) {
                throw std::runtime_error("Non-terminal rule " + definition.first + " is not defined");
            }
        }
        return rules;
    }

    std::map<Definition *, std::vector<std::vector<Definition *>>> parseFromFile(
        const std::string& filename, Definition** start) {
        return parse(readRules(filename), start);
    }

    void print(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules) {
        for (const auto& rule: rules) {
            std::cout << rule.first->getName() << " -> ";
            for (const auto& alternative: rule.second) {
                for (const auto& definition: alternative) {
                    if (definition->getIsTerminal()) {
                        std::cout << "'" << definition->getName() << "' ";
                    } else {
                        std::cout << definition->getName() << " ";
                    }
                }
                if (alternative != rule.second.back()) {
                    std::cout << "| ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};
