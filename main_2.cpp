#include <iostream>
#include <iomanip>
#include <fstream>
#include "LexicalAnalyzer/LexicalAnalyzer.h"

int main() {
    std::string src_path = __FILE__;
    src_path = src_path.substr(0, src_path.find_last_of("\\/"));

    std::ofstream outputFile(src_path+"/output.txt"); // Open a file for writing

    LexicalAnalyzer lexer(src_path + "/test_program.txt", 1024, src_path + "/min_dfa.dat");
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
            }
            outputFile << std::endl;
        }
        outputFile << std::setw(8) << "Pos: " << std::setw(5) << token.filePos
                    << " | Type: " << std::setw(10) << token.type
                    << " | Lexeme: \"" << token.lexeme << "\""
                    << std::endl;
    } while (token.type != "EOF");

    outputFile.close(); // Close the file when you're done
    return 0;
}

