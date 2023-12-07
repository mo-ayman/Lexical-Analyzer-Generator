#include "LexicalAnalyzer/LexicalAnalyzer.h"
#include <iomanip>

int main(const int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        return 1;
    }

    const auto dfa_path = std::string(argv[1]);
    const auto input_path = std::string(argv[2]);

    LexicalAnalyzer lexer(input_path, 1024, dfa_path);
    Token token;
    do {
        token = lexer.getNextToken();
        if (!token.error.empty()) {
            std::cout << "Error, removing \"" << token.error << "\"" << std::endl;
        }
        std::cout << std::setw(8) << "Pos: " << std::setw(5) << token.filePos
                << " | Type: " << std::setw(10) << token.type
                << " | Lexeme: \"" << token.lexeme << "\""
                << std::endl;
    } while (token.type != "EOF");
    return 0;
}
