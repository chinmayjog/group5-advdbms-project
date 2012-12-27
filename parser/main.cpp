#include <iostream>
#include <cstring>
#include <string>
#include "parser.h"
//#include "DB.h"
using namespace std;

//extern int cacheController();
/*
void traverse(condition *node){
	if(node != NULL){
		if(node->cond!= ANDC && node->cond!= ORC){
			cout<<node->colname<<" "<<node->cond<<" ";
						switch(node->rt){
							case STRING :
								for(int i=0; i<=node->rightcnt; i++)
									cout<<node->rightstr[i]<<", ";
								cout<<"|";
								break;
							case INTEGER :
								for(int i=0; i<=node->rightcnt; i++)
									cout<<node->rightint[i]<<", ";
								cout<<"|";
								break;
							case FLOAT :
								for(int i=0; i<=node->rightcnt; i++)
									cout<<node->rightflt[i]<<", ";
								cout<<"|";
								break;
							}
		}
		else if(node->cond==ANDC){
			cout<<" and ";
		}
		else if(node->cond==ORC){
			cout<<" or ";
		}
		traverse(node->lhs);
		traverse(node->rhs);
	}	
}
*/
int main( int argc, char *argv[] ) {
	string s="";
	do{
		cout<<"\ndb> ";
		std::getline(cin, s);	
		if(s=="exit" || s=="exit;") 
			break;
		else if(s=="$cache"){
			//cacheController();
			s="";
		}
		else {
			string c = s;
			query q = load_query((const char *)c.c_str());
			if(q->error!=1){
				cout<< q->type <<endl;
				cout<< q->dbname << endl;
				cout<< q->index << endl;
				cout<< q->table << endl;
				cout<<"Columns: "<<endl;
				if(!strcmp(q->type, "CREATETBL") || !strcmp(q->type, "CREATEIND"))
				for(int i = q->cntColumns -1; i>= 0; i--)
				{
					cout<<(q->columns[i]).name<<endl;
					cout<<(q->columns[i]).type<<endl;
					cout<<(q->columns[i]).function<<endl;
					cout<<(q->columns[i]).sizeofField1<<endl;
				}
				else if(!strcmp(q->type, "INSERT") || !strcmp(q->type, "UPDATE"))
				for(int i = q->cntColumns -1; i>= 0; i--)
				{
					cout<<(q->ins[i]).colname<<endl;
					cout<<(q->ins[i]).type<<endl;
					cout<<(q->ins[i]).i<<endl;
					cout<<(q->ins[i]).f<<endl;
					cout<<(q->ins[i]).str<<endl;
				}
				if(!strcmp(q->type, "SELECT") || !strcmp(q->type, "DELETE")  || !strcmp(q->type, "UPDATE"))
					//traverse(q->root);
				cout<<endl;
			}
			delete(q);
		}
	}while(1);
    return 0;
}
