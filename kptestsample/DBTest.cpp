// DBTest.cpp

#include "DB.h"

int createDataBase(int dbSize,int pageSize,string dbName)
{
	DB *newdb = new DB();
	int result = newdb->createDB(dbSize,pageSize,dbName);
	cout<<"Database created."<<endl;
	cout<<"Query: "<<result<<" rows affected.";
	delete newdb;
	return result;
}

void DBFunction(int choice)
{
	string dbName;
	int dbSize,pageSize;
	int result;
	// Add time information for the query... Use start time and end time......
	switch(choice)
	{
		case 1: cout<<"Creating the database....."<<endl;
			cout<<"Enter the name of the database to be created: ";
			cin>>dbName;
			cout<<endl;
			cout<<"Do you want to specify the DB file size (default 100MB): ";
			cin>>dbSize;
			cout<<endl;
			cout<<"Do you want to specify the page size (default 2048): ";
			cin>>pageSize;
			result = createDataBase(dbSize,pageSize,dbName);
			break;
		default: cout<<"Unimplemented operation....."<<endl;
			 cout<<"Contact the developer for the operation...."<<endl;
			 break;
	}
}

int main()
{
	int choice;
	do
	{
		cout<<"Welcome to the DB Engine...."<<endl;
		cout<<"Enter 1 to create a DB."<<endl;
		cout<<"Enter 2 to create tables."<<endl;
		cout<<"Enter 3 to use a DB."<<endl;
		cout<<"Enter 4 to show tables."<<endl;
		cout<<"Enter 5 to show databases."<<endl;
		cout<<"Enter 6 to insert an entry."<<endl;
		cout<<"Enter 7 to delete an entry."<<endl;
		cout<<"Enter 8 to execute select query."<<endl;
		cout<<"Enter 9 to update an entry."<<endl;
		cout<<"Enter 10 to execute alter query."<<endl;
		cout<<"Enter 11 to drop column."<<endl;
		cout<<"Enter 12 to drop table."<<endl;
		cout<<"Enter 13 to drop database."<<endl;
		cout<<"Indexing opeations"<<endl;
		cout<<"Enter 14 to create an index."<<endl;
		cout<<"Enter 15 to drop index."<<endl;
		cout<<"Enter 16 to execute operations using index."<<endl;
		cout<<"Enter -1 to exit."<<endl;
		cout<<"Enter your choice: ";
		cin>>choice;
		if((choice < 1 || choice > 16) && choice != -1)
		{
			if(choice != -1)
				cout<<"Invalid operation."<<endl;
				continue;
		}
		DBFunction(choice);
	}while(choice!=-1);
	cout<<endl;
}
