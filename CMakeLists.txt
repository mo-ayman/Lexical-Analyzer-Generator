cmake_minimum_required(VERSION 3.25)
project(Lexical_Analyzer_Generator)

set(CMAKE_CXX_STANDARD 17)

#### Phase 1
add_executable(Lexical_Analyzer_Generator main_1.cpp
        LexicalAnalyzerGenerator/LexicalRules/Node.cpp
        LexicalAnalyzerGenerator/LexicalRules/Node.h
        LexicalAnalyzerGenerator/LexicalRules/RuleTree.h
        LexicalAnalyzerGenerator/LexicalRules/RuleTree.cpp
        LexicalAnalyzerGenerator/LexicalRules/RuleParser.cpp
        LexicalAnalyzerGenerator/LexicalRules/RuleParser.h
        LexicalAnalyzerGenerator/NFA/NFAConstruction.cpp
        LexicalAnalyzerGenerator/NFA/NFAConstruction.h
        LexicalAnalyzerGenerator/DFA/EpsilonNFA.cpp
        LexicalAnalyzerGenerator/DFA/EpsilonNFA.h
        LexicalAnalyzerGenerator/DFA/DFA.cpp
        LexicalAnalyzerGenerator/DFA/DFA.h
        LexicalAnalyzerGenerator/DFA/HelpingMethods.h
        LexicalAnalyzerGenerator/DFA/HelpingMethods.cpp
        LexicalAnalyzerGenerator/DFAMinimizer/DFAMinimizer.h
        LexicalAnalyzerGenerator/DFAMinimizer/DFAMinimizer.cpp
        LexicalAnalyzerGenerator/Visualization/Visualization.cpp
        LexicalAnalyzerGenerator/Visualization/Visualization.h
        LexicalAnalyzerGenerator/DFASerialization/DFASerialization.cpp
        LexicalAnalyzerGenerator/DFASerialization/DFASerialization.h)

add_executable(Lexical_Analyzer main_2.cpp
        LexicalAnalyzerGenerator/DFASerialization/DFASerialization.cpp
        LexicalAnalyzerGenerator/DFASerialization/DFASerialization.h
        LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.cpp
        LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.h
        ParserGenerator/FirstAndFollow/First.h
        ParserGenerator/FirstAndFollow/First.cpp
        ParserGenerator/FirstAndFollow/Follow.h
        ParserGenerator/FirstAndFollow/Follow.cpp
        ParserGenerator/FirstAndFollow/First.h
        ParserGenerator/FirstAndFollow/Follow.h
        ParserGenerator/FirstAndFollow/Follow.cpp
        ParserGenerator/FirstAndFollow/First.cpp
)

#### Phase 2
add_executable(Parser_Generator ParserGenerator/parserGenerator.cpp
        ParserGenerator/Definition/Definition.cpp
        ParserGenerator/Definition/Definition.h
        ParserGenerator/Predictive_PT/PPT.h
        ParserGenerator/Predictive_PT/PPT.cpp
        ParserGenerator/FirstAndFollow/First.h
        ParserGenerator/FirstAndFollow/Follow.h
        ParserGenerator/FirstAndFollow/Follow.cpp
        ParserGenerator/FirstAndFollow/First.cpp
        ParserGenerator/ParsingCFG/ParsingCFG.h
        ParserGenerator/ParsingCFG/ParsingCFG.cpp
        ParserGenerator/ParsingCFG/AdaptCFG.h
        ParserGenerator/ParsingCFG/AdaptCFG.cpp

)

add_executable(Parser ParserGenerator/parsing.cpp
        ParserGenerator/Definition/Definition.cpp
        ParserGenerator/Definition/Definition.h
        LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.cpp
        LexicalAnalyzerGenerator/LexicalAnalyzer/LexicalAnalyzer.h
        LexicalAnalyzerGenerator/DFASerialization/DFASerialization.cpp
        LexicalAnalyzerGenerator/DFASerialization/DFASerialization.h
        ParserGenerator/Parser/ParseTree/ParseTreeNode.cpp
        ParserGenerator/Parser/ParseTree/ParseTreeNode.h
        ParserGenerator/Parser/Parser.cpp
        ParserGenerator/Parser/Parser.h
)
