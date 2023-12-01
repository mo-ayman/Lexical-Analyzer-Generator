cmake_minimum_required(VERSION 3.26)
project(Lexical_Analyzer_Generator)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lexical_Analyzer_Generator main.cpp
        LexicalRules/Node.cpp
        LexicalRules/Node.h
        LexicalRules/RuleTree.h
        LexicalRules/RuleTree.cpp
        LexicalRules/RuleParser.cpp
        LexicalRules/RuleParser.h)
