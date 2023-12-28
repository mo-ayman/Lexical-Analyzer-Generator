# Parser Generator

## Overview

This project is a non-recursive predictive parser generator written in C++. It compiles a set of parsing rules defined in a text file into a parsing table that can be used later by a parser (included here as a separate executable) to parse a given input.

Later, the parser reads the serialized minimized DFA and the serialized parsing table files and uses them to parse the input into a parse tree.

## Rules File

The rules file is a text file that contains the parsing rules. Each rule starts with `#` symbol in a new line. A rule may be written in multiple lines. The syntax of the rules file follows the following rules:

- Rules are in the form # LHS = RHS, where LHS is the name of the rule and RHS is the syntax defined by the rule. LHS can only contain alphanumeric characters and underscores. RHS can also contain references to other rules.
- Alternatives are separated by `|` symbol.
- A backslash followed by `L` is used to represent a void character.
- Tokens are referenced by their names and are enclosed in single quotes.
- References to other rules are kept without any enclosing characters.

The following is an example of a valid rules file:

```text
# METHOD_BODY = STATEMENT_LIST
# STATEMENT_LIST = STATEMENT | STATEMENT_LIST STATEMENT
# STATEMENT = DECLARATION
    | IF
    | WHILE
    | ASSIGNMENT
# DECLARATION = PRIMITIVE_TYPE 'id' ';'
# PRIMITIVE_TYPE = 'int' | 'float'
# IF = 'if' '(' EXPRESSION ')' '{' STATEMENT '}' 'else' '{' STATEMENT '}'
# WHILE = 'while' '(' EXPRESSION ')' '{' STATEMENT '}'
# ASSIGNMENT = 'id' 'assign' EXPRESSION ';'
# EXPRESSION = SIMPLE_EXPRESSION
    | SIMPLE_EXPRESSION 'relop' SIMPLE_EXPRESSION
# SIMPLE_EXPRESSION = TERM | SIGN TERM | SIMPLE_EXPRESSION 'addop' TERM
# TERM = FACTOR | TERM 'mulop' FACTOR
# FACTOR = 'id' | 'num' | '(' EXPRESSION ')'
# SIGN = '+' | '-'
```

------------

## Rules Parser

Our implementation of the rules parser is hard-wired and well optimized for the syntax of the rules file. Parsing is done in a single pass. Thus, complexity of the parser is linear in the size of the input. Each rule is parsed into a 2D array where the first level represents the alternatives. If the rules file is not valid, the parser will throw an exception with a detailed error message.

------------

## Left Recursion-Factoring Elimination 

The `AdaptCFG` class is designed to adapt context-free grammars (CFGs) by eliminating left recursion and left factoring. The adaptation process aims to transform a CFG into an equivalent CFG that is more suitable for certain parsing algorithms. The implementation utilizes a set of rules and definitions to perform the necessary transformations.

### Class Structure

The `AdaptCFG` class has the following structure:

### Public Interface

1. **Constructor**: 
   ```cpp
   AdaptCFG(const std::map<Definition*, std::vector<std::vector<Definition*>>> &rules);
   ```
   - Initializes the `AdaptCFG` object with the input set of rules.

2. **`adaptCFG` Method**:
   ```cpp
   void adaptCFG();
   ```
   - Executes the adaptation process, which involves eliminating left recursion, achieving consistency, eliminating left factoring, and achieving consistency again.

3. **`getNewRules` Method**:
   ```cpp
   [[nodiscard]] std::map<Definition*, std::vector<std::vector<Definition*>>> getNewRules() const;
   ```
   - Retrieves the adapted rules after the adaptation process.

### Private Methods

1. **`eliminateLeftRecursion` Method**:
   ```cpp
   void eliminateLeftRecursion();
   ```
   - Implements the elimination of left recursion in the CFG.

2. **`consistencyAchieve` Method**:
   ```cpp
   void consistencyAchieve();
   ```
   - Ensures consistency in the representation of definitions within the CFG.

3. **`eliminateLeftFactoring` Method**:
   ```cpp
   void eliminateLeftFactoring();
   ```
   - Implements the elimination of left factoring in the CFG.

### Private Data Members

1. **`rules` Member**:
   ```cpp
   std::map<Definition*, std::vector<std::vector<Definition*>>> rules;
   ```
   - Stores the original rules provided during the initialization.

2. **`newRules` Member**:
   ```cpp
   std::map<Definition*, std::vector<std::vector<Definition*>>> newRules;
   ```
   - Stores the adapted rules after the adaptation process.

### Left Recursion Elimination

The `eliminateLeftRecursion` method identifies left-recursive rules and applies transformations to eliminate the left recursion. The algorithm introduces new definitions to represent the recursive part, achieving a CFG without left recursion.

### Left Factoring Elimination

The `eliminateLeftFactoring` method identifies left-factored rules and introduces new definitions to eliminate left factoring. The resulting CFG is more suitable for predictive parsing algorithms.

### Consistency Achieve

The `consistencyAchieve` method ensures consistency in the representation of definitions within the CFG. It associates each definition with a unique instance to facilitate further processing.

----------

## First Construction


The `First` class is responsible for computing the First sets for non-terminals in a context-free grammar (CFG). The First set of a non-terminal is the set of terminals that can appear as the first symbol in any string derived from that non-terminal. This information is crucial for constructing predictive parsing tables.

### Class Structure

### Public Interface

1. **Constructor**:
   ```cpp
   First(const std::map<Definition*, std::vector<std::vector<Definition*>>> &rules);
   ```
   - Initializes the `First` object with the set of rules provided.

2. **`constructFirst` Method**:
   ```cpp
   void constructFirst();
   ```
   - Computes the First sets for all non-terminals.

3. **`print` Method**:
   ```cpp
   void print() const;
   ```
   - Prints the computed First sets for each non-terminal.

4. **`getFirst` Method**:
   ```cpp
   std::vector<std::pair<int, Definition*>> getFirst(Definition* definition);
   ```
   - Returns the computed First set for a specific non-terminal.

5. **`getFirst` Method**:
   ```cpp
   std::map<Definition*, std::vector<std::pair<int, Definition*>>> getFirst();
   ```
   - Returns all computed First sets.

### Private Methods

1. **`getFirst` Method**:
   ```cpp
   std::vector<std::pair<int, Definition*>> getFirst(Definition* definition);
   ```
   - Recursive utility method to compute the First set for a given non-terminal.

### Private Data Members

1. **`rules` Member**:
   ```cpp
   std::map<Definition*, std::vector<std::vector<Definition*>>> rules;
   ```
   - Stores the set of rules provided during initialization.

2. **`isFirstCalculated` Member**:
   ```cpp
   std::unordered_set<Definition*> isFirstCalculated;
   ```
   - Tracks non-terminals for which the First set has already been computed.

3. **`first` Member**:
   ```cpp
   std::map<Definition*, std::vector<std::pair<int, Definition*>>> first;
   ```
   - Stores the computed First sets for each non-terminal.

### First Set Computation

The `constructFirst` method iterates over all rules and calls the `getFirst` method for each non-terminal to compute its First set.

### Recursive First Set Computation

The `getFirst` method is a recursive utility function that computes the First set for a given non-terminal. It checks for terminal symbols and recursively processes rules to compute the First set.

### Printing First Sets

The `print` method prints the computed First sets for each non-terminal in a readable format.

## Follow Construction

The `Follow` class is responsible for computing the Follow sets for non-terminals in a context-free grammar (CFG). The Follow set of a non-terminal is the set of terminals that can appear immediately to the right of that non-terminal in any sentential form. The computed Follow sets are crucial for constructing predictive parsing tables.

### Class Structure

### Public Interface

1. **Constructor**:
   ```cpp
   Follow(const std::map<Definition *, std::vector<std::vector<Definition *>>> &rules,
          First* first, Definition* startSymbol);
   ```
   - Initializes the `Follow` object with the set of rules, the `First` sets, and the start symbol provided.

2. **`constructFollow` Method**:
   ```cpp
   void constructFollow();
   ```
   - Computes the Follow sets for all non-terminals.

3. **`print` Method**:
   ```cpp
   void print() const;
   ```
   - Prints the computed Follow sets for each non-terminal.

4. **`getFollow` Method**:
   ```cpp
   std::vector<Definition *> getFollow(Definition *definition);
   ```
   - Returns the computed Follow set for a specific non-terminal.

5. **`getFollow` Method**:
   ```cpp
   std::map<Definition *, std::vector<Definition *>> getFollow();
   ```
   - Returns all computed Follow sets.

### Private Methods

1. **`getFollow` Method**:
   ```cpp
   std::vector<Definition *> getFollow(Definition *definition);
   ```
   - Recursive utility method to compute the Follow set for a given non-terminal.

### Private Data Members

1. **`rules` Member**:
   ```cpp
   std::map<Definition *, std::vector<std::vector<Definition *>>> rules;
   ```
   - Stores the set of rules provided during initialization.

2. **`first` Member**:
   ```cpp
   First* first{};
   ```
   - Pointer to the `First` object, which is used to access the computed First sets.

3. **`follow` Member**:
   ```cpp
   std::map<Definition *, std::vector<Definition *>> follow;
   ```
   - Stores the computed Follow sets for each non-terminal.

4. **`isFollowCalculated` Member**:
   ```cpp
   std::unordered_set<Definition *> isFollowCalculated;
   ```
   - Tracks non-terminals for which the Follow set has already been computed.

5. **`inStack` Member**:
   ```cpp
   std::unordered_set<Definition *> inStack;
   ```
   - Tracks non-terminals currently in the stack to handle recursive calls.

6. **`startSymbol` Member**:
   ```cpp
   Definition* startSymbol;
   ```
   - Pointer to the start symbol of the CFG.

### Follow Set Computation

The `constructFollow` method iterates over all rules and calls the `getFollow` method for each non-terminal to compute its Follow set.

### Recursive Follow Set Computation

The `getFollow` method is a recursive utility function that computes the Follow set for a given non-terminal. It checks for terminal symbols and recursively processes rules to compute the Follow set.

### Printing Follow Sets

The `print` method prints the computed Follow sets for each non-terminal in a readable format.

----------

## Predictive Parsing Table Generator
- In this part I take the input Rules, first and follow sets in format of `map<Definition*, vector<vector<Definition*>>>` as set of map 
T=>FT'|e T is Definition* of nonterminal and map to vector of <FT' and epslon> all as Definition*.
- Then first set in a format `map<Definition*, vector<pair<int, Definition*>>>` first contains vector of non-terminals also in form of map T=>{(*,1),("+",0)} integer value represent index of
 production rule in Rules map
- Also, follow in a fromat `map<Definition*, vector<Definition*>>` 
contains follow of all non-terminal in form of T=>{"$",")"} all as pointer (terminal and non-terminals) that i used for epsilon production or sync state.
- Predictive parsing table is in format `unordered_map<Definition*, unordered_map<string, vector<Definition*>>>` as unordered map containing each 
non terminal as Definition* and it's corresponding map that map each non-terminal input with production rules `unordered_map<string, vector<Definition*>`
string here represents terminal name.

<!-- Table showing example of predictive parsing table -->
| Non terminal  | terminal1  | terminal2| $|
| -------- | -------- | --- | --------- |
| **E**| E=>ET'| E=>E/D|empty vector as (sync) |
| **F**| F=>(E)F'| F=>-ED | F=>epsilon|
| **D**|| D=>*EF| D=>epsilon| D=>epsilon|
.


**Some Methods Definitions in Predictive Parsing Table Generator:**

1. `PPT(const map<Definition*, vector<vector<Definition *>>>& InputRules, map<Definition*, vector<pair<int,Definition*>>>& first, map<Definition*,vector<Definition*>>& follow);` 
    - This is the constructor part it takes 3 pointers.
    - First pointer point to InputRules as T=>FT'|e T is Definition* and map to vector of FT' and e all as Definition*.
    - second, first_set contains vector of non-terminals also in form of map T=>{(*,1),("+",0)} integer value represents index of the production rule.
    - Third, pointer follow_set contains follow of all non-terminal in form of T=>{"$",")"} all as pointer Definition* (terminal and non-terminals).
 .
2. `void fillFirstChunck()`
    - This function used to fill first production rules in PPTable(Predictive Parsing Table)
    - First get non-terminal that search for it's first non-terminals
    - second loop over all first and add in PPTable equivalent production rule
    - Finally if slot in table is empty append in PPTable the production rule else give error Not LL(1) grammar.
3. `void fillFollowChunck()`
    - In this function i loop through all follow map
    - In each follow i check if there is epsilon transition in first then add production rule That result to that in table if there is no conflict in table slot
    - If there is no epsilon in first of the non-terminal then place sync in table
4. `checkEpslon(vector<pair<int, Definition*>> mappingFirst)`
    - This function used to check if there is epslon in first of given non-terminal vector contains all first of that non-terminals.
5. `print(unordered_map<Definition*, unordered_map<string, vector<Definition*>>>* table)`
    - This function used to print Predictive parsing table.
6. `unordered_map<Definition*, unordered_map<string, vector<Definition*>>>* PPT::computePPT()` 
    - This method first call fillFirstChunck() method to fill first production rules in PPtable.
    - Then call fillFollowChunck() method to fill follow production (epslon or sync) i represet sync state as empty vector in PPTable map.
    - Finally, return unordered_map PPTable.

--------

## Parser

The parser module performs the transformation of the sequence of tokens, generated by the lexical analyzer, into a parse tree. This module is responsible for enforcing the syntactic rules of the context free grammer through the use of a predictive parsing table and a stack-based parsing algorithm.

### Classes

#### 1. **Parser**
```cpp
class Parser {
unordered_map<const Definition*, unordered_map<string, vector<const Definition*>>> table;
public:
    Parser(const unordered_map<const Definition*, unordered_map<string, vector<const Definition*>>>& parsingTable);
    std::shared_ptr<const ParseTreeNode> parse(LexicalAnalyzer& lexicalAnalyzer, const Definition* startSymbol);
};
```

#### 2. **ParseTreeNode**
```cpp
class ParseTreeNode {
public:
    const Definition* content;
    vector<std::shared_ptr< const ParseTreeNode>> children;

    bool getIsTerminal();
    explicit ParseTreeNode(const Definition* definition);
    void insertLeft(std::shared_ptr<const ParseTreeNode> child);
    void insertRight(std::shared_ptr<const ParseTreeNode> child);
    void plotGraph(const std::string& outputPath) const;
    void printLeftmostDerivationSteps(std::ostream& os) const;
};
```

### Parsing Algorithm

The parsing algorithm used by the `Parser` class is a stack-based approach that employs the predictive parsing table obtained in the previous stage for decision making. Here is a simplified pseudocode representation:
```cpp
initialize empty parse tree T
initialize root node R with the start symbol
insert R into T
initialize stack with S
initialize currentToken
while stack is not empty:
    stackTopNode = top of stack
    if stackTopNode matches currentToken:
        matchToken()
    else if stackTopNode is a terminal:
        Perform Error Recovery
    else:
        row = stackTopNode.content
        column = currentToken.type or eofString
        if table[row][column] exists:
            production = table[row][column]
            insert production into T
            for element in reverse(production):
                push element into stack
        else:
            Perform Error Recovery
            
if excess tokens or unexpected EOF:
    handleExcessTokensOrEOFError()
    
return R
```

### Error Recovery

The parsing module includes error handling mechanisms to recover from unexpected situations. These mechanisms involve discarding or inserting tokens, as well as sync and error productions.
1. Inserting an input token:
   - If the current token does not match the top of the stack, and the stack top is a terminal, an error occurs. In this case, the stack top is inserted onto the input, the token is matched, and parsing continues.


3. Discarding an input token:
   - If there is no valid production for the current non-terminal and token, an error is detected. In this case, the current token is discarded from the input, and parsing continues.


5. Parsing Table Sync Cell:
   - If the production for the current non-terminal and token is empty, a sync rule is applied. The stack top (non-terminal) is popped, and parsing continues.


## Usage

### Parser Generator

It takes two arguments which are the path to the rules file and the path to the parsing table output file. The parsing table is serialized into a text file.

```bash
./<executable> <path-to-rules-file> -o <path-to-output-file>
```

### Parser

It takes three arguments: the path to the serialized minimized DFA file, the path to the serialized parsing table file and the path to the input source code file.

```bash
./<executable> <path-to-serialized-minimized-DFA-file> <path-to-serialized-parsing-table-file> <path-to-input-source-code-file>
```

## Test Case

### Lexical Analyzer Generator

**Rules File**

```text
# METHOD_BODY = STATEMENT_LIST
# STATEMENT_LIST = STATEMENT | STATEMENT_LIST STATEMENT
# STATEMENT = DECLARATION
    | IF
    | WHILE
    | ASSIGNMENT
# DECLARATION = PRIMITIVE_TYPE 'id' ';'
# PRIMITIVE_TYPE = 'int' | 'float'
# IF = 'if' '(' EXPRESSION ')' '{' STATEMENT '}' 'else' '{' STATEMENT '}'
# WHILE = 'while' '(' EXPRESSION ')' '{' STATEMENT '}'
# ASSIGNMENT = 'id' 'assign' EXPRESSION ';'
# EXPRESSION = SIMPLE_EXPRESSION
    | SIMPLE_EXPRESSION 'relop' SIMPLE_EXPRESSION
# SIMPLE_EXPRESSION = TERM | SIGN TERM | SIMPLE_EXPRESSION 'addop' TERM
# TERM = FACTOR | TERM 'mulop' FACTOR
# FACTOR = 'id' | 'num' | '(' EXPRESSION ')'
# SIGN = '+' | '-'
```

**LL(1) Rules**

```text
STATEMENT_LIST -> STATEMENT STATEMENT_LIST'
METHOD_BODY -> STATEMENT_LIST
SIMPLE_EXPRESSION' -> 'EPSILON' | 'addop' TERM SIMPLE_EXPRESSION'
FACTOR -> '(' EXPRESSION ')' | 'id' | 'num'
TERM -> FACTOR TERM'
STATEMENT_LIST' -> 'EPSILON' | STATEMENT STATEMENT_LIST'
TERM' -> 'EPSILON' | 'mulop' FACTOR TERM'
SIGN -> '+' | '-'
EXPRESSION' -> 'EPSILON' | 'relop' SIMPLE_EXPRESSION
STATEMENT -> ASSIGNMENT | DECLARATION | IF | WHILE
DECLARATION -> PRIMITIVE_TYPE 'id' ';'
WHILE -> 'while' '(' EXPRESSION ')' '{' STATEMENT '}'
IF -> 'if' '(' EXPRESSION ')' '{' STATEMENT '}' 'else' '{' STATEMENT '}'
ASSIGNMENT -> 'id' 'assign' EXPRESSION ';'
PRIMITIVE_TYPE -> 'float' | 'int'
SIMPLE_EXPRESSION -> SIGN TERM SIMPLE_EXPRESSION' | TERM SIMPLE_EXPRESSION'
EXPRESSION -> SIMPLE_EXPRESSION EXPRESSION'
```

**First Sets**

```text
STATEMENT_LIST -> 'id' 'float' 'int' 'if' 'while'
METHOD_BODY -> 'id' 'float' 'int' 'if' 'while'
SIMPLE_EXPRESSION' -> 'EPSILON' 'addop'
FACTOR -> '(' 'id' 'num'
TERM -> '(' 'id' 'num'
STATEMENT_LIST' -> 'EPSILON' 'id' 'float' 'int' 'if' 'while'
TERM' -> 'EPSILON' 'mulop'
SIGN -> '+' '-'
EXPRESSION' -> 'EPSILON' 'relop'
STATEMENT -> 'id' 'float' 'int' 'if' 'while'
DECLARATION -> 'float' 'int'
WHILE -> 'while'
IF -> 'if'
ASSIGNMENT -> 'id'
PRIMITIVE_TYPE -> 'float' 'int'
SIMPLE_EXPRESSION -> '+' '-' '(' 'id' 'num'
EXPRESSION -> '+' '-' '(' 'id' 'num'
```

**Follow Sets**

```text
STATEMENT_LIST : '$'
METHOD_BODY : '$'
SIMPLE_EXPRESSION' : ')' ';' 'relop'
FACTOR : 'mulop' 'addop' ')' ';' 'relop'
TERM : 'addop' ')' ';' 'relop'
STATEMENT_LIST' : '$'
TERM' : 'addop' ')' ';' 'relop'
SIGN : '(' 'id' 'num'
EXPRESSION' : ')' ';'
STATEMENT : 'id' 'float' 'int' 'if' 'while' '$' '}'
DECLARATION : 'id' 'float' 'int' 'if' 'while' '$' '}'
WHILE : 'id' 'float' 'int' 'if' 'while' '$' '}'
IF : 'id' 'float' 'int' 'if' 'while' '$' '}'
ASSIGNMENT : 'id' 'float' 'int' 'if' 'while' '$' '}'
PRIMITIVE_TYPE : 'id'
SIMPLE_EXPRESSION : ')' ';' 'relop'
EXPRESSION : ')' ';'
```

### Parser

**Input File**

```c
int x;
x=5;
if(x>2){x=0;}
```

**Left Most Derivation**

```text
METHOD_BODY 
STATEMENT_LIST
STATEMENT STATEMENT_LIST'
DECLARATION STATEMENT_LIST'
PRIMITIVE_TYPE id ; STATEMENT_LIST'
int id ; STATEMENT_LIST'
int id ; STATEMENT STATEMENT_LIST'
int id ; ASSIGNMENT STATEMENT_LIST'
int id ; id assign EXPRESSION ; STATEMENT_LIST'
int id ; id assign SIMPLE_EXPRESSION EXPRESSION' ; STATEMENT_LIST'
int id ; id assign TERM SIMPLE_EXPRESSION' EXPRESSION' ; STATEMENT_LIST'
int id ; id assign FACTOR TERM' SIMPLE_EXPRESSION' EXPRESSION' ; STATEMENT_LIST'
int id ; id assign num TERM' SIMPLE_EXPRESSION' EXPRESSION' ; STATEMENT_LIST'
int id ; id assign num SIMPLE_EXPRESSION' EXPRESSION' ; STATEMENT_LIST'
int id ; id assign num EXPRESSION' ; STATEMENT_LIST'
int id ; id assign num ; STATEMENT_LIST'
int id ; id assign num ; STATEMENT STATEMENT_LIST'
int id ; id assign num ; IF STATEMENT_LIST'
int id ; id assign num ; if ( EXPRESSION ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( SIMPLE_EXPRESSION EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( TERM SIMPLE_EXPRESSION' EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( FACTOR TERM' SIMPLE_EXPRESSION' EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id TERM' SIMPLE_EXPRESSION' EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id SIMPLE_EXPRESSION' EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop SIMPLE_EXPRESSION ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop TERM SIMPLE_EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop FACTOR TERM' SIMPLE_EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num TERM' SIMPLE_EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num SIMPLE_EXPRESSION' ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { STATEMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { ASSIGNMENT } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign EXPRESSION ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign SIMPLE_EXPRESSION EXPRESSION' ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign TERM SIMPLE_EXPRESSION' EXPRESSION' ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign FACTOR TERM' SIMPLE_EXPRESSION' EXPRESSION' ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign num TERM' SIMPLE_EXPRESSION' EXPRESSION' ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign num SIMPLE_EXPRESSION' EXPRESSION' ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign num EXPRESSION' ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign num ; } else { STATEMENT } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign num ; } else { } STATEMENT_LIST'
int id ; id assign num ; if ( id relop num ) { id assign num ; } else { }
```

**Parse Tree**

Check it [here](Results/parser/parse_tree_graph.pdf). Make sure you can open PDF files.


