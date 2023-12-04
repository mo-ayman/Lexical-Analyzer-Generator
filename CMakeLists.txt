cmake_minimum_required(VERSION 3.25)
project(Lexical_Analyzer_Generator)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lexical_Analyzer_Generator main.cpp
        LexicalRules/Node.cpp
        LexicalRules/Node.h
        LexicalRules/RuleTree.h
        LexicalRules/RuleTree.cpp
        LexicalRules/RuleParser.cpp
        LexicalRules/RuleParser.h
        NFA/NFAConstruction.cpp
        NFA/NFAConstruction.h
        DFA/EpsilonNFA.cpp
        DFA/EpsilonNFA.h
        DFA/DFA.cpp
        DFA/DFA.h
        DFA/HelpingMethods.h
        DFA/HelpingMethods.cpp
        DFAMinimizer/DFAMinimizer.h
        DFAMinimizer/DFAMinimizer.cpp
        DFASerialization/DFASerialization.cpp
        DFASerialization/DFASerialization.h
        LexicalAnalyzer/LexicalAnalyzer.cpp
        Visualization/Visualization.cpp
        Visualization/Visualization.h
        LexicalAnalyzer/LexicalAnalyzer.h)
