#include "PPT.h"
#include <stdexcept>
#include "../Definition/Definition.cpp"
#include <iostream>
#include <string>
using namespace std;



PPT::PPT(map<Definition *, vector<vector<Definition *>>>* InputRules,
         map<Definition *, vector<pair<int, Definition*>>>* first, map<Definition *, vector<Definition*>>* follow)
{
   InputRulesM = InputRules;
   firstM = first;
   followM = follow;
}


map<Definition*, map<Definition*, vector<Definition*>>>* PPT::get_PPT()
{
    //cout<<"gg"<<(*InputRules).size()<<endl;
    fillFirstChunck();
    fillFollowChunck();

	return PPTable;
}
void PPT::fillFirstChunck()
{
    //cout<<"ffff"<<(*firstM).size()<<endl;
	for (auto& pairLoop : *firstM) {

        Definition* NonTerminal = pairLoop.first;
		//map<Definition*, vector<Definition*>> Input_map; //=new  map<Definition*, vector<Definition*>>() ;
        for(pair<int,Definition*>& subFirst :pairLoop.second) {
            if((subFirst.second)->getName()=="w"){continue;}
            int Ptoduction_Indx = subFirst.first;
            map<Definition*, vector<Definition*>> LL1_checkMap = (*PPTable)[NonTerminal];
            auto it = ((*PPTable)[NonTerminal]).find((subFirst.second));
            if (it !=LL1_checkMap.end()) {
                auto& Input_map = (*PPTable)[NonTerminal];
                Input_map.insert(pair<Definition*,vector<Definition*>>(subFirst.second,(*InputRulesM)[NonTerminal][Ptoduction_Indx]));
            }
            else {
                throw runtime_error("Not LL(1) grammer");
            }
        }

	}
}

void PPT::fillFollowChunck()
{
    // first for loop to iterate over follow map
	for (auto& pairLoop : *followM) {
        cout<<"uuuUUU "<<(*followM).size()<<endl;
        //get non-terminal key of follow
        Definition* NonTerminal = pairLoop.first;

        //this input map to
        auto& Input_map = (*PPTable)[NonTerminal];

        //check if first of that non-terminal contain epslon transition
		int getEpslonIndx = checkEpslon((*firstM)[NonTerminal]);

        // loop through all follow of given non-terminal and filling Input_map that represent str->production rule
        for(auto& subFollow :pairLoop.second){
            auto it = Input_map.find(subFollow);
            if (getEpslonIndx > -1) {
                if (it != Input_map.end()) {
                    throw runtime_error("Not11 LL(1) grammar");

                } else {
                    Input_map.insert(pair<Definition*, vector<Definition *>>(subFollow,
                                                                             (*InputRulesM)[NonTerminal][getEpslonIndx]));
                }
            } else {
                if (it != Input_map.end()) {
                    throw runtime_error("Not88 LL(1) grammar");

                } else {
                    Input_map.insert(pair<Definition*, vector<Definition *>>(subFollow, {}));
                }

            }
        }
	}
}

int PPT::checkEpslon(vector<pair<int, Definition*>>mappingFirst)
{
    //cout<<mappingFirst.size()<<"  iiii"<<endl;
    for(pair<int,Definition*> subFirst :mappingFirst) {

        string Tname = (subFirst.second)->getName();
        cout<<Tname<<endl;
        if (Tname=="w") {
                cout<<"armt"<<endl;
                return subFirst.first;
            }
	}
    cout<<mappingFirst.size()<<"  iiii"<<endl<<endl;
	return -1;
}


void PPT::print(map<Definition *, map<Definition *, vector<Definition *>>> *table) {
    for (const auto& pair: *table) {
        std::cout << ": " << pair.first->getName() << " ->str: ";
        for (const auto& pairSecond: pair.second)
        {
            std::cout <<pairSecond.first->getName() <<"=>" ;
            for (const auto& pairVect: pairSecond.second){
                cout << pairVect->getName() ;
            }
            cout<<"  ";
        }

        std::cout << std::endl;
    }
}

int main() {
    Definition* n1=new Definition("E", false);
    Definition* n2=new Definition("E'",false);
    Definition* n3=new Definition("T",false);
    Definition* n4=new Definition( "T'",false);
    Definition* n5=new Definition("F",false);
    Definition* T1=new Definition("+");
    Definition* T2=new Definition("*");
    Definition* T3=new Definition("(");
    Definition* T4=new Definition(")");
    Definition* T5=new Definition("id");
    Definition* T6=new Definition("w");
    Definition* T7=new Definition("$");

    cout<<T6->getName()<<endl;

	map<Definition*, vector<vector<Definition*>>>* InputRules = new map<Definition*, vector<vector<Definition*>>>();
	map<Definition*, vector<pair<int, Definition*>>>* first = new map<Definition*, vector<pair<int, Definition*>>>();
	map<Definition*, vector<Definition*>>* follow = new map<Definition*, vector<Definition*>>();

    vector<vector<Definition*>> v1 ={ initializer_list<Definition*>{n3,n2}};
	InputRules->insert(make_pair(n1, v1));
	InputRules->insert(pair<Definition*, vector<vector<Definition*>>>(n2, { {T1,n3,n2},{T6} }));
	InputRules->insert(pair<Definition*, vector<vector<Definition*>>>(n3, { {n5,n4}}));
	InputRules->insert(pair<Definition*, vector<vector<Definition*>>>(n4, { {T2,n5,n4},{T6}}));
	InputRules->insert(pair<Definition*, vector<vector<Definition*>>>(n5, { {T3,n1,T4},{T5}}));

    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n1,{make_pair(0,T3),make_pair(0,T5)}));
    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n2,{make_pair(0,T1),make_pair(1,T6)}));
    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n3,{make_pair(0,T3),make_pair(0,T5)}));
    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n4,{make_pair(0,T2),make_pair(1,T6)}));
    first->insert(pair<Definition*, vector<pair<int, Definition*>>>(n5,{make_pair(0,T3),make_pair(1,T5)}));

    follow->insert(pair<Definition*, vector<Definition*>>(n1,{T7,T4}));
    follow->insert(pair<Definition*, vector<Definition*>>(n2,{T7,T4}));
    follow->insert(pair<Definition*, vector<Definition*>>(n3,{T1,T7,T4}));
    follow->insert(pair<Definition*, vector<Definition*>>(n4,{T7,T1,T4}));
    follow->insert(pair<Definition*, vector<Definition*>>(n5,{T7,T1,T2,T4}));

    cout<<(*follow).size();
    PPT obj(InputRules,first,follow);
    map<Definition*, map<Definition*, vector<Definition*>>>* table=obj.get_PPT();
    obj.print(table);
//    for (const auto& pair: *table) {
//        std::cout << ": " << pair.first->getName() << " ->str: ";
//        for (const auto& pairSecond: pair.second)
//        {
//            std::cout <<pairSecond.first->getName() << ":Stat:" ;
//            for (const auto& pairVect: pairSecond.second){
//                cout << pairVect->getName() << ", " << std::endl;
//            }
//        }
//
//        std::cout << std::endl;
//    }
 return 0;
}