#include <iostream>
#include <iomanip>
#include <fstream>
#include "LexicalAnalyzer/LexicalAnalyzer.h"

int main(const int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        return 1;
    }

    const auto dfa_path = std::string(argv[1]);
    const auto input_path = std::string(argv[2]);
    const auto output_path = std::string(argv[3]);
    const auto tokens_path = output_path.substr(0, output_path.find_last_of("\\/")+1) + "output_tokens.txt";

    std::ofstream outputFile(output_path);
    std::ofstream tokensFile(tokens_path);

    LexicalAnalyzer lexer(input_path, 1024, dfa_path);

    Token token;
    do {
        token = lexer.getNextToken();
        if (!token.error.empty()) {
            outputFile << std::setw(8) << "-- Error" << ", discarding ";
            int k=0;
            for (char ch : token.error) {
                outputFile << "'" << ch << "'";
                if(k<(token.error).size()-1) {
                    outputFile << ", ";
                }
                k++;
                tokensFile << "Error" << std::endl;
            }
            outputFile << std::endl;
        }
        tokensFile << token.type << std::endl;
        outputFile << std::setw(8) << "Pos: " << std::setw(5) << token.filePos
                    << " | Type: " << std::setw(10) << token.type
                    << " | Lexeme: \"" << token.lexeme << "\""
                    << std::endl;
    } while (token.type != "EOF");

    tokensFile.close();
    outputFile.close();
    return 0;
}

