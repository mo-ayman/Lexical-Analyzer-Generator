
/*
maximal_munch_type = -1
maximal_munch_end = -1 (an index within lexeme that marks the end of the maximal munch)

state = start_state (current state)
while true:
    if buffer is fully processed, 
        if file had no more characters (EOF):
            break 
        load next chunk from file into buffer & set pos to the start of the buffer
    
    c = buffer[pos]
    next_state = move(state, c)
    if next_state doesn't exist (no such key as 'c' in dfa[s]):
        if maximal_munch_type == -1:
            handle error (return Token("Error", "") for now)
        else:
            maximal_munch_token = Token(final_states[maximal_munch_type], lexeme[:maximal_munch_end]) 
            buffer = lexeme[maximal_munch_end:] + buffer[pos:]    
            pos = 0

    else:
        state = next_state
        pos ++
        lexeme += c
        if(state is final):
            maximal_munch_type = final_states[state]
            maximal_munch_end = len(lexeme)

*/
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "../DFA_Serialization/DFA_serialization.cpp"

using namespace std;

class Token {
public:
    string type;
    string value;
    Token(){}
    Token(string t, const std::string& v) : type(t), value(v) {}
};

class LexicalAnalyzer {
private:
    // Input 
    ifstream file;
    vector<char> buffer;
    size_t bufferPos;

    // Dfa
    vector<map<char, int>> dfa;
    int start_state;
    map<int, string> final_states;

public:
    LexicalAnalyzer(const string& input_path, size_t buffer_size, const std::string& DFA_path)
        : file(input_path), buffer(buffer_size), bufferPos(0) {

        //TODO: handle when file doesn't exist
        //...

        // Read the initial chunk into the buffer
        file.read(buffer.data(), buffer.size());

        // Import the DFA
        if (import_DFA(dfa, start_state, final_states, DFA_path) == -1){
            //TODO: handle importing DFA error (DFA_path doesn't exist)
            //...
        }
    }

    Token getNextToken() {
        string lexeme = "";
        int maximalMunchType = -1;
        int maximalMunchEnd = 0; // an index within lexeme that marks the end of the maximal munch (not inclusive)
        int state = start_state; // current state

        while (true) {
            // If the buffer is fully processed
            if (bufferPos >= buffer.size()) {
                // If the file has no more characters (EOF)
                if (file.eof()) {
                    if (maximalMunchType != -1) {
                        Token maximalMunchToken(final_states[maximalMunchType], lexeme.substr(0, maximalMunchEnd));
                        buffer = vector<char>(lexeme.begin() + maximalMunchEnd, lexeme.end()); // put the rest of lexeme back into the buffer
                        bufferPos = 0;
                        return maximalMunchToken;
                    }
                    if(lexeme.empty()){
                        // Return an EOF token
                        return Token("EOF", "");
                    }
                    // Handle unmatched lexeme error (return Error Token for now)
                    return Token("Error", "");
                }
                // Load the next chunk from the file into the buffer and set the position to the start of the buffer
                file.read(buffer.data(), buffer.size());
                bufferPos = 0;
            }

            char c = buffer[bufferPos];

            if (dfa[state].find(c) == dfa[state].end()) {
                // Character not found in the DFA
                if (maximalMunchType == -1) {
                    // Handle unmatched lexeme error (return Error Token for now)
                    return Token("Error", "");
                } else {
                    // Maximal munch found
                    Token maximalMunchToken(final_states[maximalMunchType], lexeme.substr(0, maximalMunchEnd));
                    // Update buffer and reset maximal munch variables
                    vector<char> new_buffer = vector<char>(lexeme.begin() + maximalMunchEnd, lexeme.end()); // put the rest of lexeme back into the buffer (from the front)
                    new_buffer.insert(new_buffer.end(), buffer.begin() + bufferPos, buffer.end()); 
                    buffer = new_buffer;
                    bufferPos = 0;
                    maximalMunchType = -1;
                    return maximalMunchToken;
                }
            } else {
                // Character found in the DFA
                int nextState = dfa[state][c];
                state = nextState;
                bufferPos++;
                lexeme += c;
                if (final_states.find(state) != final_states.end()) {
                    // Current state is final
                    maximalMunchType = state;
                    maximalMunchEnd = lexeme.length();
                }
            }
        }

        // If there's a valid token at the end of the file
        if (maximalMunchType != -1) {
            return Token(final_states[maximalMunchType], lexeme.substr(0, maximalMunchEnd));
        }

        // Return an EOF token
        return Token("EOF", "");
    }
};


int main() {
    /*
    Question 2 in:
    https://drive.google.com/file/d/1e3cLv4e8OTNEiOp_TFDnFI853S1RElFh
    */
    vector<map<char, int>> v = {
        {{'a', 4}, {'b', 1}}, // x 0
        {{'a', 3}, {'b', 2}}, // z 1
        {{'a', 4}, {'b', 5}}, // w 2
        {{'a', 4}, {'b', 6}}, // u 3
        {{'a', 4}},           // y,T 4
        {{'a', 4}, {'b', 5}}, // k 5
        {}                    // M 6
    };
    int x = 0;
    map<int, string> f = {
        {3, "P3"},
        {6, "P2"},
        {2, "p1"},
        {4, "p3"}
    };
    export_DFA(v,x,f,"2018_q2.dat");

    LexicalAnalyzer lexer("2018_q2.txt", 1024, "2018_q2.dat");

    Token token;
    do {
        token = lexer.getNextToken();
        std::cout << "Type: " << token.type << ", Value: " << token.value << std::endl;
    }
    while (token.type != "EOF" && token.type != "Error");

    return 0;
}
