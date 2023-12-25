#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>

#include "../LexicalRules/RuleTree.h"

class Token {
public:
    std::string lexeme; // the sequence of characters making the token
    size_t filePos{}; // the position of the start of the lexeme within the input file
    std::string type; // the name of the corresponding final state in the minimal DFA
    std::string error; // the sequence of characters that got removed by PMR before this token
    bool isEOF;

    int rule_index{}; // the index of the corresponding lexical rule
    Priority rule_priority; // the priority class of the corresponding lexical rule
    Token();
    explicit Token(bool boolisEOF);
    Token(size_t fp, const std::tuple<std::string, Priority, int>& t, std::string l);
    Token(size_t fp, const std::tuple<std::string, Priority, int>& t, std::string l, std::string e);
    Token(size_t fp, std::string t, std::string l);
    Token(size_t fp, std::string t, std::string l, std::string e);
};

class LexicalAnalyzer {
    // Input
    std::ifstream file;
    std::vector<char> buffer;
    size_t bufferPos{};
    size_t bufferSize{};
    size_t lexemePosInFile = 0; // the position of the last character of lexeme within the input file
    bool boolisEOF = false;
    // Dfa
    std::vector<std::map<char, int>> dfa;
    int start_state{};
    std::unordered_map<int, std::tuple<std::string, Priority, int>> final_states;

    void panicModeRecovery(std::string* error, std::string* lexeme, int* state);

public:
    ~LexicalAnalyzer() = default;

    LexicalAnalyzer(const std::string& input_path, size_t buffer_size, const std::string& DFA_path);

    Token getNextToken();

    bool isEOF() const;
};

#endif //LEXICALANALYZER_H
