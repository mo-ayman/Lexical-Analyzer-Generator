#include "PPT.h"
#include <stdexcept>
#include "../Definition/Definition.cpp"
#include <iostream>
#include <string>
using namespace std;

/*
 * This is the constructor part it take 3 pointers
 * First pointer point to InputRules as T=>FT'|e T is Definition* and map to vector of FT' and e all as Definition*
 * second, first contains vector of non-terminals also in form of map T=>{(*,1),("+",0)} integer value represent index of production rule
 * Third pointer follow contain follow of all non-terminal in form of T=>{"$",")"} all as pointer (terminal and non-terminals)
 * */

PPT::PPT(const map<Definition *, vector<vector<Definition *>>>& InputRules,
         map<Definition *, vector<pair<int, Definition*>>>& first, map<Definition *, vector<Definition*>>& follow)
{
   this->InputRulesM = InputRules;
   firstM = first;
   followM = follow;
}

/*
 * This function called to return final predictive parsing table as pointer
 * */
map<Definition*, map<string, vector<Definition*>>>* PPT::get_PPT()
{
    fillFirstChunck();
    fillFollowChunck();

	return PPTable;
}
/*
 * This function used to fill first production rules in table
 * First get non-terminal that search for it's first non-terminals
 * second loop over all first and add in PPTable equivalent production rule
 * Finally if  slot is empty append in PPTable the production rule
 * */
void PPT::fillFirstChunck()
{
	for (auto& pairLoop : firstM) {
        Definition* NonTerminal = pairLoop.first;
        for(pair<int,Definition*>& subFirst :pairLoop.second) {
            if((subFirst.second)->getName()=="EPSILON"){continue;}
            // get index of the production rule T=>FT'|e FT' has index 0
            int Ptoduction_Indx = subFirst.first;
            //check if slot in table not contain 2 production rule check LL(1) condition
            map<string, vector<Definition*>> LL1_checkMap = (*PPTable)[NonTerminal];
            auto it = LL1_checkMap.find((subFirst.second)->getName());
            if (it !=LL1_checkMap.end()) {
                throw runtime_error("Not LL(1) grammar");
            }
            else {
                auto& Input_map = (*PPTable)[NonTerminal];
                Input_map.insert(pair<string,vector<Definition*>>((subFirst.second)->getName(),(InputRulesM)[NonTerminal][Ptoduction_Indx]));
            }
        }

	}
}
/*
 * In this function i loop through all follow map
 * in each follow i check if there is epsilon transition in first then add production rule
 * that result to that in table if there is no conflict in table slot
 * if there is no epsilon in first of the non-terminal then place sync in table
 * */
void PPT::fillFollowChunck()
{
    // first for loop to iterate over follow map
	for (auto& pairLoop : followM) {

        //get non-terminal key of follow
        Definition* NonTerminal = pairLoop.first;

        //this input map to
        auto& Input_map = (*PPTable)[NonTerminal];

        //check if first of that non-terminal contain epslon transition
		int getEpslonIndx = checkEpslon((firstM)[NonTerminal]);

        // loop through all follow of given non-terminal and filling Input_map that represent str->production rule
        for(auto& subFollow :pairLoop.second){
            auto it = Input_map.find(subFollow->getName());
            if (getEpslonIndx > -1) {
                if (it != Input_map.end()) {
                    throw runtime_error("Not11 LL(1) grammar");

                } else {
                    Input_map.insert(pair<string, vector<Definition *>>(subFollow->getName(),
                                                                             (InputRulesM)[NonTerminal][getEpslonIndx]));
                }
            } else {
                if (it != Input_map.end()) {
                    throw runtime_error("Not88 LL(1) grammar");

                } else {
                    Input_map.insert(pair<string, vector<Definition *>>(subFollow->getName(), {}));
                }

            }
        }
	}
}
/*
 * This function used to check if there is epslon in first of given non-terminal vector contail
 * All first of that non-terminals
 * */
int PPT::checkEpslon(vector<pair<int, Definition*>>mappingFirst)
{

    for(pair<int,Definition*> subFirst :mappingFirst) {

        string T_name = (subFirst.second)->getName();
        if (T_name=="EPSILON") {
                return subFirst.first;
            }
	}

	return -1;
}

/*
 * This function used to print Predictive parsing table
 * */
void PPT::print(map<Definition *, map<string, vector<Definition *>>> *table) {
    for (const auto& pair: *table) {
        std::cout << pair.first->getName() << " --str(NT)=>P-Rule:  ";
        for (const auto& pairSecond: pair.second)
        {
            std::cout <<pairSecond.first<<"=>" ;
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
    Definition* T6=n1->getEpsilon();
    Definition* T7=new Definition("$");


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
    auto* obj =new PPT(*InputRules,*first,*follow);

    map<Definition*, map<string, vector<Definition*>>>* table=obj->get_PPT();
    obj->print(table);

 return 0;
}