#include <iostream>
#include <cstring>
#include <string>
#include "parser.h"
#include "../../DBEngine/DB.h"
#include "../../cachemgr/src/BufferManager.h"
using namespace std;

extern int BufferManagerInterface();

int main( int argc, char *argv[] ) {
	BufferManager *bu = BufferManager::getBufferManager();
	(*bu).setPageSize(2048);
	(*bu).initializeCache(2048); 
	string s="";
	DB *curDB = new DB();
	do{
		cout<<"\ndb> ";
		std::getline(cin, s);	
		if(s=="exit" || s=="exit;") {
			break;
		}
		else if(s=="$cache"){
			BufferManagerInterface();
		}
		else if(s=="$index"){
			//IndexManagerInterface();
		}
		else {
			string c = s;
			query q = load_query((const char *)c.c_str());
			if(q->error!=1){
				int result;
				if(s!=""){
					if(!strcmp(q->type, "CREATEDB")){
						result = curDB->createDB(q);
					}
					else if(!strcmp(q->type, "USEDB")){
						result = curDB->createDB(q);
					}
					else if(!strcmp(q->type,"DROPDB")){
						result = curDB->dropDB(q);
					}
					else if(!strcmp(q->type,"SHOWDB")){
						result = curDB->showDB(q);
					}
					else if(!strcmp(q->type,"SHOWTBL")){
						//result = curDB->showTables(q);
					}
					else if(!strcmp(q->type,"CREATETBL")){
						//result = curDB->createTable(q);
					}
					else if(!strcmp(q->type,"CREATEIND")){
						//result = curDB->createIndex(q);
					}
					else if(!strcmp(q->type,"DROPIND")){
						//result = curDB->dropIndex(q);
					}
					else if(!strcmp(q->type,"INSERT")){
						//result = curDB->insertEntry(q);
					}
					else if(!strcmp(q->type,"DELETE")){
						//result = curDB->deleteEntry(q);
					}
					else if(!strcmp(q->type,"UPDATE")){
						//result = curDB->updateEntry(q);
					}
					else if(!strcmp(q->type,"SELECT")){
						//result = curDB->selectEntry(q);
					}
					else if(!strncmp(q->type,"ALTER",5)){
					//	result = curDB->selectEntry(q);
					}	
					cout<<endl<<"Result is: "<<result<<endl;
				}
			}
			delete(q);
		}
	}while(1);
	delete(curDB);
    return 0;
}


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
