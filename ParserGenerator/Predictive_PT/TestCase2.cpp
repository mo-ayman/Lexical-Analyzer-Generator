#include <stdexcept>
#include "PPT.cpp"
#include "../Definition/Definition.h"
#include <iostream>
#include <string>

using namespace std;

//int main()
//{
//    Definition* n1=new Definition("S", false);
//    Definition* n2=new Definition("E'",false);
//    Definition* n3=new Definition("C",false);
//    Definition* T1=new Definition("i");
//    Definition* T2=new Definition("t");
//    Definition* T3=new Definition("a");
//    Definition* T4=new Definition("b");
//    Definition* T5=new Definition("e");
//    Definition* T6 =n1->getEpsilon();
//    Definition* T7=new Definition("$");
//
//
//    map<Definition*, vector<vector<Definition*>>>* InputRules = new map<Definition*, vector<vector<Definition*>>>();
//    map<Definition*, vector<pair<int, Definition*>>>* first = new map<Definition*, vector<pair<int, Definition*>>>();
//    map<Definition*, vector<Definition*>>* follow = new map<Definition*, vector<Definition*>>();
//
//
//    InputRules->insert(pair<Definition*, vector<vector<Definition*>>>(n1,{{T1,n3,T2,n1,n2},{T3}}));
//    InputRules->insert(pair<Definition*, vector<vector<Definition*>>>(n2, { {T5,n1},{T6} }));
//    InputRules->insert(pair<Definition*, vector<vector<Definition*>>>(n3, { {T4}}));
//
//
//    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n1,{make_pair(0,T1),make_pair(1,T3)}));
//    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n2,{make_pair(0,T5),make_pair(1,T6)}));
//    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n3,{make_pair(0,T4)}));
//
//
//    follow->insert(pair<Definition*, vector<Definition*>>(n1,{T7,T5}));
//    follow->insert(pair<Definition*, vector<Definition*>>(n2,{T7,T5}));
//    follow->insert(pair<Definition*, vector<Definition*>>(n3,{T2}));
//    //PPT obj(InputRules,first,follow);
//
//     //
//    //cout<<(*follow).size();
//    PPT* obj=new PPT(InputRules,first,follow);
//    //obj.print(obj.get_PPT());
//    (*obj).print((*obj).get_PPT());
//
//    return 0;
//}
