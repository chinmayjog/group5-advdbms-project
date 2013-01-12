#include <iostream>
#include <cstring>
#include <string>
#include <cstdio>
#include "parser.h"
#include "../../DBEngine/DB.h"
#include "../../cachemgr/src/BufferManager.h"
using namespace std;

extern int BufferManagerInterface();

void executeQuery(DB *curDB, string s){
	string c = s;
	query q = load_query((const char *)c.c_str());
	if(q->error!=1){
		int result;
		if(s!=""){
			if(!strcmp(q->type, "CREATEDB")){
				result = curDB->createDB(q);
			}
			else if(!strcmp(q->type, "USEDB")){
				result = curDB->useDB(q);
			}
			else if(!strcmp(q->type,"DROPDB")){
				result = curDB->dropDB(q);
			}
			else if(!strcmp(q->type,"SHOWDB")){
				result = curDB->showDB(q);
				if(result > 0){
					for(int i = 0; i < q->cntColumns; i++){
						cout<<q->results[i]<<endl;
					}
				}
			}
			else if(!strcmp(q->type,"SHOWTBL")){
				result = curDB->showTables(q);
				if(result > 0){
					for(int i = 0; i < q->cntColumns; i++){
						cout<<q->results[i]<<endl;
					}
				}
			}
			else if(!strcmp(q->type,"CREATETBL")){
				result = curDB->createTable(q);
			}
			else if(!strcmp(q->type,"CREATEIND")){
				//result = curDB->createIndex(q);
			}
			else if(!strcmp(q->type,"DROPIND")){
				//result = curDB->dropIndex(q);
			}
			else if(!strcmp(q->type,"INSERT")){
				result = curDB->insertEntry(q);
			}
			else if(!strcmp(q->type,"DELETE")){
				result = curDB->deleteEntry(q);
			}
			else if(!strcmp(q->type,"UPDATE")){
				//result = curDB->updateEntry(q);
			}
			else if(!strcmp(q->type,"SELECT")){
				result = curDB->selectEntry(q);
				if(result > 0){
					for(int i=0; i<result; i++){
						int len = strlen(q->results[i]);
						cout<<endl<<"Length of result string:"<<len<<endl;
						for(int j=0;j<len;j++){
							cout<<(q->results[i])[j]<<endl;
						}
					}
				}
				/*	
						int cntpipes = 0;
						for(int j=0;j<len;j++){
							int cntcols = 0;
							if(result[i][j]=='|'){
								cntpipes ++;
							}
						}
					}
				}*/
			}
			else if(!strncmp(q->type,"ALTER",5)){
			//	result = curDB->selectEntry(q);
			}	
			cout<<endl<<"Result is: "<<result<<endl;
		}
	}
	delete(q);
}

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
			(*bu).commitCache();
			break;
		}
		else if(s=="$cache"){
			BufferManagerInterface();
		}
		else if(s=="$index"){
			//IndexManagerInterface();
		}
		else if(s=="$bulkinsert"){
			FILE *fp;
			fp = fopen("insert.sql", "r");
			if(fp == NULL){
				cout<<endl<<"File not found"<<endl;
			} 
			else {
				char line[200];
				while(fgets(line, 199, fp)!= NULL){
					s = string(line);
					executeQuery(curDB, s);
				}
			}
		}
		else{
			executeQuery(curDB, s);
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
