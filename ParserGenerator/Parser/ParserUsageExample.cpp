#include <list>

#include "../../LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.h"
#include "Parser.h"
using namespace std;

// Mock class derived from LexicalAnalyzer
class MockLexicalAnalyzer final : public LexicalAnalyzer {
    vector<Token> Tokens;
    size_t index;

public:
    explicit MockLexicalAnalyzer(const list<string>& tokens) {
        for (const string& token: tokens) {
            Tokens.emplace_back(0, token, token + "_lexeme");
        }
        Tokens[Tokens.size() - 1].isEOF = true;
        index = 0;
    }

    Token getNextToken() override {
        return Tokens[std::min(index++, Tokens.size() - 1)];
    }

    bool isEOF() const override {
        return index >= Tokens.size();
    };
};

int main(const int argc, char** argv) {
    // Sheet 5, Question 2
    const Definition S("S", false), R("R", false), U("U", false), V("V", false), T("T", false);
    const Definition s("s"), u("u"), v("v"), b("b"), t("t");
    const auto epsilon=Definition::getEpsilon();
    const auto eof=Definition::getDollar();

    unordered_map<const Definition *, unordered_map<string, vector<const Definition *>>> table;
    table[&S] = unordered_map<string, vector<const Definition *>>();
    table[&S][s.getName()] = vector<const Definition *>();
    table[&S][v.getName()] = vector<const Definition *>();
    table[&S][t.getName()] = vector<const Definition *>();

    table[&S][s.getName()].push_back(&R);
    table[&S][s.getName()].push_back(&T);
    table[&S][v.getName()].push_back(&R);
    table[&S][v.getName()].push_back(&T);
    table[&S][t.getName()].push_back(&R);
    table[&S][t.getName()].push_back(&T);
    table[&S][eof->getName()] = vector<const Definition *>();
    table[&S][eof->getName()].push_back(&R);
    table[&S][eof->getName()].push_back(&T);

    table[&R] = unordered_map<string, vector<const Definition *>>();
    table[&R][s.getName()] = vector<const Definition *>();
    table[&R][v.getName()] = vector<const Definition *>();
    table[&R][t.getName()] = vector<const Definition *>();
    table[&R][b.getName()] = vector<const Definition *>();
    table[&R][eof->getName()] = vector<const Definition *>();
    table[&R][s.getName()].push_back(&s);
    table[&R][s.getName()].push_back(&U);
    table[&R][s.getName()].push_back(&R);
    table[&R][s.getName()].push_back(&b);
    table[&R][v.getName()].push_back(epsilon);
    table[&R][t.getName()].push_back(epsilon);
    table[&R][b.getName()].push_back(epsilon);
    table[&R][eof->getName()].push_back(epsilon);

    table[&U] = unordered_map<string, vector<const Definition *>>();
    table[&U][s.getName()] = vector<const Definition *>();
    table[&U][v.getName()] = vector<const Definition *>();
    table[&U][u.getName()] = vector<const Definition *>();
    table[&U][b.getName()] = vector<const Definition *>();
    table[&U][s.getName()].push_back(epsilon);
    table[&U][u.getName()].push_back(&u);
    table[&U][u.getName()].push_back(&U);
    table[&U][b.getName()].push_back(epsilon);

    table[&V] = unordered_map<string, vector<const Definition *>>();
    table[&V][v.getName()] = vector<const Definition *>();
    table[&V][t.getName()] = vector<const Definition *>();
    table[&V][v.getName()].push_back(&v);
    table[&V][v.getName()].push_back(&V);
    table[&V][t.getName()].push_back(epsilon);

    table[&T] = unordered_map<string, vector<const Definition *>>();
    table[&T][v.getName()] = vector<const Definition *>();
    table[&T][t.getName()] = vector<const Definition *>();
    table[&T][eof->getName()] = vector<const Definition *>();
    table[&T][v.getName()].push_back(&V);
    table[&T][v.getName()].push_back(&t);
    table[&T][v.getName()].push_back(&T);
    table[&T][t.getName()].push_back(&V);
    table[&T][t.getName()].push_back(&t);
    table[&T][t.getName()].push_back(&T);
    table[&T][eof->getName()].push_back(epsilon);

    // Parsing (Syntax Analysis)
    auto parser = Parser(table);
    auto lex = (MockLexicalAnalyzer(
        {"s", "s", "u", "u", "b", "b", "t", "v", "t", "$"}));

    auto root = parser.parse(lex, &S);
    // Write Derivation steps
    std::ofstream outputFile("Sheet5_Q2_LeftMostDerivation.txt");
    if (outputFile.is_open()) {
        outputFile << *root;
        outputFile.close();
    } else {
        std::cerr << "Unable to open the file for writing." << std::endl;
    }
    // Export tree graph
    root->plotGraph("Sheet5_Q2_parseTree");
    return 0;
}
