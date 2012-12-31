// The code below is for testing process only.... Will not be part of main code......

#include "SysColumns.h"
#include"FileHandling.cpp"

int writeSysColumnBufferCreate(string fName,SysColumns newPage)
{
	ofstream fd;
	fd.open(fName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	/*There will be no pages to read.....
	result = pageRead(_pageID,buffer); // From Chinmay, get buffer and page from cache........
	if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}*/

	int pageSize = newPage.getPageSize();
	int _pageSize = pageSize;
	int _pageID = newPage.getpageID();
	short _pagePriority = newPage.getpagePriority();
	int _nextSysColumnsPage = newPage.getnextSysColumnsPage();
	int _curSysColumnsSize = newPage.getCurSysColumnsSize(); // Check this is not required... This can be got by METADATASIZE + noe*TABLEENTRYSIZE
	int _remSysColumnsSize = newPage.getRemSysColumnsSize();
	long _sysColumnsEntryPointer = newPage.getSysColumnsEntryPointer();
	int _noOfEntries = newPage.getNoOfEntries();

	char *buffer = new char [pageSize];
	int result;

	cout<<"Object contents....: "<<endl;
	cout<<"Remaining size: "<<_remSysColumnsSize<<endl;
	cout<<"MetaData Size: "<<SYSCOLUMNMETADATASIZE<<endl;
	cout<<"Entry Space: "<<SYSCOLUMNENTRYSPACE<<endl;
	cout<<"Page Size: "<<PAGESIZE<<endl;

	// Copying contents to buffer.....
	cout<<"PageID PTR: "<<PAGEIDPTR<<" Value: "<<_pageID<<endl;
	memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
	cout<<"PagePriority PTR: "<<PAGEPRIPTR<<" Value: "<<_pagePriority<<endl;
	memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
	cout<<"Next PTR: "<<NEXTPTR<<" Value: "<<_nextSysColumnsPage<<endl;
	memcpy(&buffer[NEXTPTR],&_nextSysColumnsPage,sizeof(int));
	cout<<"CurrentSysColSize PTR: "<<CURSYSCOLSIZEPTR<<" Value: "<<_curSysColumnsSize<<endl;
	memcpy(&buffer[CURSYSCOLSIZEPTR],&_curSysColumnsSize,sizeof(int));
	cout<<"RemSysColSize PTR: "<<REMSYSCOLSIZEPTR<<" Value: "<<_remSysColumnsSize<<endl;
	memcpy(&buffer[REMSYSCOLSIZEPTR],&_remSysColumnsSize,sizeof(int));
	cout<<"SysColEntry PTR: "<<SYSCOLUMNENTRYPTR<<" Value: "<<_sysColumnsEntryPointer<<endl;
	memcpy(&buffer[SYSCOLUMNENTRYPTR],&_sysColumnsEntryPointer,sizeof(long));
	cout<<"NoOfEnt PTR: "<<SYSCOLNOEPTR<<" Value: "<<_noOfEntries<<endl;
	memcpy(&buffer[SYSCOLNOEPTR],&_noOfEntries,sizeof(int));

	// Copying contents to buffer.....
	/*memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
	memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
	memcpy(&buffer[NEXTPTR],&_nextSysColumnsPage,sizeof(int));
	memcpy(&buffer[CURSYSCOLSIZEPTR],&_curSysColumnsSize,sizeof(int));
	memcpy(&buffer[REMSYSCOLSIZEPTR],&_remSysColumnsSize,sizeof(int));
	memcpy(&buffer[SYSCOLUMNENTRYPTR],&_sysColumnsEntryPointer,sizeof(long));
	memcpy(&buffer[SYSCOLNOEPTR],&_noOfEntries,sizeof(int));*/

	// WriteBuffer.... Chinmay's function call here.....
	result = pageWrite(fd,_pageID,buffer,pageSize);

	if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}

	fd.close();
	return _pageSize;
}

int createSysColumnFile(string fName,int pageSize)
{
	SysColumns newPage(1,pageSize);
	newPage.setPageSize(pageSize);
	int result = writeSysColumnBufferCreate(fName,newPage);
	return result;
}

int insertEntry(string fName,int pageSize)
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
	//int result1 = pageRead(fName,pageID,buffer,pageSize);
	SysColumns * sysColumns = new SysColumns(buffer,pageSize);

	SysColumnsEntry newSysColumnsEntry;
	newSysColumnsEntry.setTableName("kp1");
	newSysColumnsEntry.setDBName("k1");
	newSysColumnsEntry.setColumnName("c1");
	newSysColumnsEntry.setColumnDefault("########");
	newSysColumnsEntry.setDataType("INT");
	newSysColumnsEntry.setRemarks("None");
	newSysColumnsEntry.setIsNullable('Y');
	newSysColumnsEntry.setUpdatable('Y');
	newSysColumnsEntry.setForeignKey('N');
	newSysColumnsEntry.setGeneratedAttribute(' ');
	newSysColumnsEntry.setHidden('N');
	newSysColumnsEntry.setScale(0);
	newSysColumnsEntry.setKeyPos(0);
	newSysColumnsEntry.setOrdinalPosition(2);
	newSysColumnsEntry.setLength(20);

	char *dataEntry = new char [SYSCOLUMNENTRYSIZE];

	int result = newSysColumnsEntry.fillBuffer(dataEntry);

	bool inserted = 0;

	int nextSysColumnsPage,insertedPage;
	int _pageSize = pageSize;

	memcpy(&nextSysColumnsPage,&buffer[NEXTPTR],sizeof(int));

	while(1)
	{
		if(sysColumns->checkEntryInsertion() == 1)
		{
			sysColumns->createNewSysColumnEntry(dataEntry,buffer);
			inserted = 1;
			insertedPage = sysColumns->getpageID();
			break;
		}
		if(nextSysColumnsPage==-1 && inserted != 1)
			break;
		//fd.seekg((nextSysTablePage-1)*pageSize,ifstream::beg);
		//fd.read(buffer,pageSize);
		memcpy(&nextSysColumnsPage,&buffer[NEXTPTR],sizeof(int));
	}
	if(inserted == 0)
	{
		cout<<"Entry could not be inserted.... So, continue trying to search for empty page for SysColumns"<<endl;
		delete buffer;
		delete dataEntry;
		delete sysColumns;
		return -1;
	}
	else
	{
		cout<<"Column Entry was inserted into "<<insertedPage<<" page"<<endl;
		result = pageWrite(fName,insertedPage,buffer,pageSize);
		delete buffer;
		delete dataEntry;
		delete sysColumns;
		return 1;
	}
}

int deleteEntry(string fName,int pageSize)
{
	string columnName;
	cout<<"Enter the name of the column to be deleted: ";
	cin>>columnName;

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

		SysColumns * sysColumns = new SysColumns(buffer,pageSize);

		int result = sysColumns->deleteSysColumnEntry(columnName,buffer);

		if(result != -1)
		{
			entryFound = 1;
			ofstream fd1;
			fd1.open(fName.c_str(),ios::binary);
			fd1.seekp((pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);
			int noOfEntries;
			memcpy(&noOfEntries,&buffer[SYSCOLNOEPTR],sizeof(int));
			if(noOfEntries == 0)
			{
				// Delete page
				//result = deleteSysColumnsPage(fName,pageSize,pageID);
				cout<<"All the entries are deleted.... You can add this page to the free list....."<<endl;
				cout<<"Make sure you handle next and prev pointers carefully......"<<endl;
			}
			delete buffer;
			delete sysColumns;
			return 1;
		}

		delete buffer;
		delete sysColumns;
	}

	if(entryFound == 1)
	{
		return 1;
	}
	else
		return -1;
}

int modifyEntry(string fName,int pageSize)
{
	string columnName;
	cout<<"Enter the name of the column to be deleted: ";
	cin>>columnName;

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

		SysColumns * sysColumns = new SysColumns(buffer,pageSize);

		int result = sysColumns->searchSysColumnEntry(columnName,buffer);

		if(result != -1)
		{
			entryFound = 1;
			// Write code for modifying the column......
			ofstream fd1;
			fd1.open(fName.c_str(),ios::binary);
			fd1.seekp((pageID-1)*pageSize,ofstream::beg);
			fd1.write(buffer,pageSize);
			int noOfEntries;
			memcpy(&noOfEntries,&buffer[SYSCOLNOEPTR],sizeof(int));
			if(noOfEntries == 0)
			{
				// Delete page
				//result = deleteSysColumnsPage(fName,pageSize,pageID);
				cout<<"All the entries are deleted.... You can add this page to the free list....."<<endl;
				cout<<"Make sure you handle next and prev pointers carefully......"<<endl;
			}
			delete buffer;
			delete sysColumns;
			return 1;
		}

		delete buffer;
		delete sysColumns;
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
	//unsigned char *buffer = reinterpret_cast<unsigned char *>(buf);
	//fd.read(buffer,PAGESIZE);
	/*
	for(int i=0; i<size; i+=16)
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
	}
	*/
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



void sysColumnsFunction(int choice)
{
	string fName;
	int pageSize,pNo,result;
	switch(choice)
	{
		case 1: cout<<"Enter the file name: ";
			cin>>fName;
			cout<<"Enter the page size: ";
			cin>>pageSize;
			result = createSysColumnFile(fName,pageSize);
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
		cout<<"SysColumns testing....."<<endl;
		cout<<"Enter 1 to create a SysColumns file and insert a SysColumns page."<<endl;
		cout<<"Enter 2 to create a SysColumns page."<<endl;
		cout<<"Enter 3 to create a SysColumn entry."<<endl;
		cout<<"Enter 4 to delete a SysColumn entry."<<endl;
		cout<<"Enter 5 to delete a SysColumns page."<<endl;
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
			sysColumnsFunction(choice);
	}while(choice!=7);
}
