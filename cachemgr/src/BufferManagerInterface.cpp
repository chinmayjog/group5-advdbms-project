#include "BufferManager.h"
#include<limits>
int getdatafileID(){
	int datafileID;
	cout<<endl<<" Enter the database id[0-2]: ";
	while(!(cin>>datafileID) || datafileID > 2 || datafileID < 0){
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout<<endl<<"\tInvalid Choice. Try Again: ";
	};
	return datafileID;
}

void initCache(BufferManager *bu){
	int pgsize = 8192, numberofpages = -1;
	cout<<"\n Enter the page size for cache(8192): ";
	cin>>pgsize;
	cout<<"\n Enter the number of pages size for cache(-1): ";
	cin>>numberofpages;
	(*bu).setPageSize(pgsize);
	(*bu).initializeCache(numberofpages); 
	cout<<"\n Cache initialized successfully."<<endl;
}

void createDB(BufferManager *bu){
	string filename;
	int size;
	cout<<"\n Enter the database name: ";
	cin>>filename;
	cout<<"\n Enter the size of database in bytes(20480): ";
	cin>>size;
	int errcode = (*bu).createDB(filename, size);
	if(errcode == 1)
		cout<<"\n Database created successfully."<<endl;
	else if(errcode == -1)
		cout<<"\n Database creation failed. Database exists."<<endl;
	else if(errcode == -2)
		cout<<"\n Database creation failed. Disk is full or write-protected."<<endl;
}

void openDB(BufferManager *bu){
	string filename;
	cout<<"\n Enter the database name: ";
	cin>>filename;
	int errcode = (*bu).openDB(filename);
	if(errcode >= 0)
		cout<<"\n Database opened successfully with fileID: "<<errcode<<"."<<endl;
	else if(errcode == -1)
		cout<<"\n Cannot open database. Too many connections open. Please try again after closing other databases."<<endl;
	else if(errcode == -2)
		cout<<"\n Database does not exist or has been moved."<<endl;
}

void readDB(BufferManager *bu){
	int datafileID, pagen, p;
	unsigned char *dest = new unsigned char[(*bu).getPageSize()];
	datafileID = getdatafileID();
	cout<<endl<<" Enter the page number [0-"<<(*bu).getPageCount(datafileID) - 1<<"]: ";
	cin>>pagen;
	cout<<endl<<" Enter the page priority: ";
	cin>>p;
	(*bu).readDB(datafileID, pagen, PagePriority(p), dest);
}

void writeDB(BufferManager *bu){
	int datafileID, pagen, p, ch;
	unsigned char *src = new unsigned char[(*bu).getPageSize()];
	datafileID = getdatafileID();
	cout<<endl<<" Enter the page number [0-"<<(*bu).getPageCount(datafileID) - 1<<"]: ";
	cin>>pagen;
	cout<<endl<<" Enter the page priority: ";
	cin>>p;
	cout<<endl<<" Enter the data: ";
	ch = '\0';
	int i=0;
	do{
		ch = getchar();
		src[i++]=ch;
	}while(ch!='.');
	(*bu).writeDB(datafileID, pagen, PagePriority(p), src);
}

void expandDB(BufferManager *bu){
	int datafileID;
	int size;
	datafileID = getdatafileID();
	cout<<endl<<" Enter the expansion size(bytes): ";
	cin>>size;
	(*bu).expandDB(datafileID, size);
}

void commitDB(BufferManager *bu){
	int datafileID;
	datafileID = getdatafileID();
	(*bu).commitFile(datafileID);
}

void commitCache(BufferManager *bu){
		(*bu).commitCache();
}

void releaseCache(BufferManager *bu){
	commitCache(bu);
	(*bu).closeAll();
	BufferManager::releaseBuffer();
}



void closeDB(BufferManager *bu){
	int datafileID = getdatafileID();
	int errcode = (*bu).closeDB(datafileID);
	if(errcode == 1){
		cout<<endl<<" Database connection with id:"<<datafileID<<" closed.";
	}
	else if(errcode == -1){
		cout<<endl<<" Wrong database connection id.";
	}
	else if(errcode == -2){
		cout<<endl<<" Database connection already closed.";
	}
}

void process_choice(int choice){
	BufferManager *bu = BufferManager::getBufferManager();
	float hr = -1;
	switch(choice){
		case 1: 
			initCache(bu);
			break;
		case 2:
			createDB(bu);
			break;
		case 3:
			openDB(bu);
			break;
		case 4:
			readDB(bu);
			break;
		case 5:
			writeDB(bu);
			break;
		case 6:
			expandDB(bu);
			break;
		case 7:
			commitDB(bu);
			break;
		case 8:
			closeDB(bu);
			break;
		case 9: 
			commitCache(bu);
			break;
		case 10:
			releaseCache(bu);
			break;
		case 11:
			cout<<endl<<"Enter Cache id: ";
			cin>>choice;
			(*bu).printHex(choice, (*bu).getPageSize());
			break;
		case 12:
			(*bu).printCacheHeaders();
			break;
		case 13:
			hr = (*bu).getHitRate();
			if(hr!=-1)
				cout<<endl<<"\tHit rate is: "<<hr*100.0<<" %"<<endl;
			else {
				cout<<endl<<"\tHit rate cannot be calculated because cache";
				cout<<" is either not initialized, or no operation";
				cout<<"	has been performed on cache yet."<<endl;
			}
			break;			
		case 14:
			releaseCache(bu);
			break;
	}
}
