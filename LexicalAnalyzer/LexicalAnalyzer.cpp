#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <map>

#include "LexicalAnalyzer.h"
#include "../DFASerialization/DFASerialization.h"

static bool isWhitespace(const char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

Token::Token() {
    rule_priority = NORMAL;
}

Token::Token(const size_t fp, const std::tuple<std::string, Priority, int>& t, std::string l) {
    lexeme = std::move(l);
    filePos = fp;
    type = std::get<0>(t);
    rule_index = std::get<2>(t);
    rule_priority = std::get<1>(t);
}

Token::Token(const size_t fp, const std::tuple<std::string, Priority, int>& t, std::string l, std::string e) {
    lexeme = std::move(l);
    filePos = fp;
    type = std::get<0>(t);
    error = std::move(e);
    rule_index = std::get<2>(t);
    rule_priority = std::get<1>(t);
}

Token::Token(const size_t fp, std::string t, std::string l) {
    lexeme = std::move(l);
    filePos = fp;
    type = std::move(t);
    rule_priority = NORMAL;
}

Token::Token(const size_t fp, std::string t, std::string l, std::string e) {
    lexeme = std::move(l);
    filePos = fp;
    type = std::move(t);
    error = std::move(e);
    rule_priority = Priority::NORMAL;
}

void LexicalAnalyzer::panicModeRecovery(std::string* error, std::string* lexeme, int* state, int* errorLength) {
    // Panic Mode Recovery
    std::vector<char> new_buffer;
    if (!lexeme->empty()) {
        // discard first character of lexeme
        if (!isWhitespace((*lexeme)[0]) || !error->empty()) {
            *error += (*lexeme)[0];
            if (!isWhitespace((*lexeme)[0])) {
                *errorLength = static_cast<int>(error->size());
            }
        }

        new_buffer.insert(new_buffer.end(), lexeme->begin() + 1, lexeme->end());
    } else {
        if (!isWhitespace(buffer[bufferPos]) || !error->empty()) {
            *error += buffer[bufferPos];
            if (!isWhitespace(buffer[bufferPos])) {
                *errorLength = static_cast<int>(error->size());
            }
        }
        bufferPos++;
        lexemePosInFile++;
    }
    if (bufferPos < buffer.size()) {
        new_buffer.insert(new_buffer.end(), buffer.begin() + static_cast<long long>(bufferPos), buffer.end());
    }
    buffer = new_buffer;
    bufferPos = 0;

    // reset maximal munch variables for the retry.
    *lexeme = "";
    *state = start_state;
}

LexicalAnalyzer::LexicalAnalyzer(const std::string& input_path, size_t buffer_size, const std::string& DFA_path) {
    file = std::ifstream(input_path);
    buffer = std::vector<char>(buffer_size);
    bufferPos = 0;
    bufferSize = buffer_size;
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file: " + input_path);
    }

    // Read the initial chunk into the buffer
    file.read(buffer.data(), static_cast<long long>(buffer_size));
    // Resize the buffer to the actual number of bytes read
    std::streamsize bytesRead = file.gcount();
    buffer.resize(bytesRead);

    // Import the DFA
    if (DFASerialization::importDFA(dfa, start_state, final_states, DFA_path) == -1) {
        // TODO: handle importing DFA error (DFA_path doesn't exist)
        throw std::runtime_error("Error: Unable to import DFA from file: " + DFA_path);
    }
}

Token LexicalAnalyzer::getNextToken() {
    std::string lexeme;

    int maximalMunchType = -1;
    int maximalMunchEnd = 0; // an index within {lexeme} that marks the end of the maximal munch (not inclusive)
    size_t maximalMunchFilePos = 0; // the position of the first char of the maximal munch lexeme in the input file

    int state = start_state; // current state
    std::string error;
    int errorLength = 0;
    while (true) {
        // If the buffer is fully processed
        if (bufferPos >= buffer.size()) {
            // If the file has no more characters (EOF)
            if (file.eof()) {
                if (maximalMunchType != -1) {
                    Token maximalMunchToken(maximalMunchFilePos, final_states[maximalMunchType],
                                            lexeme.substr(0, maximalMunchEnd), error.substr(0, errorLength));
                    std::vector<char> new_buffer;
                    if (!lexeme.empty()) {
                        new_buffer.insert(new_buffer.end(), lexeme.begin() + maximalMunchEnd, lexeme.end());
                        // put the rest of lexeme back into the buffer (from the front)
                    }
                    buffer = new_buffer;
                    bufferPos = 0;
                    return maximalMunchToken;
                }
                if (lexeme.empty()) {
                    isEOF = true;
                    return Token{lexemePosInFile, "EOF", "", error.substr(0, errorLength)};
                }
                // Panic Mode Recovery
                panicModeRecovery(&error, &lexeme, &state, &errorLength);
                continue;
            }
            // Load the next chunk from the file into the buffer and set the position to the start of the buffer
            buffer.resize(bufferSize);
            file.read(buffer.data(), bufferSize);
            // Resize the buffer to the actual number of bytes read
            const std::streamsize bytesRead = file.gcount();
            buffer.resize(bytesRead);
            bufferPos = 0;
        }

        if (const char c = buffer[bufferPos]; dfa[state].find(c) == dfa[state].end() || isWhitespace(c)) {
            // Character not found in the DFA
            if (maximalMunchType == -1) {
                // Panic Mode Recovery
                panicModeRecovery(&error, &lexeme, &state, &errorLength);
            } else {
                // Maximal munch found
                Token maximalMunchToken(maximalMunchFilePos, final_states[maximalMunchType],
                                        lexeme.substr(0, maximalMunchEnd), error.substr(0, errorLength));
                // Update buffer and reset maximal munch variables
                std::vector<char> new_buffer;
                if (!lexeme.empty()) {
                    new_buffer.insert(new_buffer.end(), lexeme.begin() + maximalMunchEnd, lexeme.end());
                    // put the rest of lexeme back into the buffer (from the front)
                }
                if (bufferPos < buffer.size()) {
                    new_buffer.insert(new_buffer.end(), buffer.begin() + static_cast<long long>(bufferPos),
                                      buffer.end());
                }
                buffer = new_buffer;
                bufferPos = 0;
                return maximalMunchToken;
            }
        } else {
            // Character found in the DFA
            const int nextState = dfa[state][c];
            state = nextState;
            bufferPos++;
            lexeme += c;
            lexemePosInFile++;
            if (final_states.find(state) != final_states.end()) {
                // new state is final
                maximalMunchType = state;
                maximalMunchEnd = static_cast<int>(lexeme.length());
                maximalMunchFilePos = lexemePosInFile - lexeme.length();
            }
        }
    }
}
