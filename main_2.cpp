#include <iostream>
#include <iomanip>
#include <fstream>
#include "LexicalAnalyzer/LexicalAnalyzer.h"

int main(const int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Error: Invalid number of arguments." << std::endl;
        return 1;
    }

    const auto dfa_path = std::string(argv[1]);
    const auto input_path = std::string(argv[2]);

    std::string tokens_path;
    std::string verbose_path;
    bool verbose = false;

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 < argc) {
                tokens_path = argv[i + 1];
                i++;
            } else {
                std::cerr << "-o option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " <dfa_path> <input_path> -o <output_path> [--verbose]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -o <output_path>  Specify output file path." << std::endl;
            std::cout << "  --verbose         Output a file with verbose output for each token." << std::endl;
            return 0;
        }
    }

    if (tokens_path.empty()) {
        std::cerr << "Error: Output file path not specified." << std::endl;
        return 1;
    }

    if (verbose) {
        const std::string dir = tokens_path.substr(0, tokens_path.find_last_of("/\\") + 1);
        std::string file_name = tokens_path.substr(tokens_path.find_last_of("/\\") + 1);
        file_name = file_name.substr(0, file_name.find_last_of("."));
        const std::string extension = tokens_path.substr(tokens_path.find_last_of("."));
        verbose_path = dir + file_name + ".verbose" + extension;
    }

    std::ofstream verboseFile = verbose ? std::ofstream(verbose_path) : std::ofstream();
    std::ofstream tokensFile(tokens_path);

    LexicalAnalyzer lexer(input_path, 1024, dfa_path);

    Token token;
    do {
        token = lexer.getNextToken();
        if (!token.error.empty()) {
            verboseFile << std::setw(8) << "-- Error" << ", discarding ";
            std::cerr << "Error: Invalid tokens: ";
            int k = 0;
            for (char ch: token.error) {
                verboseFile << "'" << ch << "'";
                std::cerr << "'" << ch << "'";
                if (k < (token.error).size() - 1) {
                    verboseFile << ", ";
                    std::cerr << ", ";
                }
                k++;
                tokensFile << "Error" << std::endl;
            }
            verboseFile << std::endl;
            std::cerr << std::endl;
        }
        tokensFile << token.type << std::endl;
        verboseFile << std::setw(8) << "Pos: " << std::setw(5) << token.filePos
                << " | Type: " << std::setw(10) << token.type
                << " | Lexeme: \"" << token.lexeme << "\""
                << std::endl;
    } while (token.type != "EOF");

    tokensFile.close();
    verboseFile.close();
    return 0;
}
