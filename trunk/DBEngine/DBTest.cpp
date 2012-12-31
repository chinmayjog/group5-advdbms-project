// DBTest.cpp

#include "DB.h"

int createDataBase(int dbSize,int pageSize,string dbName)
{
	DB *newdb = new DB();
	int result = newdb->createDB(dbSize,pageSize,dbName);
	if(result < 0)
	{
		cout<<"Error in Database creation....";
		return result;
	}
	cout<<"Database created."<<endl;
	cout<<"Query: "<<result<<" rows affected.";
	delete newdb;
	return result;
}

int createTable()
{
	string tableName;
	int colCount;
	cout<<"Enter the table name to be created: ";
	cin>>tableName;
	cout<<"Enter the no. of columns: ";
	cin>>colCount;
	// Check if table name is valid and usable
	// If available, do not create a SysTables entry for it, return error
	// Create a SysTables entry for the table
	if(colCount == 0)
	{
		// The user has not specified any columns.
		// The columns has to be specified, by adding them through alter query
		// Insertion of entries to the table not possible.
	}
	for(int i=0;i<colCount;i++)
	{
		// Create entry for each column....
		// Check whether column names are unique while inserting it.....
		// If not return error
	}
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
		case 2: cout<<"Creating the table......"<<endl;
			result = createTable();
			cout<<"Query: "<<result<<" rows affected.";
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
