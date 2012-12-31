// The code below is for testing process only.... Will not be part of main code......

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#include<math.h>

//#include"globals1.h"
#include"SysTables.h"
#include"FileHandling.cpp"

using namespace std;

int nextPageID = 5;
int sysTablesPageID = 2;

// Use the next two functions for read and write as they are not available yet.... Remove calls for them after integration

// My functions.... These are used for testing purposes only......
int writeSysTableBufferCreate(string fName,SysTables newPage)
{
	ofstream fd;
	fd.open(fName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	int pageSize = newPage.getPageSize();
	int _pageSize = pageSize;
	int pageID = newPage.getpageID();
	short pagePriority = newPage.getpagePriority();
	int nextSysTablePage = newPage.getnextSysTablePage();
	int curSysTableSize = newPage.getCurSysTableSize(); // Check this is not required... This can be got by METADATASIZE + noe*TABLEENTRYSIZE
	int remSysTableSize = newPage.getRemSysTableSize();
	long sysTableEntryPointer = newPage.getSysTableEntryPointer();
	int noOfEntries = newPage.getNoOfEntries();

	char *buffer = new char [pageSize];
	int result;

	/* Nothing to read as file is empty......
	result = pageRead(_pageID,buffer); // From Chinmay, get buffer and page from cache........
	if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}*/

	// Copying contents to buffer.....
	cout<<"Object contents....: "<<endl;
	cout<<"Remaining size: "<<remSysTableSize<<endl;
	cout<<"MetaData Size: "<<SYSTABLEMETADATASIZE<<endl;
	cout<<"Entry Space: "<<SYSTABLEENTRYSPACE<<endl;
	cout<<"Page Size: "<<PAGESIZE<<endl;
	cout<<"Calculation: "<<(PAGESIZE-SYSTABLEMETADATASIZE)<<endl;
	//cout<<2048-26;
	cout<<"PageID PTR: "<<PAGEIDPTR<<" Value: "<<pageID<<endl;
	memcpy(&buffer[PAGEIDPTR],&pageID,sizeof(int));
	cout<<"PagePriority PTR: "<<PAGEPRIPTR<<" Value: "<<pagePriority<<endl;
	memcpy(&buffer[PAGEPRIPTR],&pagePriority,sizeof(short));
	cout<<"Next PTR: "<<NEXTPTR<<" Value: "<<nextSysTablePage<<endl;
	memcpy(&buffer[NEXTPTR],&nextSysTablePage,sizeof(int));
	cout<<"CurrentSysTabSize PTR: "<<CURSYSTABSIZEPTR<<" Value: "<<curSysTableSize<<endl;
	memcpy(&buffer[CURSYSTABSIZEPTR],&curSysTableSize,sizeof(int));
	cout<<"RemSysTabSize PTR: "<<REMSYSTABSIZEPTR<<" Value: "<<remSysTableSize<<endl;
	memcpy(&buffer[REMSYSTABSIZEPTR],&remSysTableSize,sizeof(int));
	cout<<"SysTabEntry PTR: "<<SYSTABLEENTRYPTR<<" Value: "<<sysTableEntryPointer<<endl;
	memcpy(&buffer[SYSTABLEENTRYPTR],&sysTableEntryPointer,sizeof(long));
	cout<<"NoOfEnt PTR: "<<SYSTABNOEPTR<<" Value: "<<noOfEntries<<endl;
	memcpy(&buffer[SYSTABNOEPTR],&noOfEntries,sizeof(int));


	// WriteBuffer.... Chinmay's function call here.....
	result = pageWrite(fd,pageID,buffer,pageSize);

	if(result == -1)
	{
		cout<<"Page could not be written...."<<endl;
		return -1;
	}
	fd.close();
	return _pageSize;
}

int createSysTableFile(string fName,int pageSize)
{
	SysTables newPage(1,pageSize);
	newPage.setPageSize(pageSize);
	//int _pageSize = pageSize;
	//newPage.setRemSysTableSize(SYSTABLEENTRYSPACE);
	int result = writeSysTableBufferCreate(fName,newPage);
	return result;
}

int createNewSysTablesPage(string fName,int pageSize)
{
	int _pageSize = pageSize;
	int pageID = sysTablesPageID;
	char * curBuff = new char [pageSize];

	ifstream fd;
	fd.open(fName.c_str(),ios::binary);
	fd.seekg((pageID-1)*pageSize,ifstream::beg);
	fd.read(curBuff,pageSize);

	int nextPage;

	memcpy(&nextPage,&curBuff[NEXTPTR],sizeof(int));

	while(nextPage != -1)
	{
		fd.seekg((nextPage-1)*pageSize,ifstream::beg);
		fd.read(curBuff,pageSize);
		memcpy(&nextPage,&curBuff[NEXTPTR],sizeof(int));
	}


	SysTables newPage;
	newPage.setPageSize(pageSize);
	
	newPage.setRemSysTableSize(SYSTABLEENTRYSPACE);

	int nextSysTablesPageID = nextPageID;// This will be the next free ID......
	nextPageID = nextPageID+1;// This is done here.... This will be taken care of while getting the next free page ID

	newPage.setpageID(nextSysTablesPageID);
	memcpy(&curBuff[NEXTPTR],&nextSysTablesPageID,sizeof(int));

	char * buffer = new char [_pageSize];

	newPage.writeSysTableBuffer(buffer);

	ofstream fd1;
	fd1.open(fName.c_str(),ios::binary);

	//int result = pageWrite(&fd1,nextSysTablesPageID,buffer,_pageSize);
	fd1.seekp((nextSysTablesPageID-1)*_pageSize,ofstream::beg);
	fd1.write(buffer,_pageSize);

	int curPage;
	memcpy(&curPage,&curBuff[PAGEIDPTR],sizeof(int));

	//result = pageWrite(&fd1,curPage,curBuff,_pageSize);
	fd1.seekp((curPage-1)*_pageSize,ofstream::beg);
	fd1.write(curBuff,_pageSize);

	delete buffer;
	delete curBuff;
	return 1;
}

int insertEntry(string fName,int pageSize)
{
	char *buffer = new char [pageSize];
	int pageID = 1;// Go to page 2 to read the contents of first SysTab page
	string dbName,tabName;
	char tabType,rowFormat;
	unsigned short colCount;
	int tableRows,noOfPages;
	string remarks;
	unsigned short keyColumns,dataLength,recLength;
	char encodingScheme;
	int avgRowLength,directoryPagePointer;

	cout<<"Enter the database name: ";
	//cin>>dbName;
	dbName = "k2";
	/*if(dbName.length() > 64)
	{
		cout<<"Error: Database name should be less than 64 bytes."<<endl;
		return -1;
	}*/
	cout<<"Enter the table name: ";
	//cin>>tabName;
	tabName = "k1";
	/*if(tabName.length() > 64)
	{
		cout<<"Error: Table name should be less than 64 bytes."<<endl;
		return -1;
	}*/
	cout<<"Enter type of entry: ";
	//cin>>tabType;
	tabType='T';
	/*if(tabType != 'T' || tabType != 'V')
	{
		cout<<"Error: Table type has to be T for Table or V for View";
		return -1;
	}*/
	cout<<"Enter no. of columns: ";
	//cin>>colCount;
	colCount = 12;
	//int shortSize = sizeof(short);
	//unsigned short maxShort = pow(2,shortSize) - 1;
	/*if(!(colCount>=0 && colCount<=maxShort))
	{
		cout<<"Error: A table can have only "<<maxShort<<" columns. And the value cannot be negative.";
		return -1;
	}*/
	cout<<"Enter the data format: ";
	//cin>>rowFormat;
	rowFormat='D';
	/*if(rowFormat != 'D' || rowFormat != 'F')
	{
		cout<<"Error: The data format can be Dynamic or fixed.";
		return -1;
	}*/
	cout<<"Enter the no. of table rows: ";
//	cin>>tableRows;
	tableRows=122;
	/*int intSize = sizeof(int);
	unsigned int maxint = pow(2,intSize) - 1;
	if(!(tableRows>=0 && tableRows<=intSize))
	{
		cout<<"Error: The number of rows should be in between 0 and "<<maxint;
		return -1;
	}*/
	cout<<"Enter the no. of pages: ";
	//cin>>noOfPages;
	noOfPages = 23;
	/*if(!(noOfPages>=0 && noOfPages<=intSize))
	{
		cout<<"Error: The number of pages should be in between 0 and "<<maxint;
		return -1;
	}*/
	cout<<"Enter the remarks: ";
	//cin>>remarks;
	remarks="None";
	/*if(remarks.length() > 256)
	{
		cout<<"Error: Remarks should be less than 256 bytes."<<endl;
		return -1;
	}*/
	cout<<"Enter no. of key columns: ";
	//cin>>keyColumns;
	keyColumns=2;
	/*if(!(keyColumns>=0 && keyColumns<=maxShort))
	{
		cout<<"Error: A table can have only "<<maxShort<<" key columns. And the value cannot be negative.";
		return -1;
	}*/
	cout<<"Enter the Data Length: ";
	//cin>>dataLength;
	dataLength=10;
	/*if(!(dataLength>=0 && dataLength<=maxShort))
	{
		cout<<"Error: A table can have only "<<maxShort<<" data length. And the value cannot be negative.";
		return -1;
	}*/
	cout<<"Enter the Record Length: ";
	//cin>>recLength;
	recLength=30;
	/*if(!(recLength>=0 && recLength<=maxShort))
	{
		cout<<"Error: A table can have only "<<maxShort<<" record length. And the value cannot be negative.";
		return -1;
	}*/
	cout<<"Enter the encoding scheme: ";
	//cin>>encodingScheme;
	encodingScheme='A';
	/*if(encodingScheme != 'E' || encodingScheme != 'A' || encodingScheme != 'M' || encodingScheme != 'U')
	{
		cout<<"Error: The encoding scheme can be EBCDIC(E), ASCII(A),Unicode(U) or multiple CCSID units(M).";
		return -1;
	}*/
	cout<<"Enter the average record length: ";
	//cin>>avgRowLength;
	avgRowLength=30;
	/*if(!(avgRowLength>=0 && avgRowLength<=intSize))
	{
		cout<<"Error: The average record length should be in between 0 and "<<maxint;
		return -1;
	}*/
	cout<<"Enter the directory page pointer: ";
	//cin>>directoryPagePointer;
	directoryPagePointer = 5;
	/*if(!(directoryPagePointer>=0 && directoryPagePointer<=intSize))
	{
		cout<<"Error: The directory page pointer should be in between 0 and "<<maxint;
		return -1;
	}*/
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
	//int result1 = pageRead(fName,pageID,buffer,pageSize);
	SysTables * sysTables = new SysTables(buffer,pageSize);

	// Create a new SysTableEntry object.....
	SysTableEntry newSysTableEntry;
	newSysTableEntry.setTableName(tabName);
	newSysTableEntry.setDBName(dbName);
	newSysTableEntry.setTableType(tabType);
	newSysTableEntry.setRowFormat(rowFormat);
	newSysTableEntry.setColCount(colCount);
	newSysTableEntry.setTableRows(tableRows);
	newSysTableEntry.setNoPages(noOfPages);
	newSysTableEntry.setRemarks(remarks);
	newSysTableEntry.setKeyColumns(keyColumns);
	newSysTableEntry.setDataLength(dataLength);
	newSysTableEntry.setRecLength(recLength);
	newSysTableEntry.setEncodingScheme(encodingScheme);
	newSysTableEntry.setAvgRowLength(avgRowLength);
	newSysTableEntry.setDirectoryPagePointer(directoryPagePointer);

	char *dataEntry = new char [SYSTABLEENTRYSIZE];
	//char *newBuff = new char [pageSize];

	int result = newSysTableEntry.fillBuffer(dataEntry);

	bool inserted = 0;

	int nextSysTablePage,insertedPage;
	int _pageSize = pageSize;

	memcpy(&nextSysTablePage,&buffer[NEXTPTR],sizeof(int));

	while(1)
	{
		if(sysTables->checkEntryInsertion() == 1)
		{
			sysTables->createNewSysTableEntry(dataEntry,buffer);
			inserted = 1;
			insertedPage = sysTables->getpageID();
			break;
		}
		if(nextSysTablePage==-1 && inserted != 1)
			break;
		//fd.seekg((nextSysTablePage-1)*pageSize,ifstream::beg);
		//fd.read(buffer,pageSize);
		memcpy(&nextSysTablePage,&buffer[NEXTPTR],sizeof(int));
	}
	

	if(inserted == 0)
	{
		//int nextSysTablesPageID = nextPageID;// This will be the next free ID......
		//nextPageID = nextPageID+1;// This is done here.... This will be taken care of while getting the next free page ID
		//int pageSize = sysTables->getPageSize();

		//char * buffer = new char [pageSize];

		//buffer = readBuffer(nextFreeID);

		//SysTables * newsysTablesPage = new SysTables(nextSysTablesPageID,pageSize);
		//newsysTablesPage->writeSysTableBuffer(newBuff);

		//newsysTablesPage->createNewSysTableEntry(dataEntry,newBuff);

		//sysTables->setnextSysTablePage(nextSysTablesPageID);
		//memcpy(&buffer[NEXTPTR],&nextSysTablesPageID,sizeof(int));
		//insertedPage = nextSysTablesPageID;

		//delete newsysTablesPage;
		delete dataEntry;
		delete buffer;
		//delete newBuff;
		delete sysTables;
		return -1;
	}

	/*ofstream fd1;
	fd1.open(fName.c_str(),ios::binary);
	if(!fd1)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	fd1.seekp((insertedPage-1)*pageSize,ofstream::beg);
	fd1.write(buffer,pageSize);
	fd1.close();*/
	result = pageWrite(fName,insertedPage,buffer,pageSize);

	//int curpage = sysTables->getpageID();
	//fd1.seekp((curpage-1)*pageSize,ofstream::beg);
	//fd1.write(buffer,pageSize);

	delete dataEntry;
	delete buffer;
	//delete newBuff;
	delete sysTables;
	return 1;
}

int deleteSysTablesPage(string fName,int pageSize)
{
	int pno;
	int _pageSize = pageSize;
	cout<<"Enter the page no. to be deleted: ";
	cin>>pno;

	int pageID = sysTablesPageID;

	ifstream fd;
	fd.open(fName.c_str(),ios::binary);

	fd.seekg((pageID-1)*pageSize,ifstream::beg);
	char *curBuff = new char [pageSize];
	char *prevBuff = new char [pageSize];

	int curPage,nextPage,prevPage=-1;

	bool deletedFound = 0;
	fd.read(curBuff,pageSize);
	memcpy(&curPage,&curBuff[PAGEIDPTR],sizeof(int));
	memcpy(&nextPage,&curBuff[NEXTPTR],sizeof(int));

	while(nextPage != -1)
	{
		if(curPage == pno)
		{
			deletedFound = 1;
			break;
		}
		prevPage = curPage;
		curPage = nextPage;
		fd.seekg((curPage-1)*pageSize,ifstream::beg);
		fd.read(curBuff,pageSize);
		memcpy(&nextPage,&curBuff[NEXTPTR],sizeof(int));
	}

	if(deletedFound == 0)
	{
		cout<<"Page not found or does not exist...."<<endl;
		return -1;
	}

	ofstream fd1;
	fd1.open(fName.c_str(),ios::binary);

	if(curPage == 2)
	{
		if(nextPage == -1)
		{
			// Delete page
			// Add page 2 to Free List

			sysTablesPageID = 2;

			char * buffer = new char [pageSize];

			int _pageID,_pagePriority=0,_nextFreePage = nextPageID;
			memcpy(&_pageID,&curBuff[PAGEIDPTR],sizeof(int));

			memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&_nextFreePage,sizeof(int));

			fd1.seekp((_pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);

			delete buffer;
		}
		else
		{
			sysTablesPageID = nextPage;
			// Add page 2 to Free List
			char * buffer = new char [pageSize];

			int _pageID,_pagePriority=0,_nextFreePage = nextPageID;
			memcpy(&_pageID,&curBuff[PAGEIDPTR],sizeof(int));

			memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&_nextFreePage,sizeof(int));

			fd1.seekp((_pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);

			fd.seekg((prevPage-1)*pageSize,ifstream::beg);
			fd.read(buffer,pageSize);
			memcpy(&buffer[NEXTPTR],&nextPage,sizeof(int));
			fd1.seekp((prevPage-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);

			delete buffer;
		}
	}
	else
	{
		if(nextPage == -1)
		{
			// Delete page
			char * buffer = new char [pageSize];

			int _pageID,_pagePriority=0,_nextFreePage = nextPageID;
			memcpy(&_pageID,&curBuff[PAGEIDPTR],sizeof(int));

			memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&_nextFreePage,sizeof(int));

			fd1.seekp((_pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);

			delete buffer;
		}
		else
		{
			char * buffer = new char [pageSize];

			int _pageID,_pagePriority=0,_nextFreePage = nextPageID;
			memcpy(&_pageID,&curBuff[PAGEIDPTR],sizeof(int));

			memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&_nextFreePage,sizeof(int));

			fd1.seekp((_pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);

			fd.seekg((prevPage-1)*pageSize,ifstream::beg);
			fd.read(buffer,pageSize);
			memcpy(&buffer[NEXTPTR],&nextPage,sizeof(int));
			fd1.seekp((prevPage-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);

			delete buffer;
		}
	}
	delete curBuff;
	delete prevBuff;
}

int deleteSysTablesPage(string fName,int pageSize,int pNumber)
{
	int pno;
	int _pageSize = pageSize;
	cout<<"Enter the page no. to be deleted: ";
	cin>>pno;

	int pageID = sysTablesPageID;

	ifstream fd;
	fd.open(fName.c_str(),ios::binary);

	fd.seekg((pageID-1)*pageSize,ifstream::beg);
	char *curBuff = new char [pageSize];
	char *prevBuff = new char [pageSize];

	int curPage,nextPage,prevPage;

	bool deletedFound = 1;

	fd.read(curBuff,pageSize);
	memcpy(&curPage,&curBuff[PAGEIDPTR],sizeof(int));
	memcpy(&nextPage,&curBuff[NEXTPTR],sizeof(int));

	while(curPage != pNumber)
	{
		if(nextPage == -1)
		{
			deletedFound = 0;
			break;
		}
		prevPage = curPage;
		curPage = nextPage;
		fd.seekg((curPage-1)*pageSize,ifstream::beg);
		fd.read(curBuff,pageSize);
		memcpy(&nextPage,&curBuff[NEXTPTR],sizeof(int));
	}

	if(deletedFound == 0 && curPage != pNumber)
	{
		cout<<"Page not found or does not exist for deletion...."<<endl;
		return -1;
	}

	char * buffer = new char [pageSize];
	int pagePriority = 0;

	ofstream fd1;
	fd1.open(fName.c_str(),ios::binary);

	if(curPage == 2)
	{
		if(nextPage == -1)
		{
			// Delete page
			memcpy(&buffer[PAGEIDPTR],&curPage,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&nextPage,sizeof(int));
		}
		else
		{
			sysTablesPageID = nextPage;
			memcpy(&buffer[PAGEIDPTR],&curPage,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&nextPage,sizeof(int));
		}
	}
	else
	{
		if(nextPage == -1)
		{
			// Delete page
			memcpy(&buffer[PAGEIDPTR],&curPage,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&nextPage,sizeof(int));
		}
		else
		{
			memcpy(&buffer[PAGEIDPTR],&curPage,sizeof(int));
			memcpy(&buffer[PAGEPRIPTR],&pagePriority,sizeof(short));
			memcpy(&buffer[NEXTPTR],&nextPage,sizeof(int));
		}
	}

	fd1.seekp((curPage-1)*pageSize,ifstream::beg);
	fd1.write(buffer,pageSize);

	delete curBuff;
	delete prevBuff;
	delete buffer;
}


int deleteEntry(string fName,int pageSize)
{
	string tableName;
	cout<<"Enter the name of the table to be deleted: ";
	cin>>tableName;

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

		SysTables * sysTables1 = new SysTables(buffer,pageSize);

		int result = sysTables1->deleteSysTableEntry(tableName,buffer);

		if(result != -1)
		{
			entryFound = 1;
			ofstream fd1;
			fd1.open(fName.c_str(),ios::binary);
			fd1.seekp((pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);
			int noOfEntries;
			memcpy(&noOfEntries,&buffer[SYSTABNOEPTR],sizeof(int));
			if(noOfEntries == 0)
			{
				// Delete page
				result = deleteSysTablesPage(fName,pageSize,pageID);
			}
			delete buffer;
			delete sysTables1;
			return 1;
		}

		delete buffer;
		delete sysTables1;
	}

	if(entryFound == 1)
	{
		return 1;
	}
	else
		return -1;
}

int modifyEntry(string fName,int pageSize,int varID,int value,string value1,char value2)
{
	string tableName;
	cout<<"Enter the name of the table to be modified: ";
	cin>>tableName;

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

		SysTables * sysTables1 = new SysTables(buffer,pageSize);

		int result = sysTables1->deleteSysTableEntry(tableName,buffer);

		if(result != -1)
		{
			entryFound = 1;
			ofstream fd1;
			fd1.open(fName.c_str(),ios::binary);
			fd1.seekp((pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);
			int noOfEntries;
			memcpy(&noOfEntries,&buffer[SYSTABNOEPTR],sizeof(int));
			if(noOfEntries == 0)
			{
				// Delete page
				result = deleteSysTablesPage(fName,pageSize,pageID);
			}
			delete buffer;
			delete sysTables1;
			return 1;
		}

		delete buffer;
		delete sysTables1;
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
	unsigned char *buffer = reinterpret_cast<unsigned char *>(buf);
	//fd.read(buffer,PAGESIZE);
	/*for(int i=0; i<size; i+=16)
	{
		cout<<setfill('0')<<setw(4)<<setbase(16)<<i<<"  ";
		
		for(int j=0;j<16;j++){
			cout<<setfill('0')<<setw(2)<<hex<<int(buffer[i+j]);
			cout<<" ";
		}
		for(int j=0;j<16;j++){
			cout<<isprint(*(buf+i+j)?*(buf+i+j):'.');
			cout<<" ";
		}
		cout<<setbase(10);
		cout<<endl;
	}*/
	printf("\n");
	for(int i=0;i<size;i+=16)
	{
		printf("%04x ",i);
		for(int j=0;j<16;j++)
			printf("%02x ",(unsigned char)*(buffer+i+j));
		printf("  ");
		for(int j=0;j<16;j++)
			printf("%c",isprint(*(buffer+i+j))?*(buffer+i+j):'.');
		printf("\n");
	}
	fd.close();
	delete buf;
}

void sysTablesFunction(int choice)
{
	string fName;
	int pageSize,pNo,result;
	switch(choice)
	{
		case 1: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = createSysTableFile(fName,pageSize);
			if(result == -1)
				cout<<"SysTable page creation failed!!!"<<endl;
			break;
		case 2: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = createNewSysTablesPage(fName,pageSize);
			if(result == -1)
				cout<<"SysTableEntry insertion failed!!!"<<endl;
			break;
		case 3: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = insertEntry(fName,pageSize);
			break;
		case 4: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = deleteEntry(fName,pageSize);
			break;
		case 5: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = deleteSysTablesPage(fName,pageSize);
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
		cout<<"SysTables testing....."<<endl;
		cout<<"Enter 1 to create a SysTables file and insert a SysTable page."<<endl;
		cout<<"Enter 2 to create a SysTable page."<<endl;
		cout<<"Enter 3 to create a SysTable entry."<<endl;
		cout<<"Enter 4 to delete a SysTable entry."<<endl;
		cout<<"Enter 5 to delete a SysTable page."<<endl;
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
			sysTablesFunction(choice);
	}while(choice!=7);
}
