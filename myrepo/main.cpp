// Main.cpp

void databaseFunction(int choice)
{
	string dbName;
	switch(choice)
	{
		case 1: cout<<"\n Enter the name of the database: ";
			cin>>dbName;
			createDB(dbName);
			break;
		case 10: cout<<"\n Exiting the database";
			// Call Chinmay's code for flushing the cache....
			break;
	}
}

int main()
{
	int choice;

	do
	{
		cout<<"\n Enter 1 to create a database.";
		cout<<"\n Enter 10 to exit.";
		cout<<"\n Enter your choice: ";
		cin>>choice;
		if(choice!=1 || choice!=10)
			continue;
		else
			databaseFunction(choice);
	}while(choice!=10);
}
