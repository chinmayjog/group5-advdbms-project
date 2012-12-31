// DirectoryPageTest.cpp

#include "dataDirectoryPage.h"
//#include "FreeList.h"
#include "FileHandling.cpp"
#define DEPAGESIZE1 pageSize
#define PAGEID_PTR1 (DEPAGESIZE1-sizeof(int))
#define PAGEPRIORITY_PTR1 (PAGEID_PTR1-sizeof(short))
#define NEXT_PTR1 (PAGEPRIORITY_PTR1-sizeof(int))
#define NOE_PTR1 (NEXT_PTR1-sizeof(int))
#define MAXTFS_PTR1 (NOE_PTR1-sizeof(int))
#define REMSIZE_PTR1 (MAXTFS_PTR1-sizeof(int))
#define NEXTDE_PTR1 (REMSIZE_PTR1-sizeof(long))

//string fileName = "testDirectoryPage";

int createNewFile(string fileName,int pageSize)
{
	DirectoryPage newDirectoryPage;
	int pageID=1;
	char *buffer = new char [pageSize];
	cout<<"Creating a directory page...."<<endl;
	newDirectoryPage.setpageID(pageID);
	newDirectoryPage.setPageSize(pageSize);
	newDirectoryPage.setRemDirectoryPageSize(pageSize-DEPAGEMETADATA);
	newDirectoryPage.fillBuffer(buffer);
	memcpy(&buffer[PAGEID_PTR1],&pageID,sizeof(int));
	int result = pageWrite(fileName,pageID,buffer,pageSize);
	delete buffer;
}

int createNewPage(string fileName,int pageSize)
{
	char *buffer = new char [pageSize];
	char *newbuff = new char [pageSize];
	int totFileSize = totalFileSize(fileName);
	int totPages = totFileSize/pageSize;
	int result;
	string nfileName = fileName;

	result = pageRead(fileName,totPages,buffer,pageSize);
	DirectoryPage * directoryPage = new DirectoryPage(buffer,pageSize);
	directoryPage->setNextDirectoryPagePointer(totPages+1);
	directoryPage->fillBuffer(buffer);
	result = pageWrite(fileName,totPages,buffer,pageSize);

	DirectoryPage * newDirectoryPage = new DirectoryPage(pageSize);
	newDirectoryPage->setpageID(totPages+1);
	newDirectoryPage->fillBuffer(newbuff);
	totPages = totPages + 1;
	memcpy(&newbuff[PAGEID_PTR1],&totPages,sizeof(int));
	result = pageWrite(fileName,totPages,newbuff,pageSize);

	delete newDirectoryPage;
	delete newbuff;
	delete buffer;
	delete directoryPage;
	return totPages+1;
}

void dispPageData(string fileName,int pageSize)
{
	int pageID;
	cout<<"Enter the page no. for which to display properties: ";
	cin>>pageID;
	int totFileSize = totalFileSize(fileName);
	int noOfPages = totFileSize/pageSize;
	if(pageID<1 || pageID>noOfPages)
	{
		cout<<"Invalid Page ID."<<endl;
		return;
	}
	char *buffer = new char [pageSize];
	int result = pageRead(fileName,pageID,buffer,pageSize);
	DirectoryPage * directoryPage = new DirectoryPage(buffer,pageSize);
	directoryPage->setPageSize(pageSize);
	cout<<"Page Details are: "<<endl;
	cout<<"Page ID: "<<directoryPage->getpageID()<<endl;
	cout<<"Page Priority: "<<directoryPage->getpagePriority()<<endl;
	cout<<"Page Size: "<<directoryPage->getPageSize()<<endl;
	cout<<"No. of Entries: "<<directoryPage->getNoE()<<endl;
	cout<<"Max. TFS: "<<directoryPage->getMaxTFS()<<endl;
	cout<<"Remaining Directory Page size: "<<directoryPage->getRemDirectoryPageSize()<<endl;
	cout<<"Next Directory Page: "<<directoryPage->getNextDirectoryPagePointer()<<endl;
	cout<<"Next Directory Entry Pointer: "<<directoryPage->getdePTR()<<endl;
	delete directoryPage;
	delete buffer;
}

void printhex(string fileName,int pageID,int pageSize)
{
	char *buffer = new char [pageSize];
	int result = pageRead(fileName,pageID,buffer,pageSize);
			
	printf("\n");
	for(int i=0;i<pageSize;i+=16)
	{
		printf("%04x ",i);
		for(int j=0;j<16;j++)
			printf("%02x ",(unsigned char)*(buffer+i+j));
		printf("  ");
		for(int j=0;j<16;j++)
			printf("%c",isprint(*(buffer+i+j))?*(buffer+i+j):'.');
		printf("\n");
	}
	delete buffer;
}

int insertEntry(string fileName,int pageSize,int noOfPages)
{
	int dataPageID,pageTFS,resultID;
	int i,result;

	char *buffer = new char [pageSize];

	cout<<"Enter the data page ID: ";
	cin>>dataPageID;
	cout<<"Enter the Total Free Space for the page: ";
	cin>>pageTFS;

	bool found = 0;
	for(i=1;i<=noOfPages;i++)
	{
		result = pageRead(fileName,i,buffer,pageSize);
		DirectoryPage * directoryPage = new DirectoryPage(buffer,pageSize); 
		// Check I can just get the size and test.... Deciding not to.....
		directoryPage->setPageSize(pageSize);
		int noOfEntries = directoryPage->getNoE();
		if(noOfEntries < 0)
		{
			delete directoryPage;
			continue;
		}
		if(directoryPage->checkDirectoryEntryInsertion() != 0)
		{
			found = 1;
			resultID = i;
			delete directoryPage;
			break;
		}
		delete directoryPage;
	}
	if(i == noOfPages && found == 0)
	{
		cout<<"Creating a new Directory Page... No space.... And then inserting..."<<endl;
		int newPageID = createNewPage(fileName,pageSize);
		result = pageRead(fileName,newPageID,buffer,pageSize);
		resultID = newPageID;
	}
	DirectoryPage * directoryPage = new DirectoryPage(buffer,pageSize);
	DirectoryPageEntry * newDPEntry = new DirectoryPageEntry(dataPageID,pageTFS);
	char * entryBuff = new char [DESIZE];
	newDPEntry->fillBuffer(entryBuff);
	result = directoryPage->insertDirectoryPageEntry(entryBuff,buffer);
	result = pageWrite(fileName,resultID,buffer,pageSize);
	delete entryBuff;
	delete newDPEntry;
	delete directoryPage;
	delete buffer;
	return resultID;
}

int deleteEntry(string fileName,int pageSize,int noOfPages)
{
	int result,dataPageID;
	char *buffer = new char [pageSize];
	cout<<"Enter the data page ID to be deleted: ";
	cin>>dataPageID;
	bool found = 0;
	int foundPage;
	int i;
	for(i=1;i<=noOfPages;i++)
	{
		result = pageRead(fileName,i,buffer,pageSize);
		DirectoryPage * directoryPage = new DirectoryPage(buffer,pageSize); 
		// Check I can just get the size and test.... Deciding not to.....
		if(directoryPage->deleteDirectoryPageEntry(dataPageID,buffer) != -1)
		{
			found = 1;
			foundPage = i;
			delete directoryPage;
			break;
		}
		delete directoryPage;
	}
	if(found == 1)
	{
		result = pageWrite(fileName,foundPage,buffer,pageSize);
		cout<<"Entry was deleted from page no. "<<foundPage<<endl;
	}
	if(i == noOfPages+1 && found == 0)
	{
		cout<<"Incorrect Page ID.... It is not available under this directory entry...."<<endl;
		foundPage = -1;
	}
	delete buffer;
	return foundPage;
}

int deletePage(string fileName,int pageSize,int dirPageID)
{
	int totFileSize = totalFileSize(fileName);
	int noOfPages = totFileSize/pageSize;
	char *buffer = new char [pageSize];
	if(dirPageID<1 || dirPageID>noOfPages)
	{
		cout<<"Incorrect page ID...."<<endl;
		return -1;
	}
	//int nextPageID = dirPageID + 1;
	//int prevPageID = dirPageID - 1;
	int result = pageRead(fileName,dirPageID,buffer,pageSize);
	int noOfEntries;
	memcpy(&noOfEntries,&buffer[NOE_PTR1],sizeof(int));
	noOfEntries = (-1)*noOfEntries;
	memcpy(&buffer[NOE_PTR1],&noOfEntries,sizeof(int));
	result = pageWrite(fileName,dirPageID,buffer,pageSize);
	delete buffer;
	return 1;	
}

int modifyData(string fileName,int pageSize)
{
	int dataPageID,tfsValue,result;
	int totFileSize = totalFileSize(fileName);
	int noOfPages = totFileSize/pageSize;
	char *buffer = new char [pageSize];
	cout<<"Enter the data page ID whose TFS is to be modified: ";
	cin>>dataPageID;
	cout<<"Enter the TFS value: ";
	cin>>tfsValue;
	if(tfsValue > pageSize || tfsValue < 0)
	{
		cout<<"Incorrect TFS value."<<endl;
		return -1;
	}
	bool found = 0;
	int foundPage=-1;
	int i;
	for(i=1;i<=noOfPages;i++)
	{
		result = pageRead(fileName,i,buffer,pageSize);
		DirectoryPage * directoryPage = new DirectoryPage(buffer); 
		// Check I can just get the size and test.... Deciding not to.....
		if(directoryPage->updateDirectoryPageEntry(dataPageID,tfsValue,buffer) != -1)
		{
			found = 1;
			foundPage = i;
			delete directoryPage;
			break;
		}
		delete directoryPage;
	}
	if(found == 1)
		result = pageWrite(fileName,foundPage,buffer,pageSize);
	if(i == noOfPages && found == 0)
	{
		cout<<"Incorrect Page ID.... It is not available under this directory entry...."<<endl;
	}
	delete buffer;
	return foundPage;
}

void printSlotsInformation(string fileName,int pageSize,int pageID)
{
	//int pageID;
	char *buffer = new char [pageSize];
	int result = pageRead(fileName,pageID,buffer,pageSize);
	int totFileSize = totalFileSize(fileName);
	int noOfPages = totFileSize/pageSize;
	if(pageID<1 || pageID>noOfPages)
	{
		cout<<"Invalid page no."<<endl;
		return;
	}
	int noOfEntries;
	memcpy(&noOfEntries,&buffer[NOE_PTR1],sizeof(int));
	if(noOfEntries == 0)
	{
		cout<<"There are no entries in this page."<<endl;
		return;
	}
	else if(noOfEntries < 0)
	{
		cout<<"This is a deleted Directory page."<<endl;
		return;
	}
	else
	{
		cout<<"Directory Entries information: "<<endl;
		cout<<"Data Page ID\tTFS Value\tDeleted"<<endl;
		DirectoryPage * directoryPage = new DirectoryPage(buffer); 
		/*for(int i=0;i<noOfEntries;i++)
		{
			bool flag;
			memcpy(&flag,&buffer[FIRSTDESLOTPTR-((i+1)*sizeof(bool))],sizeof(bool));
			char *entryBuffer = new char [DESIZE];
			memcpy(entryBuffer,&buffer[0+i*DESIZE],DESIZE);
			DirectoryPageEntry *dirPageEntry = new DirectoryPageEntry(entryBuffer);
			if(flag == 1)
				cout<<dirPageEntry->getPageID()<<"\t\t"<<dirPageEntry->getTFS()<<"\t\t"<<"False";
			else
				cout<<dirPageEntry->getPageID()<<"\t\t"<<dirPageEntry->getTFS()<<"\t\t"<<"True";
			delete dirPageEntry;
		}*/
		directoryPage->printSlotsInformation(buffer);
		delete directoryPage;
	}
	delete buffer;
}

void pagesSummary(string fileName,int pageSize)
{
	int totFileSize = totalFileSize(fileName);
	int noOfPages = totFileSize/pageSize;
	cout<<"File details: "<<endl;
	cout<<"Total Size of the file: "<<totFileSize<<" bytes"<<endl;
	cout<<"Page Size: "<<pageSize<<" bytes"<<endl;
	cout<<"Total no. of pages: "<<noOfPages<<endl;
	cout<<"Individual Page details: "<<endl;
	for(int i=1;i<=noOfPages;i++)
	{
		char *buffer = new char [pageSize];
		int result = pageRead(fileName,i,buffer,pageSize);
		DirectoryPage * directoryPage = new DirectoryPage(buffer);
		directoryPage->setPageSize(pageSize);
		cout<<"Page "<<i<<" Details are: "<<endl;
		cout<<"Page ID: "<<directoryPage->getpageID()<<endl;
		cout<<"Page Priority: "<<directoryPage->getpagePriority()<<endl;
		cout<<"Page Size: "<<directoryPage->getPageSize()<<endl;
		if((directoryPage->getNoE())>=0)
			cout<<"No. of Entries: "<<directoryPage->getNoE()<<endl;
		else
			cout<<"No. of Entries: "<<-1*directoryPage->getNoE()<<endl;
		cout<<"Max. TFS: "<<directoryPage->getMaxTFS()<<endl;
		cout<<"Remaining Directory Page size: "<<directoryPage->getRemDirectoryPageSize()<<endl;
		cout<<"Next Directory Page: "<<directoryPage->getNextDirectoryPagePointer()<<endl;
		cout<<"Next Directory Entry Pointer: "<<directoryPage->getdePTR()<<endl;
		if((directoryPage->getNoE())>=0)
			cout<<"Deleted Page: False";
		else
			cout<<"Deleted Page: True";
		delete directoryPage;
		delete buffer;
	}
}

void printAllPagesInformation(string fileName,int noOfPages,int pageSize)
{
	char *buffer = new char [pageSize];
	for(int i=1;i<=noOfPages;i++)
	{
		cout<<"Page "<<i<<" Details: "<<endl;
		int result = pageRead(fileName,i,buffer,pageSize);
		int noOfEntries;
	 	memcpy(&noOfEntries,&buffer[NOE_PTR1],sizeof(int));
		if(noOfEntries == 0)
		{
			cout<<"There are no entries in this page."<<endl;
			break;
		}
		else if(noOfEntries < 0)
		{
			cout<<"This is a deleted Directory page."<<endl;
			break;
		}
		else
		{
			cout<<"Directory Entries information: "<<endl;
		 	printSlotsInformation(fileName,pageSize,i);
		}
	}
	delete buffer;
}

void directoryPageFunction(int choice)
{
	int pageSize,pageID,dirPageID;
	int result;
	int totFileSize;
	string fileName;
	int noOfPages;
	int dataPageID,pageTFS;
	int newPageID;
	int tfsValue;
	switch(choice)
	{
		case 1: cout<<"Enter the name of the file for creation: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			createNewFile(fileName,pageSize);
			break;
		case 2: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			cout<<"Opening a directory page...."<<endl;
			totFileSize = totalFileSize(fileName);
			cout<<"Page details: "<<endl;
			cout<<"Total Size of the file: "<<totFileSize<<" bytes"<<endl;
			cout<<"Page Size: "<<pageSize<<" bytes"<<endl;
			noOfPages = totFileSize/pageSize;
			cout<<"Total no. of pages: "<<noOfPages<<endl;
			break;
		case 3: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			dispPageData(fileName,pageSize);
			break;
		case 4: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			totFileSize = totalFileSize(fileName);
			noOfPages = totFileSize/pageSize;
			cout<<"Enter the page no. which is to be displayed as hex: ";
			cin>>pageID;
			if(pageID<1 || pageID>noOfPages)
			{
				cout<<"Invalid Page ID."<<endl;
				break;
			}
			printhex(fileName,pageID,pageSize);
			break;
		case 5: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			cout<<"New page ID created: "<<createNewPage(fileName,pageSize)<<endl;
			break;
		case 6: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			totFileSize = totalFileSize(fileName);
			noOfPages = totFileSize/pageSize;
			result = insertEntry(fileName,pageSize,noOfPages);
			cout<<"Entry was inserted in: "<<result<<" page."<<endl;
			break;
		case 7: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			totFileSize = totalFileSize(fileName);
			noOfPages = totFileSize/pageSize;
			result = deleteEntry(fileName,pageSize,noOfPages);
			break;
		case 8: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			totFileSize = totalFileSize(fileName);
			noOfPages = totFileSize/pageSize;
			cout<<"Enter the directory page ID to be deleted: ";
			cin>>dirPageID;
			result = deletePage(fileName,pageSize,dirPageID);
			break;
		case 9: cout<<"Enter the name of the file: ";
			cin>>fileName;
			cout<<"Enter the page size for the Directory Page: ";
			cin>>pageSize;
			pagesSummary(fileName,pageSize);
			break;
		case 10: cout<<"Enter the name of the file: ";
			 cin>>fileName;
			 cout<<"Enter the page size for the Directory Page: ";
			 cin>>pageSize;
			 result = modifyData(fileName,pageSize);
			 break;
		case 11: cout<<"Enter the name of the file: ";
			 cin>>fileName;
			 cout<<"Enter the page size for the Directory Page: ";
			 cin>>pageSize;
			 cout<<"Enter the page no. whose directory entries have to be displayed: ";
			 cin>>pageID;
			 printSlotsInformation(fileName,pageSize,pageID);
			 break;
		case 12: cout<<"Enter the name of the file: ";
			 cin>>fileName;
			 cout<<"Enter the page size for the Directory Page: ";
			 cin>>pageSize;
			 totFileSize = totalFileSize(fileName);
			 noOfPages = totFileSize/pageSize;
			 cout<<"All pages information"<<endl;
			 printAllPagesInformation(fileName,noOfPages,pageSize);
			 break;
		case 13: break;
	}
}

int main()
{
	int choice;
	do
	{
		cout<<"\n Directory Page.......";
		cout<<"\n Enter 1 to create a directory page file.";
		cout<<"\n Enter 2 to open a directory page.";
		cout<<"\n Enter 3 to display directory page properties.";
		cout<<"\n Enter 4 to display data as hex.";
		cout<<"\n Enter 5 to create a new data page.";
		cout<<"\n Enter 6 to insert entry into a directory page.";
		cout<<"\n Enter 7 to delete entry from a directory page.";
		cout<<"\n Enter 8 to delete a data page.";
		cout<<"\n Enter 9 to list all the subsequent directory pages.";
		cout<<"\n Enter 10 to modify data in directory page.";
		cout<<"\n Enter 11 to print information as a table.";
		cout<<"\n Enter 12 to print information of all pages as a table.";
		cout<<"\n Enter 13 to exit.";
		cout<<"\n Enter your choice: ";
		cin>>choice;
		if(choice<1 || choice>13)
		{
			cout<<"\n Invalid option.";
			continue;
		}
		else
			directoryPageFunction(choice);
	}while(choice!=13);
}
