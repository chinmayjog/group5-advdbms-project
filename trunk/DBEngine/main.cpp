// Main.cpp

#include "DB.h"

void databaseFunction(int choice)
{
	string dbName;
	BufferManager *bu = BufferManager::getBufferManager();
	int pageSize = (*bu).getPageSize();
	DB *newdb = new DB();
	int result,i;
	query q = (query) malloc(sizeof(querystruct));
	switch(choice)
	{
		case 1: cout<<"\n Enter the name of the database: ";
			cin>>dbName;
			for(i=0;i<dbName.length();i++)
				q->dbname[i] = dbName[i];
			q->dbname[i] = '\0';
			result = newdb->createDB(q);
			break;
		case 10: cout<<"\n Exiting the database";
			// Call Chinmay's code for flushing the cache....
			break;
	}

	free(q);
}

int main()
{
	int choice;
	fdID = -1;

	do
	{
		cout<<"\n Enter 1 to create a database.";
		cout<<"\n Enter 10 to exit.";
		cout<<"\n Enter your choice: ";
		cin>>choice;
		if(choice < 1 || choice > 10)
			continue;
		else
			databaseFunction(choice);
	}while(choice!=10);
}
