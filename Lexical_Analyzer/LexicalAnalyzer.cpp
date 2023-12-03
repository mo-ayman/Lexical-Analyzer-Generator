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
    if next_state doesn't exist (no such key as 'c' in dfa[state]):
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
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cctype>
#include <vector>
#include <map>
#include "../DFA_Serialization/DFA_serialization.cpp"

using namespace std;

class Token
{
public:
    string lexeme;  // the sequence of characters making the token
    size_t filePos; // the position of the start of the lexeme within the input file
    string type;    // the name of the corresponding final state in the minimal DFA

    string error;   // the sequence of characters that got removed by PMR before this token

    int rule_index; // the index of the corresponding lexical rule
    Priority rule_priority; // the priority class of the corresponding lexical rule

    Token() {}
    Token(size_t fp, const tuple<string, Priority, int> t,  string l) : filePos(fp), type(get<0>(t)), rule_priority(get<1>(t)), rule_index(get<2>(t)), lexeme(l), error("") {}
    Token(size_t fp, const tuple<string, Priority, int> t,  string l, const string& e) : filePos(fp), type(get<0>(t)), rule_priority(get<1>(t)), rule_index(get<2>(t)), lexeme(l), error(e) {}
    Token(size_t fp, string t, const string &l) : filePos(fp), type(t), lexeme(l), error("") {}
    Token(size_t fp, string t, const string &l, const string &e) :filePos(fp), type(t), lexeme(l), error(e) {}
};

class LexicalAnalyzer
{
private:
    // Input
    ifstream file;
    vector<char> buffer;
    size_t bufferPos;
    size_t lexemePosInFile = 0; // the position of the last character of lexeme within the input file
    bool isEOF = false;
    
    // Dfa
    vector<map<char, int>> dfa;
    int start_state;
    unordered_map <int, tuple<string, Priority, int>> final_states;

    bool isWhitespace(char c)
    {
        // Use the isspace function from the cctype header
        return std::isspace(static_cast<unsigned char>(c)) != 0;
    }

    void panicModeRecovery(string *error, string *lexeme, int *state)
    {
        // Panic Mode Recovery
        vector<char> new_buffer;
        if (lexeme->size() >= 1)
        {
            // discard first character of lexeme
            if (!isWhitespace((*lexeme)[0]))
            {
                *error += (*lexeme)[0];
            }            
            new_buffer.insert(new_buffer.end(), lexeme->begin() + 1, lexeme->end());
        }
        else
        {
            if (!isWhitespace(buffer[bufferPos]))
            {
                *error += buffer[bufferPos];
            }
            bufferPos++;
            lexemePosInFile++;
        }
        if (bufferPos < buffer.size())
        {
            new_buffer.insert(new_buffer.end(), buffer.begin() + bufferPos, buffer.end());
        }
        buffer = new_buffer;
        bufferPos = 0;
        
        // reset maximal munch variables for the retry.
        *lexeme = "";
        *state = start_state;
    }

public:
    LexicalAnalyzer(const string &input_path, size_t buffer_size, const std::string &DFA_path)
        : file(input_path), buffer(buffer_size), bufferPos(0)
    {
        if (!file.is_open()) {
            throw std::runtime_error("Error: Unable to open file: " + input_path);
        }

        // Read the initial chunk into the buffer
        file.read(buffer.data(), buffer_size);
        // Resize the buffer to the actual number of bytes read
        std::streamsize bytesRead = file.gcount();
        buffer.resize(bytesRead);

        // Import the DFA
        if (import_DFA(dfa, start_state, final_states, DFA_path) == -1)
        {
            // TODO: handle importing DFA error (DFA_path doesn't exist)
            throw std::runtime_error("Error: Unable to import DFA from file: " + DFA_path);
        }
    }

    Token getNextToken()
    {
        string lexeme = "";
        
        int maximalMunchType = -1; 
        int maximalMunchEnd = 0; // an index within {lexeme} that marks the end of the maximal munch (not inclusive)
        size_t maximalMunchFilePos = 0; // the position of the first char of the maximal munch lexeme in the input file
        
        int state = start_state; // current state
        string error = "";
        while (true)
        {
            // If the buffer is fully processed
            if (bufferPos >= buffer.size())
            {
                // If the file has no more characters (EOF)
                if (file.eof())
                {
                    if (maximalMunchType != -1)
                    {
                        Token maximalMunchToken(maximalMunchFilePos, final_states[maximalMunchType], lexeme.substr(0, maximalMunchEnd), error);
                        vector<char> new_buffer;
                        if (!lexeme.empty())
                        {
                            new_buffer.insert(new_buffer.end(), lexeme.begin() + maximalMunchEnd, lexeme.end()); // put the rest of lexeme back into the buffer (from the front)
                        }
                        buffer = new_buffer;
                        bufferPos = 0;
                        return maximalMunchToken;
                    }
                    if (lexeme.empty())
                    {
                        isEOF = true;
                        return Token(lexemePosInFile, "EOF", "");
                    }
                    // Panic Mode Recovery
                    panicModeRecovery(&error, &lexeme, &state);
                    continue;
                }
                // Load the next chunk from the file into the buffer and set the position to the start of the buffer
                file.read(buffer.data(), buffer.size());
                // Resize the buffer to the actual number of bytes read
                std::streamsize bytesRead = file.gcount();
                buffer.resize(bytesRead);
                bufferPos = 0;
            }

            char c = buffer[bufferPos];
            if (dfa[state].find(c) == dfa[state].end() || isWhitespace(c))
            {
                // Character not found in the DFA
                if (maximalMunchType == -1)
                {
                    // Panic Mode Recovery
                    panicModeRecovery(&error, &lexeme, &state);
                    continue;
                }
                else
                {
                    // Maximal munch found
                    Token maximalMunchToken(maximalMunchFilePos, final_states[maximalMunchType], lexeme.substr(0, maximalMunchEnd), error);
                    // Update buffer and reset maximal munch variables
                    vector<char> new_buffer;
                    if (!lexeme.empty())
                    {
                        new_buffer.insert(new_buffer.end(), lexeme.begin() + maximalMunchEnd, lexeme.end()); // put the rest of lexeme back into the buffer (from the front)
                    }
                    if (bufferPos < buffer.size())
                    {
                        new_buffer.insert(new_buffer.end(), buffer.begin() + bufferPos, buffer.end());
                    }
                    buffer = new_buffer;
                    bufferPos = 0;
                    return maximalMunchToken;
                }
            }
            else
            {
                // Character found in the DFA
                int nextState = dfa[state][c];
                state = nextState;
                bufferPos++;
                lexeme += c;
                lexemePosInFile ++;
                if (final_states.find(state) != final_states.end())
                {
                    // new state is final
                    maximalMunchType = state;
                    maximalMunchEnd = lexeme.length();
                    maximalMunchFilePos = lexemePosInFile - lexeme.length();
                }
            }
        }
    }
};

int main()
{
    /*
    Question 2 in:
    https://drive.google.com/file/d/1e3cLv4e8OTNEiOp_TFDnFI853S1RElFh
    */
    vector<map<char, int>> v = {
        {{'a', 4}, {'b', 1}, {'c', 7}}, // x 0
        {{'a', 3}, {'b', 2}}, // z 1
        {{'a', 4}, {'b', 5}}, // w 2
        {{'a', 4}, {'b', 6}}, // u 3
        {{'a', 4}},           // yT 4
        {{'a', 4}, {'b', 5}}, // k 5
        {},                    // M 6
        {}
    };
    int x = 0;
    unordered_map<int, tuple<string, Priority, int>> f = {
        {2, make_tuple("p1", Priority(2), 2)},
        {3, make_tuple("P3", Priority(2), 3)},
        {4, make_tuple("p3", Priority(2), 4)},
        {6, make_tuple("P2", Priority(2), 6)},
        };
    export_DFA(v, x, f, "2018_q2.dat");

    LexicalAnalyzer lexer("2018_q2.txt", 1024, "2018_q2.dat");

    Token token;
    do
    {
        token = lexer.getNextToken();
        if (!token.error.empty())
        {
            cout << "Error, removing \"" << token.error << "\"" << endl;
        }
        cout << "Pos: "<< token.filePos << ", Type: " << token.type << ", Lexeme: \"" << token.lexeme << "\"" << endl;
    } while (token.type != "EOF");
    return 0;
}
