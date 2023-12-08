# Lexical Analyzer Generator

## Overview

This project is a lexical analyzer generator written in C++. It compiles a set of lexical rules defined in a text file into a minimal deterministic finite automaton (DFA) that can be used later by a lexical analyzer (included here as a separate executable) to tokenize a given input.

The whole process is done through a pipeline of four stages: **Rules File** --(input to stage 1)--> **Rules Parser** --(rule trees to stage 2)--> **NFA Generator** --(NFA to stage 3)--> **DFA Generator** --(DFA to stage 4)--> **DFA Minimizer** --(minimized DFA to output)--> **Serialized Minimized DFA File**.

Later, the lexical analyzer reads the serialized minimized DFA file and uses it to tokenize the input into a sequence of tokens. Each token holds information about its place in the input, which is useful in further stages of the compiler.

## Rules File

The rules file is a text file that contains the lexical rules. Each rule is written in a separate line. The syntax of the rules file follows the following rules:

- Each line is either a rule or a definition. There is an exception for reserved words and punctuations which are written in a single line, but they are parsed as multiple rules.
- Regular definitions are lines in the form LHS = RHS, where LHS is the name of the definition and RHS is the regular expression defining it. LHS can only contain alphanumeric characters and underscores. RHS can also contain references to other definitions.
- Regular expressions are lines in the form LHS: RHS, where LHS is the name of the token defined by the rule and RHS is the regular expression defining it. LHS can only contain alphanumeric characters and underscores. RHS can contain references to other definitions.
- Reserved words are lines in the form {word1 word2 word3 ...}. Reserved words are parsed as multiple rules, where each word is a rule with the same name as the word itself. Only the symbol `}` needs to be escaped by a backslash.
- Punctuations are lines in the form [char1 char2 char3 ...]. Punctuations are parsed as multiple rules, where each character is a rule with the same name as the character itself. Only the symbol `]` needs to be escaped by a backslash.
- Regular expressions can contain the following operators: concatenation, union, Kleene star, Kleene plus, Kleene question. Grouping is done using parentheses. A backslash followed by `L` is used to represent an epsilon transition. A backslash followed by any other character is used to represent the character itself, where it is useful for escaping special characters like `|` and `*`. A set of ranged characters can be represented using square braces. For example, `[a-z]` represents all lowercase letters, while [ax-z] represents the set of characters `a, x, y, z`.
- Whitespaces are always ignored but cannot split the rule name or the definition name. For example, `a b c = d e f` is invalid definition, while `abc = d e f` is valid.

The following is an example of a valid rules file:

```text
letter = [a-z] | [A-Z]
digit = [0-9]
id: letter (letter|digit)*
digits = digit+
{boolean int float}
num: digit+ | digit+ . digits (\L | E digits)
relop: \=\= | != | > | >= | < | <=
assign: =
{ if else while }
[; , \( ) { }]
addop: \+ | -
mulop: \* | /
```

## Rules Parser

Our implementation of the rules parser is hard-wired and well optimized for the syntax of the rules file. Parsing is done approximately in a single pass with some exceptions as described below. Thus, average complexity of the parser is linear in the size of the input. Each rule is parsed into a different tree reflecting the order of operations (e.g. concatenation before union), which facilitates the generation of the NFA using the Thompson's construction algorithm. If the rules file is not valid, the parser will throw an exception with a detailed error message.

### Data Structures

As mentioned above, each rule is parsed into a tree. The tree is represented by the class `RuleTree` which has the following public fields:

```cpp
enum Priority {
    PUNCTUATION,
    RESERVED,
    NORMAL
};

class RuleTree {
    std::string name;
    Priority priority;
    Node* root;
}
```

- `name` is the name given to the token defined by the rule. For reserved words, the name is the word itself. For punctuation tokens, the name is the character itself.

- `priority` is the priority of the rule based on the type of the token it represents. Punctuation tokens have the highest priority, followed by reserved words, and finally normal tokens. Normal tokens are further sorted based on the order of their appearance in the rules file, where the first token has the highest priority. This is useful for breaking ties.

- `root` is a pointer to the root node of the tree. The `Node` class represents a node in the tree. It has the public fields described below.

```cpp
enum Operator {
    CONCAT,
    STAR,
    PLUS,
    OR,
    QUESTION,
    EPSILON,
    LEAF_NODE,
    NONE
};

class Node {
    char terminal;
    Operator op;
    std::vector<Node *> children;
}
```

- `terminal` field only makes sense if the node is a leaf node, where it represents the terminal symbol.

- `op` field determines the operation done by the node on its children if any (e.g. concatenation, union, etc.) or whether it is a leaf node or an epsilon node. The `NONE` operator is dummy and is only there as a default value.

- `children` field is a vector of pointers to the children of the node. Leaf nodes and epsilon nodes have no children, while nodes doing unary operations (e.g. star, plus, question) have one child. Binary operations can take infinite number of children. That's because the example rule `a | b | c | d ` is parsed into a tree with a single `OR` node that has four children which are: `a, b, c, d`.

### Parsing Algorithm

This section describes the parsing algorithm briefly without going into much detail. The algorithm is implemented in the file [rules_parser.cpp](./LexicalRules/RuleParser.cpp).

**Main Routine**

1. Rules file is read from the file system.
2. Lines are extracted from the read content. Each line is trimmed from both sides and empty lines are discarded.
3. A vector of rule trees is created for the parsed rules.
4. A hashmap is created for the parsed definitions. It maps the name of the definition to the node representing it.
5. For each line in order, the following steps are done:
    1. If the line starts and ends with square braces, it is a set of punctuations and is parsed using **Parse Punctuation Routine**.
    2. If the line starts and ends with curly braces, it is a set of reserved words and is parsed using **Parse Reserved Routine**.
    3. Otherwise, the line is a normal rule and is parsed using **Parse Normal Routine**.
6. The vector of rule trees is returned to the caller.

**Parse Punctuation Routine**

1. For each character between the square braces:
    1. If the character is a whitespace, it is ignored.
    2. If the character is an unescaped closing square brace, an exception is thrown.
    3. Otherwise, a new rule tree is created whose name is the character itself and its priority is `PUNCTUATION`. The root of the tree is a leaf node with the character as its terminal. The tree is added to the vector of rule trees.
2. If no rules were found, an exception is thrown.

**Parse Reserved Routine**

1. Declare variables to track the reserved word being parsed.
2. For each character between the curly braces:
    1. If the character is a whitespace, it is ignored. In this case, if a reserved word has been found, a new rule tree is created whose name is the reserved word and its priority is `RESERVED`. The root of the tree is a concatenation node with the characters of the reserved word as its children. The tree is added to the vector of rule trees. Reserved word trackers are reset.
    2. If the character is an unescaped closing curly brace, an exception is thrown.
    3. Otherwise, the character is appended to the reserved word being parsed.
3. If no rules were found, an exception is thrown.

**Parse Normal Routine**

1. Keep reading characters until a colon or an equal sign is encountered. The characters read are considered as the name of the rule. Rule name is trimmed from both sides and an exception is thrown if it is empty. Also, a name can only contain alphanumeric characters and underscores. Otherwise, an exception is thrown.
2. If no colon or equal sign is encountered, an exception is thrown.
3. If symbol found is a colon, then the rule is a rule. If it is an equal sign, then the rule is a definition.
4. Range of characters between the symbol and the end of the line is parsed using **Parse Rule Routine**.
5. If the rule is a rule, then a new rule tree is created whose name is the name of the rule and its priority is `NORMAL`. The root of the tree is the node returned from **Parse Rule Routine**. The tree is added to the vector of rule trees.
6. If the rule is a definition, then a new entry is added to the hashmap of definitions. The key is the name of the definition and the value is the node returned from **Parse Rule Routine**.

**Parse Rule Routine**

1. Declare variables to track the current `OR` node and the current non-`OR` node being built. The non-`OR` node is a candidate for being a child of the `OR` node if `|` symbol is encountered.
2. Declare a variable to track the last parsed word which is a candidate for being a reference to a definition.
3. Declare a variable indicating if the immediate previous parsed entity has just been concatenated using the current `CONCAT` node.
4. For each character in the received range:
    1. If the character is an opening parenthesis, its corresponding closing parenthesis is searched for and the substring between them is parsed recursively using **Parse Rule Routine**. The resulting tree is added as a child to the current `CONCAT` node if any or is concatenated with the current non-`OR` node if none exists. If no non-`OR` node exists, then the tree is considered as the current non-`OR` node. Iteration continues from the character after the closing parenthesis.
    2. If the character is an opening square brace, its corresponding closing square brace is searched for and the substring between them is parsed recursively using **Parse Set Routine**. The resulting tree is added as a child to the current `CONCAT` node if any or is concatenated with the current non-`OR` node if none exists. If no non-`OR` node exists, then the tree is considered as the current non-`OR` node. Iteration continues from the character after the closing square brace.
    3. If the character is `|` symbol, then the current non-`OR` node is added as a child to the current `OR` node if any or a new `OR` node is created with the current non-`OR` node as its first child. The current non-`OR` node is reset. If no non-`OR` node exists, an exception is thrown.
    4. If the character is `*` symbol, then the immediate previous parsed entity is replaced with a `STAR` node with the immediate previous parsed entity as its child. If no previous parsed entity exists, an exception is thrown.
    5. If the character is `+` symbol, then the immediate previous parsed entity is replaced with a `PLUS` node with the immediate previous parsed entity as its child. If no previous parsed entity exists, an exception is thrown.
    6. If the character is `?` symbol, then the immediate previous parsed entity is replaced with a `QUESTION` node with the immediate previous parsed entity as its child. If no previous parsed entity exists, an exception is thrown.
    7. If the character is a whitespace, it is ignored. The variable tracking the last parsed word is reset.
    8. If the character completes the last parsed word and makes it a valid reference to a definition, then the definition is got from the hashmap of definitions. The node representing the definition is added as a child to the current non-`OR` node if any or is considered as the current non-`OR` node if none exists.
    9. If the character is a backslash followed by `L`, then an epsilon node is added as a child to the current `CONCAT` node if any or is concatenated with the current non-`OR` node if none exists. If no non-`OR` node exists, then the node is considered as the current non-`OR` node.
    10. Otherwise, the character is considered as a terminal and the leaf node holding it is added as a child to the current `CONCAT` node if any or is concatenated with the current non-`OR` node if none exists. If no non-`OR` node exists, then the node is considered as the current non-`OR` node.
5. If `OR` node already has children, then the current non-`OR` node is added as a child to the current `OR` node. Reset the current `OR` node which will be the root node of the rule tree or a substitution for an expression in parentheses.
6. If a non-`OR` node exists, then return it. It will be the root node of the rule tree or a substitution for an expression in parentheses.
7. Otherwise, an exception is thrown.

**Parse Set Routine**

1. Declare a set of characters to hold the parsed unique characters.
2. For each character in the received range:
    1. If the character has two following characters where the first of them is `-`, then the range of characters between the character and the second following character is added to the set of characters. If the ASCII code of the second following character is less than the ASCII code of the character, an exception is thrown.
    2. Otherwise, the character is added to the set of characters.
3. If no characters were found, an exception is thrown.
4. A new `OR` node is created whose children are leaf nodes with the characters in the set as their terminals. The node is returned to the caller.

## NFA Generator

Class `NFAConstruction` involved in constructing a Non-Deterministic Finite Automaton (NFA) from lexical rules represented by a `RuleTree`.

The key components and data structures used in this class:

### Data Members:

1. **`stateCount`**: An integer representing the count of states in the NFA.

2. **`startStateIndex`**: An integer representing the index of the start state in the NFA.

3. **`EPS`**: A character constant representing the epsilon transition in the NFA.

4. **`nfs`**: A vector of maps, where each map represents the transitions from a state based on input characters. The vector as a whole represents the entire NFA.

5. **`finalStates`**: An unordered map where each entry consists of a state index and a tuple containing the token name, priority, and associated rule index for final states.

### Member Functions:

1. **`constructNFA`**: Takes a vector of `RuleTree*` (presumably representing lexical rules) and constructs the NFA accordingly.

2. **`getNfs`**: Returns the vector of maps representing the NFA transitions.

3. **`getFinalStates`**: Returns the unordered map of final states with associated information.

4. **`getStartStateIndex`**: Returns the index of the start state in the NFA.

5. **`print`**: Prints information about the NFA.

6. **`performOperation`**: Performs an operation on a node of the `RuleTree`.

7. Functions with names like `concatOp`, `starOp`, `plusOp`, `orOp`, `questionOp`, `leafOp`, `noneOp`, and `epsilonOp`: These functions handle specific operations on nodes and return information about the resulting transitions.

8. **`addState`**: Adds a new state to the NFA and returns its index.

### Usage of Data Structures:

- **Vectors and Maps**: Utilized for representing the NFA transitions. The outer vector (`nfs`) represents states, and each map inside represents transitions from that state based on input characters.

- **Unordered Map (`finalStates`)**: Used to store information about final states, associating state indices with token names, priorities, and rule indices.

- **Tuples**: Used in `finalStates` to store multiple pieces of information (token name, priority, and rule index) associated with a state.

- **Integer Variables (`stateCount`, `startStateIndex`)**: Keep track of the number of states and the index of the start state.

- **Characters (`EPS`)**: Represents the epsilon transition.

### Methods Implementation:

1. **`NFAConstruction` (Constructor):**
   - Initializes the data members, including `stateCount`, `startStateIndex`, `nfs` (vector of maps representing NFA transitions), and `finalStates` (unordered map for final states).

2. **`constructNFA` Function:**
   - Takes a vector of `RuleTree*` representing lexical rules.
   - Initializes the start state and iterates over each rule.
   - Calls `performOperation` on the root of each rule's `RuleTree` to construct the NFA.
   - Updates the NFA transitions (`nfs`) and final states (`finalStates`) based on the constructed NFA.

3. **`performOperation` Function:**
   - Takes a `Node*` representing a node in the `RuleTree` and performs an operation based on its type.
   - Utilizes a switch statement to determine the type of operation and calls the corresponding private method for that operation.
   - Returns information about the resulting transitions, including the transition character and the first and last state indices.

4. **`leafOp` Function:**
   - Takes a `Node*` representing a leaf node in the `RuleTree`.
   - Adds a new state to the NFA and returns a pair containing the terminal character and a tuple with the state index as both the first and last state indices.

5. **`concatOp` Function:**
   - Takes a vector of `Node*` representing nodes in the `RuleTree` for concatenation.
   - Calls `performOperation` on each node in sequence and connects the resulting NFA chains through concatenation.
   - Returns a pair containing the transition character and the first and last state indices of the concatenated chain.

6. **`starOp` Function:**
    - Takes a vector of `Node*` representing nodes in the `RuleTree` for the Kleene star operation.
    - Calls `plusOp` to construct the NFA for one or more occurrences and adds an epsilon transition for zero occurrences.
    - Returns a pair containing the transition character and the first and last state indices.

7. **`plusOp` Function:**
    - Takes a vector of `Node*` representing nodes in the `RuleTree` for the Kleene plus operation.
    - Constructs the NFA for one or more occurrences and adds epsilon transitions for looping.
    - Returns a pair containing the epsilon transition character and the first and last state indices.

8. **`orOp` Function:**
    - Takes a vector of `Node*` representing nodes in the `RuleTree` for the alternation operation.
    - Constructs separate NFAs for each node and connects them through epsilon transitions.
    - Returns a pair containing the epsilon transition character and the first and last state indices.

9. **`questionOp` Function:**
    - Takes a vector of `Node*` representing nodes in the `RuleTree` for the optional (zero or one) operation.
    - Constructs the NFA for zero or one occurrences and adds epsilon transitions accordingly.
    - Returns a pair containing the epsilon transition character and the first and last state indices.

10. **`noneOp` Function:**
    - Takes a vector of `Node*` representing nodes in the `RuleTree` for an undefined (NONE) operation.
    - Throws a runtime error since this operation is not defined in the implementation.

11. **`epsilonOp` Function:**
    - Takes a `Node*` representing a node in the `RuleTree` for the epsilon operation.
    - Adds a new state to the NFA and returns a pair containing the epsilon transition character and the first and last state indices.

12. **`addState` Function:**
    - Adds a new state to the NFA (`nfs`) and increments the `stateCount`.
    - Returns the index of the newly added state.

Each function contributes to the overall construction of the NFA based on the given lexical rules, implementing different operations on the `RuleTree` nodes to build the corresponding NFA structure.

## DFA Generator

In this part I take the εNFA table in the form of `vector<map<char, vector<int>>>` as it contains vector of maps in each map, we represent the transition states under their inputs of character.

My role is to call Class `EpsilonNFA`. In this class I take transition table, final states and initial states to convert from εNFA to NFA.

After that I call some methods in DFA class to convert from NFA states to DFA and return Transition table in form `vector<map<char, int>>` it’s a vector of states represented by indices and each state under certain input `char` go to different state index in map of each state, also I return final states in form of `unordered_map<int, tuple<string, Priority, int>>` unordered map that maps each final state index in transition table of DFA to a tuple of three elements string: which represent final state name like it represent (abb for example), Priority that include three elements `PUNCTUATION, RESERVED, NORMAL` by their orders and int that represent majority of priority (0) most important.

In DFA, I used a queue to search states by visiting each state then pop it and push its reached states.

**Some Methods Definitions in DFA class:**

1. `DFA::DFA(const std::vector<std::map<char, std::vector<int>>>& table,const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finals, const int initial)` 
    - This constructor first calls the Epsilon_NFA_NFA class to convert epsilon_NFA states to its equivalent NFA.
2. `std::vector<std::map<char, int>> DFA::getDFA()`
    - This method first push into queue the states and calling method HandleState for each state which in turn (This function i fill stateMap that map each state to it's transition states that first state map to them and fill InputMap which map each state with it's input chars ). Finally calling  fillDFA func(This method fills DFA_States vector of each map by getting each state index and replacing it with a set of states corresponding).
3. `void DFA::fillDFA()`
    - This method fills the DFA_States vector of each map by getting each state index and replacing it with a set of states corresponding go to implementation to understand it well.
4. `std::map<std::set<int>, int> DFA::makeEachStateIndx()`
    - This method used to map each set to its index in Transition table to used in  fillDFA() func  when update DFA_states
5. `void DFA::handleState(std::set<int>& states)`
    - This function i fill stateMap that map each state to it's transition states that first state map to them and fill InputMap which map each state with it's input chars this functions help me in  getDFA func
6. `void DFA::updateFinalStates(int indx, const std::set<int>& OldState)` 
    - This method makes new final states by applying if any state of old ones contains one of the final states then take that one as the final state by taking the most  priority one.

**Some Methods Definitions in Epsilon_NFA_NFA class:**
1. `EpsilonNFA::EpsilonNFA(const std::vector<std::map<char, std::vector<int>>>& table,const std::unordered_map<int, std::tuple<std::string, Priority, int>>& finals,const int initial)` 
     - This is the constructor of NFA, first for loop is to convert vector in map of transition table to set to ease some functions in future.
2. Other methods used to convert Epsilon_NFA_NFA to NFA and are commented in code.


## DFA Minimizer

The DFA minimization algorithm is based on Hopcroft's algorithm with some modifications. The goal is to minimize the number of states in a deterministic finite automaton (DFA) while preserving its language recognition capabilities.

### Steps

1. **Initialize partitions:**
    Divide the set of states into multiple partitions:
    1. Make **`partitions`** array, so that **`partitions[i]`** represents the partition of the **i<sup>th</sup>** state.
    2. Assign each state an integer representing the partition it belongs to: 
        * **0** for non-final states.
        * **1, 2, 3, …, F** for final states.

2. **Refine partitions:** 
    1. Iterate through the states and determine their new partitions based on their behaviour; the partitions of their destinations. 
    2. Utilize Hashing for summarizing state behaviours.
    3. update `partitions`.

3. **Repeat Refinement:** 
    Repeat step 2, until no partition changes occur.

4. **Construct the Minimized DFA:** 
    Construct a new DFA using the final partitions as states, such that
    the transitions of the new DFA states are determined by the transitions 
    between the original states within each partition.

### Simplified Pseudo Code

```pseudo
partitions = assignPartitions(dfa) // step 1
partitionCount = F + 1  		// F is the number of distinct final states
While True:
    newPartitions = int[N]	// N is the number of states
    PartitionToState = HashMap() 
    for i = 0 to N:
            digest = string(partitions[i]) + ", "
            for each input c in dfa[i]:
                destination = dfa[i][c]
                digest += c + ", " + string(partitions[destination]) + ", "
            newPartitions[i] = hash(digest)
            PartitionToState[digest] = i
        if size(PartitionToState) == partitionCount:
            break
        partitionCount = size(PartitionToState)
        partitions = newPartitions
construct minimal DFA M 		// step 4
return M
```

### Hashing [^1]

The employed hashing technique is known as **Polynomial Rolling Hashing** or **String Hashing** This method is employed to generate a compact digest from a combination of the characters (**inputs**) and integers (**partitions of destinations**), summarizing the behavior of each state.

$$hash(s) = \sum_{i=0}^{n-1} s[i] * p^i \mod m$$

**m** was set to $10^9 + 7$, while $p$ was set to $256$.

### Overall Performance

Overall time complexity = $O(I * N * K)$

- $I$ is the number of iterations needed till convergence.
- $N$ is the number of states.
- $K$ is the number of inputs.

Overall space complexity = $O(N)$

- $N$ is the number of states.

[^1]: The probability of two distinct strings having the same hash value is very low (**1/m**), but in a **deployment** environment, a cryptographic hash function is recommended for enhanced security.

## Lexical Analyzer

The `LexicalAnalyzer` class is a crucial component of a lexical analysis system designed to tokenize input source code based on a pre-defined deterministic finite automaton (DFA). Lexical analysis is a vital phase in the compilation process, responsible for breaking down the source code into a sequence of meaningful tokens.

### Responsibilities

1. **Tokenization:**
   - The primary purpose is to tokenize input source code by traversing the input DFA, recognizing patterns defined by lexical rules and generating corresponding tokens within the input file.

2. **Error Recovery:**
   - The class incorporates panic mode error recovery by discarding characters and resuming analysis.

3. **Efficient Input Processing:**
   - It processes input source code by reading it in chunks, ensuring memory efficiency and making it well-suited for large files.

### Key Components

1. **Token Class:**
   ```cpp
   class Token {
       std::string lexeme;
       size_t filePos;
       std::string type;
       std::string error;
       int rule_index;
       Priority rule_priority;
   }
   ```
   - `lexeme`: A sequence of characters making the token.
   - `filePos`: Position of the start of the lexeme within the input file.
   - `type`: Name of the corresponding final state in the minimal DFA.
   - `error`: Sequence of characters removed by PMR before this token.
   - `rule_index`: Index of the corresponding lexical rule.
   - `rule_priority`: Priority class of the corresponding lexical rule.


2. **LexicalAnalyzer Class:**
   ```cpp
   class LexicalAnalyzer {
   public:
       LexicalAnalyzer(const std::string& input_path, size_t buffer_size, const std::string& DFA_path);
       Token getNextToken();
   };
   ```

- **Constructor:**
  - Initializes the `LexicalAnalyzer` object with the input source code file,
    buffer size for efficient processing, and the path to the DFA file for defining lexical rules.

- **Members:**
    - `file`: Input file stream.
    - `buffer`: Vector storing a chunk of characters from the input file.
    - `bufferPos`: Current position in the buffer.
    - `bufferSize`: Initial size of the buffer.
    - `lexemePosInFile`: Position of the last character of lexeme within the input file.
    - `isEOF`: Flag indicating the end of the file.
    - `dfa`: Vector of maps representing the DFA transitions.
    - `start_state`: Starting state of the DFA.
    - `final_states`: Map containing information about final states in the DFA. 
  
- **Public Methods:**
  - `getNextToken()`: Retrieves the next token from the input source code based on the input DFA transitions.

### Routines

**getNextToken():**

I. **Initialize local variables:**
- Initialize needed variables to maintain the current `state` in DFA, `lexeme`, `error` and other maximal munch variables.
- Set `state` to the DFA start state.

II. **Loop:**
   1. **Check Buffer:**
        - Check if the current buffer is fully processed.
   2. **Check EOF:**
      - If the end of the file (EOF) is reached:
        - If a maximal munch occurred, **return** the maximal munch token.
        - If lexeme is empty, create and **return** an EOF token.
        - Otherwise, Perform **`panicModeRecovery`** and **continue** to the next iteration.
   3. **Load Next Chunk:**
      - Load the next chunk from the file into the buffer **if needed**.
   4. **Read Next Character input:** `c = buffer[bufferPos]`
   4. **Check Dead End:**
      - **If the `c` is whitespace or not found in the DFA:**
           - If maximal munch hasn't occurred, perform **`panicModeRecovery`**.
           - If maximal munch occurred, **return** the maximal munch token.
   5. **Perform State Update:**
      - update `state` to the next state based on the DFA transition.
      - append the current character to `lexeme`.
      - increment `bufferPos` for the next iteration.
   6. **Check Final State:**
      - **If `state` is final:**
           - update maximal munch variables.

**panicModeRecovery():**

1. **Initialize a new buffering vector *`B`*.**
2. **If `lexeme` is non-empty:**
   - Remove the first character from `lexeme` and append the remaining characters to the beginning of *`B`*.
3. **If `lexeme` is empty:**
   - Insert `buffer[bufferPos+1:]` into *`B`*.
4. **If the removed character is not whitespace:**
   - Append it to `error`.
5. **Reset `bufferPos`, `lexeme`, and `state`.**
6. **Set `buffer` to *`B`***


## Usage

### Lexical Analyzer Generator

It takes one argument which is the path to the rules file. It generates a serialized minimized DFA file (.dat) in the working directory along with intermediate files for debugging purposes.

```bash
./<executable> <path-to-rules-file>
```

### Lexical Analyzer

It takes three arguments: the path to the serialized minimized DFA file, the path to the input source code file, and the path to the output file containing the tokens. It also takes an optional argument `--verbose` to output an additional file containing the tokens with more details.

```bash
./<executable> <path-to-serialized-minimized-DFA-file> <path-to-input-source-code-file> -o <path-to-output-file> [--verbose]
```

## Test Case

### Lexical Analyzer Generator

**Rules File**

```text
letter = [a-z] | [A-Z]
digit = [0-9]
id: letter (letter|digit)*
digits = digit+
{boolean int float}
num: digit+ | digit+ . digits (\L | E digits)
relop: \=\= | != | > | >= | < | <=
assign: =
{ if else while }
[; , \( ) { }]
addop: \+ | -
mulop: \* | /
```

**NFA Graph**

Check it [here](results/generator/nfa_graph.pdf). Make sure you can open PDF files.

**Minimized DFA Graph**

Check it [here](results/generator/min_dfa_graph.pdf). Make sure you can open PDF files.

**Minimized DFA Table**

| State |   Token |     ! |     ( |     ) |     * |     + |     , |     - |     . |     / |     0 |     1 |     2 |     3 |     4 |     5 |     6 |     7 |     8 |     9 |     ; |     < |     = |     > |     A |     B |     C |     D |     E |     F |     G |     H |     I |     J |     K |     L |     M |     N |     O |     P |     Q |     R |     S |     T |     U |     V |     W |     X |     Y |     Z |     a |     b |     c |     d |     e |     f |     g |     h |     i |     j |     k |     l |     m |     n |     o |     p |     q |     r |     s |     t |     u |     v |     w |     x |     y |     z |     { |     } |
| :---: | :-----: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|    0  |         |    37 |    40 |    28 |    43 |    42 |    39 |    42 |     Φ |    43 |    34 |    34 |    34 |    34 |    34 |    34 |    34 |    34 |    34 |    34 |     3 |    38 |    13 |    38 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     1 |     4 |     4 |     5 |     8 |     4 |     4 |     9 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     2 |     4 |     4 |     4 |    24 |    41 |
|    1* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    26 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|    2* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    18 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|    3* |       ; |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|    4* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|    5* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    20 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|    6* |    else |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|    7* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    21 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|    8* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    10 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|    9* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    16 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    25 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   10* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    22 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   11* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    23 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   12* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    33 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   13* |  assign |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    31 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   14* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    11 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   15* |   while |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   16* |      if |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   17* |     num |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    17 |    17 |    17 |    17 |    17 |    17 |    17 |    17 |    17 |    17 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    35 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   18* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     7 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   19* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    14 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   20* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    29 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   21* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    15 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   22* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    12 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   23* | boolean |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   24* |       { |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   25* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    32 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   26* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    27 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   27* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |    19 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   28* |       ) |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   29* |      id |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     6 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   30  |         |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    17 |    17 |    17 |    17 |    17 |    17 |    17 |    17 |    17 |    17 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   31* |   relop |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   32* |     int |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   33* |   float |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |     Φ |     Φ |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     4 |     Φ |     Φ |
|   34* |     num |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    30 |     Φ |    34 |    34 |    34 |    34 |    34 |    34 |    34 |    34 |    34 |    34 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   35  |         |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    36 |    36 |    36 |    36 |    36 |    36 |    36 |    36 |    36 |    36 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   36* |     num |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    36 |    36 |    36 |    36 |    36 |    36 |    36 |    36 |    36 |    36 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   37  |         |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    31 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   38* |   relop |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |    31 |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   39* |       , |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   40* |       ( |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   41* |       } |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   42* |   addop |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |
|   43* |   mulop |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |     Φ |

### Lexical Analyzer

**Input File**

```c
int sum , count , pass , mnt; while (pass !=
10)
{
    pass = pass + 1 ;
}
```

**Output File**

```text
int
id
,
id
,
id
,
id
;
while
(
id
relop
num
)
{
id
assign
id
addop
num
;
}
EOF
```

**Verbose Output**

```text
   Pos:     0 | Type:        int | Lexeme: "int"
   Pos:     4 | Type:         id | Lexeme: "sum"
   Pos:     8 | Type:          , | Lexeme: ","
   Pos:    10 | Type:         id | Lexeme: "count"
   Pos:    16 | Type:          , | Lexeme: ","
   Pos:    18 | Type:         id | Lexeme: "pass"
   Pos:    23 | Type:          , | Lexeme: ","
   Pos:    25 | Type:         id | Lexeme: "mnt"
   Pos:    28 | Type:          ; | Lexeme: ";"
   Pos:    30 | Type:      while | Lexeme: "while"
   Pos:    36 | Type:          ( | Lexeme: "("
   Pos:    37 | Type:         id | Lexeme: "pass"
   Pos:    42 | Type:      relop | Lexeme: "!="
   Pos:    45 | Type:        num | Lexeme: "10"
   Pos:    47 | Type:          ) | Lexeme: ")"
   Pos:    49 | Type:          { | Lexeme: "{"
   Pos:    55 | Type:         id | Lexeme: "pass"
   Pos:    60 | Type:     assign | Lexeme: "="
   Pos:    62 | Type:         id | Lexeme: "pass"
   Pos:    67 | Type:      addop | Lexeme: "+"
   Pos:    69 | Type:        num | Lexeme: "1"
   Pos:    71 | Type:          ; | Lexeme: ";"
   Pos:    73 | Type:          } | Lexeme: "}"
   Pos:    75 | Type:        EOF | Lexeme: ""
```

## Flex Short Tutorial

Flex (fast lexical analyzer generator) is a free and open-source software alternative to lex. It is a computer program that generates lexical analyzers. In this short tutorial, we will show how to use flex to generate a lexical analyzer for a simple language.

### Stages

The process of generating a lexical analyzer using flex consists of three stages:

1. Writing the `.l` file containing the lexical rules and behavior of the lexical analyzer (e.g. what to do when a token is recognized).
2. Compiling the `.l` file using flex to generate a C file.
3. Compiling the C file to generate an executable. This executable is the lexical analyzer and can be used to tokenize input source code.

### Writing the `.l` File

The `.l` file contains the lexical rules and behavior of the lexical analyzer. It is written in a language called `lex`. This file has 3 main sections:

1. **Definitions Section:** This section contains any definitions needed by the lexical analyzer. For example, we can define the following regular expression for an identifier:

    ```lex
    ID [a-zA-Z_][a-zA-Z0-9_]*
    ```

    C Definitions wanted to be copied to the compiled file are written in the form `%{ ... %}`. For example, we can define a variable to hold the value of the current line number as follows:

    ```lex
    %{
        int line_number = 1;
    %}

2. **Rules Section:** This section contains the lexical rules and is enclosed between `%%` symbols. It is written in the form `pattern action`. For example, we can define a rule that matches a digit as follows:

    ```lex
    [0-9] { printf("Digit: %c\n", yytext[0]); }
    ```
    
    Note how the action is written in C and is enclosed between `{ ... }` symbols. Also, note how the variable `yytext` is used to access the matched text.

3. **User Code Section:** This section contains any user code that will be included in the lexical analyzer. It is written directly after the rules section and in C. Usually, it contains the `main` function which is the entry point of the lexical analyzer. Another function that is usually defined in this section is `yywrap` which is called when the end of the input file is reached. To assign a file to the lexical analyzer, we use the `yyin` variable. To start the lexical analyzer, we call the `yylex` function. The following is an example of a user code section:

    ```lex
    int yywrap() {
        return 1;
    }
    int main(int argc, char **argv) {
        yyin = fopen(argv[1], "r");
        yylex();
        return 0;
    }
    ```

### Compiling the `.l` File

To compile the `.l` file, we use the `flex` command as follows:

```bash
flex <path-to-l-file>
```

This will generate a C file with the same name as the `.l` file but with the extension `.yy.c`.

### Compiling the C File

To compile the C file, the method differs based on the operating system and the compiler used. For example, to compile the C file using gcc, we use the following command:

```bash
gcc <path-to-c-file> -o <path-to-output-executable>
```

### Using the Lexical Analyzer

To use the lexical analyzer, we run the executable generated in the previous step and pass the input source code file as an argument assuming that the input source code file is the first argument. In our example, we can use the following command:

```bash
./<path-to-output-executable> <path-to-input-source-code-file>
```
