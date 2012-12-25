// SysIndexTest.cpp for testing SysIndex 

#include "SysIndex.h"
#include"FileHandling.cpp"

int writeSysIndexBufferCreate(string fName,SysIndex newPage)
{
	ofstream fd;
	fd.open(fName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	int _pageSize = newPage.getPageSize();

	char *buffer = new char [_pageSize];
	int result;

	int _pageID = newPage.getPageID();
	short _pagePriority = newPage.getPagePriority();
	int _nextSysIndexPage = newPage.getNextSysIndexPage();
	int _curSysIndexSize = newPage.getCurSysIndexSize(); // Check this is not required... This can be got by METADATASIZE + noe*TABLEENTRYSIZE
	int _remSysIndexSize = newPage.getRemSysIndexSize();
	long _sysIndexEntryPointer = newPage.getSysIndexEntryPointer();
	int _noOfEntries = newPage.getNoOfEntries();

	memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
	memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
	memcpy(&buffer[NEXTPTR],&_nextSysIndexPage,sizeof(int));
	memcpy(&buffer[CURSYSINDSIZEPTR],&_curSysIndexSize,sizeof(int));
	memcpy(&buffer[REMSYSINDSIZEPTR],&_remSysIndexSize,sizeof(int));
	memcpy(&buffer[SYSINDEXENTRYPTR],&_sysIndexEntryPointer,sizeof(long));
	memcpy(&buffer[SYSINDNOEPTR],&_noOfEntries,sizeof(int));

	result = pageWrite(fd,_pageID,buffer,_pageSize);

	if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}

	fd.close();
	return _pageSize;
}

int createSysIndexFile(string fName,int pageSize)
{
	SysIndex newSysIndexPage(1,pageSize);
	int result = writeSysIndexBufferCreate(fName,newSysIndexPage);
	return result;
}

int insertSysIndexEntry(string fName,int pageSize)
{
	char *buffer = new char [pageSize];
	int pageID = 1;

	ifstream fd;
	fd.open(fName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return -1;
	}
	fd.seekg((pageID-1)*pageSize,ifstream::beg);
	fd.read(buffer,pageSize);
	fd.close();

	SysIndex * sysIndex = new SysIndex(buffer,pageSize);

	SysIndexEntry newSysIndexEntry;
	newSysIndexEntry.setIndexName("Index1");
	newSysIndexEntry.setDBName("DB1");
	newSysIndexEntry.setTableName("Table1");
	newSysIndexEntry.setUniqueRule('P');
	newSysIndexEntry.setColCount(0);
	newSysIndexEntry.setFanOutNo(3);
	newSysIndexEntry.setIndexPageID(300);
	newSysIndexEntry.setPageSize(pageSize);
	newSysIndexEntry.setEraseRule('N');
	newSysIndexEntry.setCloseRule('N');
	newSysIndexEntry.setRemarks("None");
	newSysIndexEntry.setAvgKeyLength(20);

	char *dataEntry = new char [SYSINDEXENTRYSIZE];

	int result = newSysIndexEntry.fillBuffer(dataEntry);

	bool inserted = 0;

	int nextSysIndexPage,insertedPage;
	int _pageSize = pageSize;

	memcpy(&nextSysIndexPage,&buffer[NEXTPTR],sizeof(int));

	while(1)
	{
		if(sysIndex->checkEntryInsertion() == 1)
		{
			sysIndex->createNewSysIndexEntry(dataEntry,buffer);
			inserted = 1;
			insertedPage = sysIndex->getPageID();
			break;
		}
		if(nextSysIndexPage==-1 && inserted != 1)
			break;
		//fd.seekg((nextSysTablePage-1)*pageSize,ifstream::beg);
		//fd.read(buffer,pageSize);
		memcpy(&nextSysIndexPage,&buffer[NEXTPTR],sizeof(int));
	}
	if(inserted == 0)
	{
		cout<<"Entry could not be inserted.... So, continue trying to search for empty page for SysColumns"<<endl;
		delete buffer;
		delete dataEntry;
		delete sysIndex;
		return -1;
	}
	else
	{
		cout<<"Column Entry was inserted into "<<insertedPage<<" page"<<endl;
		result = pageWrite(fName,insertedPage,buffer,pageSize);
		delete buffer;
		delete dataEntry;
		delete sysIndex;
		return 1;
	}
}

int deleteSysIndexEntry(string fName,int pageSize)
{
	string indexName;
	cout<<"Enter the name of the index to be deleted: ";
	cin>>indexName;

	int pageID = 1;
	int _pageSize = pageSize;

	ifstream fd;
	fd.open(fName.c_str(),ios::binary);

	bool entryFound = 0;

	int nextPage = 2;

	while(nextPage != -1)
	{
		//int noOfEntries;
		char * buffer = new char [pageSize];

		fd.seekg((pageID-1)*pageSize,ifstream::beg);
		fd.read(buffer,pageSize);

		memcpy(&pageID,&buffer[PAGEIDPTR],sizeof(int));
		memcpy(&nextPage,&buffer[NEXTPTR],sizeof(int));

		SysIndex * sysIndex = new SysIndex(buffer,pageSize);

		int result = sysIndex->deleteSysIndexEntry(indexName,buffer);

		if(result != -1)
		{
			entryFound = 1;
			ofstream fd1;
			fd1.open(fName.c_str(),ios::binary);
			fd1.seekp((pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);
			int noOfEntries;
			memcpy(&noOfEntries,&buffer[SYSINDNOEPTR],sizeof(int));
			if(noOfEntries == 0)
			{
				// Delete page
				//result = deleteSysColumnsPage(fName,pageSize,pageID);
				cout<<"All the entries are deleted.... You can add this page to the free list....."<<endl;
				cout<<"Make sure you handle next and prev pointers carefully......"<<endl;
			}
			delete buffer;
			delete sysIndex;
			return 1;
		}

		delete buffer;
		delete sysIndex;
	}

	if(entryFound == 1)
	{
		return 1;
	}
	else
		return -1;
}


void printhex(string fileName,int pageNo,int pageSize)
{
	ifstream fd;
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return;
	}
	fd.seekg(0,ifstream::end);
	long size = fd.tellg();
	int no_pages = size/pageSize;
	fd.seekg((pageNo-1)*pageSize,ifstream::beg);
	//unsigned char *buffer = new unsigned char[PAGESIZE];
	char *buf = new char[pageSize];
	fd.read(buf,pageSize);
	printf("\n");
	for(int i=0;i<size;i+=16)
	{
		printf("%04x ",i);
		for(int j=0;j<16;j++)
			printf("%02x ",(unsigned char)*(buf+i+j));
		printf("  ");
		for(int j=0;j<16;j++)
			printf("%c",isprint(*(buf+i+j))?*(buf+i+j):'.');
		printf("\n");
	}
	fd.close();
	delete buf;
}

void sysIndexFunction(int choice)
{
	string fName;
	int pageSize,pNo,result;
	switch(choice)
	{
		case 1: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = createSysIndexFile(fName,pageSize);
			break;
		case 3: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = insertSysIndexEntry(fName,pageSize);
			break;
		case 4: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = deleteSysIndexEntry(fName,pageSize);
			break;
		case 6: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page no.: ";
			cin>>pNo;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			printhex(fName,pNo,pageSize);
			break;
		default: cout<<"Unimplemented operation..."<<endl;
			 break;
	}
}

int main()
{
	int choice;
	do
	{
		cout<<"SysIndex testing....."<<endl;
		cout<<"Enter 1 to create a SysIndex file and insert a SysIndex page."<<endl;
		cout<<"Enter 2 to create a SysIndex page."<<endl;
		cout<<"Enter 3 to create a SysIndex entry."<<endl;
		cout<<"Enter 4 to delete a SysIndex entry."<<endl;
		cout<<"Enter 5 to delete a Sysindex page."<<endl;
		cout<<"Enter 6 to view the data in hex."<<endl;
		cout<<"Enter 7 to exit."<<endl;
		cout<<"Enter your choice: ";
		cin>>choice;
		if(choice < 1 || choice > 7)
		{
			cout<<"Incorrect choice..."<<endl;
			continue;
		}
		if(choice < 7)
			sysIndexFunction(choice);	
	}while(choice!=7);
}
