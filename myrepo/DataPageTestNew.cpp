// DataPageTestNew.cpp test for Data page......

#include"DataPage.h"
#include"FileHandling.cpp"

int dataPageCreate()
{
	string fileName;
	int _pageSize;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>_pageSize;
	int pageID = 1,nextPTR = -1;
	short pagePriority = 1;
	DataPage dataPage(_pageSize,pageID);
	char * buffer = new char [_pageSize];
	dataPage.fillBuffer(buffer);
	memcpy(&buffer[PAGEIDPTR],&pageID,sizeof(int));
	memcpy(&buffer[PAGEPRIPTR],&pagePriority,sizeof(short));
	memcpy(&buffer[NEXTPTR],&nextPTR,sizeof(int));
	pageWrite(fileName,pageID,buffer,_pageSize);
	delete buffer;
}

int insertEntry()
{
	string fileName;
	int pageSize,pageNumber;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>pageSize;

	int noOfPages;

	// Opening the file to get the file size and calculate the no of pages......
	int fd = open(fileName.c_str(),O_RDWR,0666);
	if(fd < 0)
	{
		cout<<"File not found or does not exist....";
		return -1;
	}
	off_t fileSize = lseek(fd,0,SEEK_END);
	noOfPages = fileSize/pageSize;
	close(fd);

	int i;

	//long int val1 = lrand48();
	double val2 = drand48();
	if(val2 < 0.05)
		val2 = 0;
	else if(val2 < 0.1)
		val2 = 0.049;
	else if(val2 < 0.15)
		val2 = 0.09;
	else if(val2 < 0.2)
		val2 = 0.149;
	else if(val2 < 0.25)
		val2 = 0.199;
	else if(val2 < 0.3)
		val2 = 0.249;
	else if(val2 < 0.35)
		val2 = 0.299;
	else if(val2 < 0.40)
		val2 = 0.349;
	else if(val2 < 0.45)
		val2 = 0.399;
	else if(val2 < 0.5)
		val2 = 0.449;
	else if(val2 < 0.55)
		val2 = 0.499;
	else if(val2 < 0.6)
		val2 = 0.549;
	else if(val2 < 0.65)
		val2 = 0.599;
	else if(val2 < 0.7)
		val2 = 0.649;
	else if(val2 < 0.75)
		val2 = 0.699;
	else if(val2 < 0.8)
		val2 = 0.749;
	else if(val2 < 0.85)
		val2 = 0.799;
	else if(val2 < 0.9)
		val2 = 0.849;
	else if(val2 < 0.95)
		val2 = 0.899;
	else
		val2 = 0.949;

	long int val1 = val2*100;

	short longIntSize = sizeof(long int),doubleSize = sizeof(double);

	char * dataBuf = new char [sizeof(short)+sizeof(long int)+sizeof(short)+sizeof(double)];

	memcpy(&dataBuf[0],&longIntSize,sizeof(short));
	memcpy(&dataBuf[0+sizeof(short)],&val1,sizeof(long int));
	memcpy(&dataBuf[0+sizeof(short)+sizeof(long int)],&doubleSize,sizeof(short));
	memcpy(&dataBuf[0+sizeof(short)+sizeof(long int)+sizeof(short)],&val2,sizeof(double));

	int dataSize = sizeof(short)+sizeof(long int)+sizeof(short)+sizeof(double);

	// Try inserting into one of the pages of the data page file

	for(i=1;i<=noOfPages;i++)
	{
		char * buffer = new char [pageSize];
		int res = pageRead(fileName,i,buffer,pageSize);
		DataPage * newDataPage = new DataPage(buffer,pageSize);
		int inserted = newDataPage->insertData(buffer,dataBuf,dataSize);
		if(inserted == -1 && i == noOfPages)
		{
			// Create new Data page and insert the data there code......
			// Otherwise this function will contain code for defragmenting the data page....
			cout<<"Unimplemented...."<<endl;
			cout<<"Create a new page and insert there..."<<endl;
			cout<<"After integration you will have to insert a directory entry and then create a data page and insert the data there...";
			cout<<endl;
		}
		if(inserted > 0)
		{
			// Data was inserted.....
			cout<<endl<<"Data was inserted in "<<i<<"th page."<<endl;
			res = pageWrite(fileName,i,buffer,pageSize);
		}
		delete newDataPage;
		delete buffer;
	}

	delete dataBuf;
	return 1;
}

int insertMultipleEntries()
{
	string fileName;
	int pageSize,noOfEntries;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>pageSize;
	cout<<"Enter the no. of Entries you want to insert: ";
	cin>>noOfEntries;

	int noOfPages;

	// Opening the file to get the file size and calculate the no of pages......
	int fd = open(fileName.c_str(),O_RDWR,0666);
	if(fd < 0)
	{
		cout<<"File not found or does not exist....";
		return -1;
	}
	off_t fileSize = lseek(fd,0,SEEK_END);
	noOfPages = fileSize/pageSize;
	close(fd);

	int j;
	for(j=0;j<noOfEntries;j++)
	{
		cout<<"Trying to insert "<<j<<"th entry....."<<endl;
		int i;

		//long int val1 = lrand48();
		double val2 = drand48();
		if(val2 < 0.05)
			val2 = 0;
		else if(val2 < 0.1)
			val2 = 0.049;
		else if(val2 < 0.15)
			val2 = 0.09;
		else if(val2 < 0.2)
			val2 = 0.149;
		else if(val2 < 0.25)
			val2 = 0.199;
		else if(val2 < 0.3)
			val2 = 0.249;
		else if(val2 < 0.35)
			val2 = 0.299;
		else if(val2 < 0.40)
			val2 = 0.349;
		else if(val2 < 0.45)
			val2 = 0.399;
		else if(val2 < 0.5)
			val2 = 0.449;
		else if(val2 < 0.55)
			val2 = 0.499;
		else if(val2 < 0.6)
			val2 = 0.549;
		else if(val2 < 0.65)
			val2 = 0.599;
		else if(val2 < 0.7)
			val2 = 0.649;
		else if(val2 < 0.75)
			val2 = 0.699;
		else if(val2 < 0.8)
			val2 = 0.749;
		else if(val2 < 0.85)
			val2 = 0.799;
		else if(val2 < 0.9)
			val2 = 0.849;
		else if(val2 < 0.95)
			val2 = 0.899;
		else
			val2 = 0.949;

		long int val1 = val2*100;

		short longIntSize = sizeof(long int),doubleSize = sizeof(double);

		char * dataBuf = new char [sizeof(short)+sizeof(long int)+sizeof(short)+sizeof(double)];

		memcpy(&dataBuf[0],&longIntSize,sizeof(short));
		memcpy(&dataBuf[0+sizeof(short)],&val1,sizeof(long int));
		memcpy(&dataBuf[0+sizeof(short)+sizeof(long int)],&doubleSize,sizeof(short));
		memcpy(&dataBuf[0+sizeof(short)+sizeof(long int)+sizeof(short)],&val2,sizeof(double));

		int dataSize = sizeof(short)+sizeof(long int)+sizeof(short)+sizeof(double);

		// Try inserting into one of the pages of the data page file

		int inserted;

		for(i=1;i<=noOfPages;i++)
		{
			char * buffer = new char [pageSize];
			int res = pageRead(fileName,i,buffer,pageSize);
			DataPage * newDataPage = new DataPage(buffer,pageSize);
			inserted = newDataPage->insertData(buffer,dataBuf,dataSize);
			if(inserted == -1 && i == noOfPages)
			{
				// Create new Data page and insert the data there code......
				// Otherwise this function will contain code for defragmenting the data page....
				cout<<"Unimplemented...."<<endl;
				cout<<"Create a new page and insert there..."<<endl;
				cout<<"After integration you will have to insert a directory entry and then create a data page and insert the data there...";
				cout<<endl;
			}
			if(inserted > 0)
			{
				// Data was inserted.....
				cout<<endl<<"Data was inserted in "<<i<<"th page."<<endl;
				res = pageWrite(fileName,i,buffer,pageSize);
			}
			delete newDataPage;
			delete buffer;
		}
		delete dataBuf;
		if(inserted == -1 && i == noOfPages)
		{
			cout<<"As we are inserting fixed length records, we need not insert any further to this file, unless new pages are added to the file...."<<endl;
			cout<<"Add new page and then try adding new entries...."<<endl;
			break;
		}
	}
	cout<<"The function was able to insert "<<j<<" entries...."<<endl;
	return j;
}

int searchEntries()
{
	string fileName;
	int pageSize,searchChoice;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>pageSize;
	cout<<"Each record has 2 items."<<endl<<"Enter 1 to search the records with Integer value."<<endl;
	cout<<"Enter 2 to search the records with double value."<<endl;
	cout<<"Enter 3 to search the records with both Integer and double values"<<endl;
	cout<<"Enter 4 to search the records with either of the values."<<endl;
	cout<<"Enter your choice: ";
	cin>>searchChoice;

	if(searchChoice < 1 || searchChoice > 4)
	{
		cout<<"Invalid choice for search....";
		return -1;
	}

	int longIntValue;
	double doubleValue;

	if(searchChoice == 1 || searchChoice == 3 || searchChoice == 4)
	{
		cout<<"Enter the Integer value: ";
		cin>>longIntValue;
	}
	if(searchChoice == 2 || searchChoice == 3 || searchChoice == 4)
	{
		cout<<"Enter the Double value: ";
		cin>>doubleValue;
	}

	int _pageSize = pageSize;

	int noOfPages;

	// Opening the file to get the file size and calculate the no of pages......
	int fd = open(fileName.c_str(),O_RDWR,0666);
	if(fd < 0)
	{
		cout<<"File not found or does not exist....";
		return -1;
	}
	off_t fileSize = lseek(fd,0,SEEK_END);
	noOfPages = fileSize/pageSize;
	close(fd);

	int i,j,resCount=0,result = 0;
	for(i=1;i<=noOfPages;i++)
	{
		resCount = 0;
		char * buffer = new char [pageSize];
		int res = pageRead(fileName,i,buffer,pageSize);
		DataPage * newDataPage = new DataPage(buffer,pageSize);
		cout<<"Searching data in page "<<i<<"...."<<endl;
		if(searchChoice == 1 || searchChoice == 2)
		{
			int dataPos;
			if(searchChoice == 1)
			{
				dataPos = 1;
				char * searchData = new char [sizeof(long int)];
				memcpy(searchData,&longIntValue,sizeof(long int));
				resCount = newDataPage->searchData(buffer,searchData,dataPos);
				delete searchData;
			}
			else
			{
				dataPos = 2;
				char * searchData = new char [sizeof(double)];
				memcpy(searchData,&doubleValue,sizeof(double));
				resCount = newDataPage->searchData(buffer,searchData,dataPos);
				delete searchData;
			}
		}
		else if(searchChoice == 3)
		{
			int noOfEntries = newDataPage->getslotCounter();
			if(noOfEntries == 0)
			{
				cout<<"There are no entries in this page..."<<endl;
			}
			else
			{
				for(j=0;j<noOfEntries;j++)
				{
					int slotSize;
					long slotPointer;
					memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
					memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
					if(slotSize<0)
					{
						// Slot has been already deleted....
						// No need to search.....
						continue;
					}
					else
					{
						char * dataBuffer = new char [slotSize];
						memcpy(dataBuffer,&buffer[slotPointer],slotSize);
						long int val1Data;
						double val2Data;
						memcpy(&val1Data,&dataBuffer[0+sizeof(short)],sizeof(long int));
						memcpy(&val2Data,&dataBuffer[0+sizeof(short)+sizeof(long int)+sizeof(short)],sizeof(double));
						delete dataBuffer;
						if(longIntValue == val1Data && doubleValue == val2Data)
						{
							resCount++;
						}
					}
				}
			}
		}
		else if(searchChoice == 4)
		{
			int noOfEntries = newDataPage->getslotCounter();
			if(noOfEntries == 0)
			{
				cout<<"There are no entries in this page..."<<endl;
			}
			else
			{
				for(j=0;j<noOfEntries;j++)
				{
					int slotSize;
					long slotPointer;
					memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
					memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
					if(slotSize<0)
					{
						// Slot has been already deleted....
						// No need to search.....
						continue;
					}
					else
					{
						char * dataBuffer = new char [slotSize];
						memcpy(dataBuffer,&buffer[slotPointer],slotSize);
						long int val1Data;
						double val2Data;
						memcpy(&val1Data,&dataBuffer[0+sizeof(short)],sizeof(long int));
						memcpy(&val2Data,&dataBuffer[0+sizeof(short)+sizeof(long int)+sizeof(short)],sizeof(double));
						delete dataBuffer;
						if(longIntValue == val1Data || doubleValue == val2Data)
						{
							resCount++;
						}
					}
				}
			}
		}
		delete newDataPage;
		delete buffer;
		result = result + resCount;
	}
	cout<<"The no. of matched records is: "<<result;
	return result;
}

int deleteEntries()
{
	string fileName;
	int pageSize,delChoice;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>pageSize;
	cout<<"Each record has 2 items."<<endl;
	cout<<"Enter 1 to delete the records with Integer value."<<endl;
	cout<<"Enter 2 to delete the records with double value."<<endl;
	cout<<"Enter 3 to delete the records with both Integer and double values"<<endl;
	cout<<"Enter 4 to delete the records with either of the values."<<endl;
	cout<<"Enter your choice: ";
	cin>>delChoice;

	if(delChoice < 1 || delChoice > 4)
	{
		cout<<"Invalid choice for deletion....";
		return -1;
	}

	int longIntValue;
	double doubleValue;

	if(delChoice == 1 || delChoice == 3 || delChoice == 4)
	{
		cout<<"Enter the Integer value: ";
		cin>>longIntValue;
	}
	if(delChoice == 2 || delChoice == 3 || delChoice == 4)
	{
		cout<<"Enter the Double value: ";
		cin>>doubleValue;
	}
	

	int _pageSize = pageSize;

	int noOfPages;

	// Opening the file to get the file size and calculate the no of pages......
	int fd = open(fileName.c_str(),O_RDWR,0666);
	if(fd < 0)
	{
		cout<<"File not found or does not exist....";
		return -1;
	}
	off_t fileSize = lseek(fd,0,SEEK_END);
	noOfPages = fileSize/pageSize;
	close(fd);

	int i,j,resCount=0,result = 0;
	for(i=1;i<=noOfPages;i++)
	{
		resCount = 0;
		char * buffer = new char [pageSize];
		int res = pageRead(fileName,i,buffer,pageSize);
		DataPage * newDataPage = new DataPage(buffer,pageSize);
		cout<<"Deleting data from page "<<i<<"...."<<endl;
		if(delChoice == 1 || delChoice == 2)
		{
			int dataPos;
			if(delChoice == 1)
			{
				dataPos = 1;
				char * deleteData = new char [sizeof(long int)];
				memcpy(deleteData,&longIntValue,sizeof(long int));
				resCount = newDataPage->deleteData(buffer,deleteData,dataPos);
				delete deleteData;
			}
			else
			{
				dataPos = 2;
				char * deleteData = new char [sizeof(double)];
				memcpy(deleteData,&doubleValue,sizeof(double));
				resCount = newDataPage->deleteData(buffer,deleteData,dataPos);
				delete deleteData;
			}
		}
		else if(delChoice == 3)
		{
			int noOfEntries = newDataPage->getslotCounter();
			if(noOfEntries == 0)
			{
				cout<<"There are no entries in this page..."<<endl;
			}
			else
			{
				for(j=0;j<noOfEntries;j++)
				{
					int slotSize;
					long slotPointer;
					memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
					memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
					if(slotSize<0)
					{
						// Slot has been already deleted....
						// No need to delete.....
						continue;
					}
					else
					{
						char * dataBuffer = new char [slotSize];
						memcpy(dataBuffer,&buffer[slotPointer],slotSize);
						long int val1Data;
						double val2Data;
						memcpy(&val1Data,&dataBuffer[0+sizeof(short)],sizeof(long int));
						memcpy(&val2Data,&dataBuffer[0+sizeof(short)+sizeof(long int)+sizeof(short)],sizeof(double));
						delete dataBuffer;
						if(longIntValue == val1Data && doubleValue == val2Data)
						{
							slotSize = (-1)*slotSize;
							resCount++;
							memcpy(&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],&slotSize,sizeof(int));
						}
					}
				}
			}
		}
		else if(delChoice == 4)
		{
			int noOfEntries = newDataPage->getslotCounter();
			if(noOfEntries == 0)
			{
				cout<<"There are no entries in this page..."<<endl;
			}
			else
			{
				for(j=0;j<noOfEntries;j++)
				{
					int slotSize;
					long slotPointer;
					memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
					memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
					if(slotSize<0)
					{
						// Slot has been already deleted....
						// No need to delete.....
						continue;
					}
					else
					{
						char * dataBuffer = new char [slotSize];
						memcpy(dataBuffer,&buffer[slotPointer],slotSize);
						long int val1Data;
						double val2Data;
						memcpy(&val1Data,&dataBuffer[0+sizeof(short)],sizeof(long int));
						memcpy(&val2Data,&dataBuffer[0+sizeof(short)+sizeof(long int)+sizeof(short)],sizeof(double));
						delete dataBuffer;
						if(longIntValue == val1Data || doubleValue == val2Data)
						{
							slotSize = (-1)*slotSize;
							resCount++;
							memcpy(&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],&slotSize,sizeof(int));
						}
					}
				}
			}
		}
		delete newDataPage;
		res = pageWrite(fileName,i,buffer,pageSize);
		delete buffer;
		result = result + resCount;
	}
	cout<<"The no. of deleted records is: "<<result;
	return result;
}

int modifyEntries()
{
	return -1;
}

void printhex()
{
	string fileName;
	int pageSize,pageNumber;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>pageSize;
	cout<<"Enter the page number: ";
	cin>>pageNumber;
	char * buffer = new char [pageSize];
	int result = pageRead(fileName,pageNumber,buffer,pageSize);
	int i,j;
	unsigned char *buf = (unsigned char *) buffer;

	printf("\n");
	for(i=0;i<pageSize;i+=16)
	{
		printf("%04x ",i);
		for(j=0;j<16;j++)
			printf("%02x ",*(buf+i+j));
		printf("  ");
		for(j=0;j<16;j++)
			printf("%c",isprint(*(buf+i+j))?*(buf+i+j):'.');
		printf("\n");
	}
	delete buffer;
}

void printAllSlotsInformation()
{
	string fileName;
	int pageSize;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>pageSize;

	int _pageSize = pageSize;

	int noOfPages;

	// Opening the file to get the file size and calculate the no of pages......
	int fd = open(fileName.c_str(),O_RDWR,0666);
	if(fd < 0)
	{
		cout<<"File not found or does not exist....";
		return;
	}
	off_t fileSize = lseek(fd,0,SEEK_END);
	noOfPages = fileSize/pageSize;
	close(fd);

	int i;
	for(i=1;i<=noOfPages;i++)
	{
		cout<<"Printing Slots Information of "<<i<<"th Data Page...."<<endl;
		char * buffer = new char [pageSize];
		int res = pageRead(fileName,i,buffer,pageSize);
		DataPage * newDataPage = new DataPage(buffer,pageSize);
		int noOfEntries = newDataPage->getslotCounter();
		if(noOfEntries == 0)
		{
			cout<<"There are no entries in this page..."<<endl;
			delete buffer;
			continue;
		}
		cout<<"Slot ID\tSlot Pointer\tSlot Size\tDeleted"<<endl;
		int j;
		for(j=0;j<noOfEntries;j++)
		{
			cout<<j+1<<"\t";
			int slotSize;
			long slotPointer;
			bool deletedFlag=0;
			memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
			memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
			if(slotSize<0)
			{
				slotSize = (-1)*slotSize;
				deletedFlag=1;
			}
			cout<<slotPointer<<"\t\t"<<slotSize<<"\t\t";
			if(deletedFlag == 1)
				cout<<"True"<<endl;
			else
				cout<<"False"<<endl;
		}
		delete buffer;
	}
}

void printAllSlotsData()
{
	string fileName;
	int pageSize;
	cout<<"Enter the file name for the data page file: ";
	cin>>fileName;
	cout<<"Enter the page size for the file: ";
	cin>>pageSize;

	int _pageSize = pageSize;

	int noOfPages;

	// Opening the file to get the file size and calculate the no of pages......
	int fd = open(fileName.c_str(),O_RDWR,0666);
	if(fd < 0)
	{
		cout<<"File not found or does not exist....";
		return;
	}
	off_t fileSize = lseek(fd,0,SEEK_END);
	noOfPages = fileSize/pageSize;
	close(fd);

	int i;
	for(i=1;i<=noOfPages;i++)
	{
		cout<<"Printing Slots Data of "<<i<<"th Data Page...."<<endl;
		char * buffer = new char [pageSize];
		int res = pageRead(fileName,i,buffer,pageSize);
		DataPage * newDataPage = new DataPage(buffer,pageSize);
		int noOfEntries = newDataPage->getslotCounter();
		if(noOfEntries == 0)
		{
			cout<<"There are no entries in this page..."<<endl;
			delete buffer;
			continue;
		}
		cout<<"Slot ID\tSlot Pointer\tSlot Size\tDeleted\tSlot Data1\tSlot Data2"<<endl;
		int j;
		for(j=0;j<noOfEntries;j++)
		{
			cout<<j+1<<"\t";
			int slotSize;
			long slotPointer;
			bool deletedFlag=0;
			memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
			memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
			if(slotSize<0)
			{
				slotSize = (-1)*slotSize;
				deletedFlag=1;
			}
			cout<<slotPointer<<"\t\t"<<slotSize<<"\t\t";
			if(deletedFlag == 1)
				cout<<"True\t--\t\t--"<<endl;
			else
			{
				cout<<"False\t";
				char * dataBuffer = new char [slotSize];
				memcpy(dataBuffer,&buffer[slotPointer],slotSize);
				long int val1Data;
				double val2Data;
				memcpy(&val1Data,&dataBuffer[0+sizeof(short)],sizeof(long int));
				memcpy(&val2Data,&dataBuffer[0+sizeof(short)+sizeof(long int)+sizeof(short)],sizeof(double));
				cout<<val1Data<<"\t\t"<<val2Data<<endl;
				delete dataBuffer;
			}
		}
		delete buffer;
	}
}

void dataPageFunction(int choice)
{
	int result;
	switch(choice)
	{
		case 1: cout<<"Creating a data page file...."<<endl;
			result = dataPageCreate();
			break;
		case 3: cout<<"Inserting an entry into the data page..."<<endl;
			cout<<"As of now we will be inserting one random integer and one random float value..."<<endl;
			result = insertEntry();
			break;
		case 4: cout<<"Inserting multiple entries into a data page...."<<endl;
			cout<<"As of now we will be inserting one random integer and one random float value..."<<endl;
			result = insertMultipleEntries();
			break;
		case 5: cout<<"Searching entries in the data page...."<<endl;
			result = searchEntries();
			break;
		case 6: cout<<"Deleting entries in the data page...."<<endl;
			result = deleteEntries();
			break;
		case 7: cout<<"Modifying entries in the data page..."<<endl;
			result = modifyEntries();
			break;
		case 8: cout<<"Printing the hex dump...."<<endl;
			printhex();
			break;
		case 9: cout<<"Printing all the slots information..."<<endl;
			printAllSlotsInformation();
			break;
		case 10: cout<<"Printing all the slots data...."<<endl;
			 printAllSlotsData();
			 break;
		default: cout<<"Unimplemented operation."<<endl;
			 break;
	}
}

int main()
{
	int choice;
	do
	{
		cout<<"Testing of data pages....."<<endl;
		cout<<"Enter 1 to create a data page file (Creates a file with a single data page)."<<endl;
		cout<<"Enter 2 to add a new data page to an existing data page file."<<endl;
		cout<<"Enter 3 to insert an entry into the data page file."<<endl;
		cout<<"Enter 4 to insert multiple entries."<<endl;
		cout<<"Enter 5 to search entries in the data page."<<endl;
		cout<<"Enter 6 to delete entries in the data page."<<endl;
		cout<<"Enter 7 to modify some entries in the data page."<<endl;
		cout<<"Enter 8 to display the hex dump for a page."<<endl;
		cout<<"Enter 9 to print all the slots information."<<endl;
		cout<<"Enter 10 to print all the slots data."<<endl;
		cout<<"Enter 11 to exit."<<endl;
		cout<<"As of now only fixed length records are added."<<endl;
		cout<<"Enter your choice: ";
		cin>>choice;
		if(choice < 1 || choice > 11)
		{
			cout<<"Invalid option!!!"<<endl;
			continue;
		}
		dataPageFunction(choice);
	}while(choice != 11);
}
