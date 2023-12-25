#ifndef PPT_H
#define PPT_H

#include <string> 
#include <vector>
#include <map>
#include "../Definition/Definition.h"
using namespace std;


class PPT {
private:
	map<Definition*,map<string,vector<Definition*>>>* PPTable =new map<Definition*,map<string,vector< Definition*>>>();
    map<Definition*, vector<vector<Definition*>>>* InputRulesM;
    map<Definition*, vector<pair<int, Definition*>>>* firstM;
    map<Definition*, vector<Definition*>>* followM;
public:
    PPT(map<Definition*, vector<vector<Definition *>>>* InputRules, map<Definition*, vector<pair<int,Definition*>>>* first, map<Definition*,vector<Definition*>>* follow);
    map<Definition*, map<string, vector<Definition*>>>* get_PPT();
    void fillFirstChunck();
    void fillFollowChunck();
    int checkEpslon(vector<pair<int, Definition*>> mappingFirst);
    void print(map<Definition*, map<string, vector<Definition*>>>* table);
};

#endif
