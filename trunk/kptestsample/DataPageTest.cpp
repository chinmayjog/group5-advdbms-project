void createFileName(string fileName)
{
	ofstream fd;
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return;
	}
	DataPage *d1 = DataPage::getDataPage();
	char *buffer = new char[PAGESIZE];
	short slotCounter = d1->getslotCounter();
	int cfs = d1->getcfs();
	long cfsPointer = d1->getcfsPointer();
	memcpy(&buffer[PAGESLOTCOUNTER],&slotCounter,sizeof(short));
	memcpy(&buffer[PAGECFSSIZEPOINTER],&cfs,sizeof(int));
	memcpy(&buffer[PAGECFSPOINTER],&cfsPointer,sizeof(long));
	fd.write(buffer,PAGESIZE);
	fd.close();
	delete buffer;
}

void openFileName(string fileName)
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
	cout<<"\n The size of the file is: "<<size<<" bytes";
	fd.close();
}

void dispPageProperties(string fileName,int pageNo)
{
	ifstream fd;
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return;
	}
	//fd.seekg(0,ifstream::end);
	//long size = fd.tellg();
	//int no_pages = size/PAGESIZE;
	fd.seekg((pageNo-1)*PAGESIZE,ifstream::beg);
	char *buffer = new char[PAGESIZE];
	fd.read(buffer,PAGESIZE);
	DataPage *d1 = DataPage::getDataPage(buffer);
	cout<<"\n The details of the page "<<pageNo<<" are: ";
	cout<<"\n No. of Slots: "<<d1->getslotCounter();
	cout<<"\n Contiguous Free Space: "<<d1->getcfs();
	cout<<"\n Pointer to Contiguous Free Space: "<<d1->getcfsPointer();
	fd.close();
	delete buffer;
}

void dispPageProperties(ifstream fd,int pageNo)
{
	fd.seekg(0);
	fd.seekg((pageNo-1)*PAGESIZE,ifstream::beg);
	char *buffer = new char[PAGESIZE];
	fd.read(buffer,PAGESIZE);
	DataPage *d1 = DataPage::getDataPage(buffer);
	cout<<"\n The details of the page "<<pageNo<<" are: ";
	cout<<"\n No. of Slots: "<<d1->getslotCounter();
	cout<<"\n Contiguous Free Space: "<<d1->getcfs();
	cout<<"\n Pointer to Contiguous Free Space: "<<d1->getcfsPointer();
	delete buffer;
}

void dispPageProperties(string fname)
{
	ifstream fd;
	fd.open(fname.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return;
	}
	fd.seekg(0,ifstream::end);
	long size = fd.tellg();
	int no_pages = size/PAGESIZE;
	cout<<"\n Page Properties are: ";
	cout<<"\n File name: "<<fname;
	cout<<"\n No. of pages: "<<no_pages;
	cout<<"\n Detailed page information is as follows: ";
	for(int i=1;i<=no_pages;i++)
		dispPageProperties(fname,i);
}

void printhex(string fileName,int pageNo)
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
	int no_pages = size/PAGESIZE;
	fd.seekg((pageNo-1)*PAGESIZE,ifstream::beg);
	//unsigned char *buffer = new unsigned char[PAGESIZE];
	char *buf = new char[PAGESIZE];
	fd.read(buf,PAGESIZE);
	unsigned char *buffer = reinterpret_cast<unsigned char *>(buf);
	//fd.read(buffer,PAGESIZE);
	for(int i=0; i<size; i+=16)
	{
		cout<<setfill('0')<<setw(4)<<setbase(16)<<i<<"  ";
		
		for(int j=0;j<16;j++){
			cout<<setfill('0')<<setw(2)<<hex<<int(buffer[i+j]);
			cout<<" ";
		}
		cout<<setbase(10);
		cout<<endl;
	}
	/*printf("\n");
	for(int i=0;i<size;i+=16)
	{
		printf("%04x ",i);
		for(int j=0;j<16;j++)
			printf("%02x ",(unsigned char)*(buf+i+j));
		printf("  ");
		for(int j=0;j<16;j++)
			printf("%c",isprint(*(buf+i+j))?*(buf+i+j):'.');
		printf("\n");
	}*/
	fd.close();
	delete buf;
}

void printSlotsInformation(string fileName,int pageNo)
{
	ifstream fd;
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return;
	}
	fd.seekg((pageNo-1)*PAGESIZE,ifstream::beg);
	char *buffer = new char[PAGESIZE];
	fd.read(buffer,PAGESIZE);
	short slotCounter;
	long slotPointer;
	int slotSize;
	memcpy(&slotCounter,&buffer[PAGESLOTCOUNTER],sizeof(short));
	if(slotCounter == 0)
	{
		cout<<"\n The page does not contain any slots. No information is stored here.....";
		return;
	}
	cout<<"\n Slot ID\tSlot Offset\tSlot Size\tDeleted\tData";
	for(int i=0;i<slotCounter;i++)
	{
		bool deleted = 0;
		memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
		memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));
		if(slotSize<0)
		{
			slotSize = (-1)*slotSize;
			deleted = 1;
		}
		char *databuf = new char[slotSize];
		memcpy(databuf,&buffer[slotPointer],slotSize);

		if(deleted == 1)
			cout<<"\n "<<i+1<<"\t\t"<<slotPointer<<"\t\t"<<slotSize<<"\t\tTrue\t"<<databuf;
		else
			cout<<"\n "<<i+1<<"\t\t"<<slotPointer<<"\t\t"<<slotSize<<"\t\tFalse\t"<<databuf;

		delete databuf;
	}
	cout<<"\n";
	delete buffer;
}

void printAllSlotsInformation(string fileName)
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
	int no_pages = size/PAGESIZE;
	cout<<"\n Page Properties are: ";
	cout<<"\n File name: "<<fileName;
	cout<<"\n No. of pages: "<<no_pages;
	for(int i=1;i<=no_pages;i++)
	{
		cout<<"\n Page No. "<<i<<" Slot Details";
		printSlotsInformation(fileName,i);
	}
}

int insertData(string fileName,int pageNo,string data)
{
	ifstream fd;
	ofstream fd1;
	fd.open(fileName.c_str(),ios::binary);
	fd1.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return -2;
	}
	if(!fd1)
	{
		cerr<<"\n File does not exist.....";
		return -2;
	}
	fd.seekg((pageNo-1)*PAGESIZE,ifstream::beg);
	char *buffer = new char[PAGESIZE];
	fd.read(buffer,PAGESIZE);
	DataPage *d1 = DataPage::getDataPage(buffer);
	int dataSize = data.length();
	short slotCounter = d1->getslotCounter();
	long slotPointer = d1->getcfsPointer();
	int cfs = d1->getcfs();

	// Checking whether data can be inserted into the page
	if(dataSize+SLOTSIZE > cfs)
	{
		cout<<"\n This page is full.....";
		cout<<"\n Writing to a new page.....";
		return -1; // Implement functionality to handle -1 case.......
	}

	// Check if the no. of slots is 0.....
	if(slotCounter == 0)
	{
		cout<<"\n This page is empty.....";
		slotCounter = slotCounter + 1;
		d1->setslotCounter(slotCounter);
		memcpy(&buffer[slotPointer],data.c_str(),dataSize);
		slotPointer += dataSize;
		d1->setcfsPointer(slotPointer);
		cfs -= (dataSize + SLOTSIZE);
		d1->setcfs(cfs);
		memcpy(&buffer[PAGESLOTCOUNTER],&slotCounter,sizeof(short));
		memcpy(&buffer[PAGECFSPOINTER],&slotPointer,sizeof(long));
		memcpy(&buffer[PAGECFSSIZEPOINTER],&cfs,sizeof(int));
	}
	else
	{
		// Check if we can insert in one of the deleted slots.... Using best fit algorithm instead of worst fit......
		bool deletedFlag = 0;
		int bestSize,bestFitSize = 0,position;
		long bestSlotPointer;
		for(int i=0;i<slotCounter;i++)
		{
			memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
			memcpy(&bestSize,&buffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));
			if(bestSize < 0 && (-1)*bestSize >= dataSize)
			{
				if(bestFitSize == 0)
				{
					deletedFlag = 1;
					bestFitSize = bestSize;
					bestSlotPointer = slotPointer;
					position = i;
				}
				else if(bestSize > bestFitSize)
					continue;
				else
				{
					bestFitSize = bestSize;
					bestSlotPointer = slotPointer;
					position = i;
				}
			}
		}

		// Check if there was a deleted slot available for inserting the item.... If not insert at the proper position......
		if(deletedFlag == 1)
		{
			// There is some deleted slot available for insertion....... Insert at that position
			memcpy(&buffer[bestSlotPointer],data.c_str(),dataSize);
			memcpy(&buffer[FIRSTSLOTPTR-(position+1)*sizeof(long)-(position+1)*sizeof(int)],&dataSize,sizeof(int));
		}
		else
		{
			// There are no deleted slots available for insertion..... So, insert at the new slot......
			slotCounter = slotCounter + 1;
			d1->setslotCounter(slotCounter);
			memcpy(&buffer[slotPointer],data.c_str(),dataSize);
			slotPointer += dataSize;
			d1->setcfsPointer(slotPointer);
			cfs -= (dataSize + SLOTSIZE);
			d1->setcfs(cfs);
			memcpy(&buffer[PAGESLOTCOUNTER],&slotCounter,sizeof(short));
			memcpy(&buffer[PAGECFSPOINTER],&slotPointer,sizeof(long));
			memcpy(&buffer[PAGECFSSIZEPOINTER],&cfs,sizeof(int));
		}
	}
	fd1.seekp((pageNo-1)*PAGESIZE,ifstream::beg);
	fd1.write(buffer,PAGESIZE);

	return 1;
}

int deleteData(string fname,string data)
{
	ifstream fd;
	ofstream fd1;
	fd.open(fname.c_str(),ios::binary);
	fd1.open(fname.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return -2;
	}
	if(!fd1)
	{
		cerr<<"\n File does not exist.....";
		return -2;
	}

	fd.seekg(0,ios::end);
	int fileSize = fd.tellg();
	cout<<"\n File size: "<<fileSize;
	int no_Pages = fileSize/PAGESIZE;
	int resCount = 0;
	fd.seekg(0,ios::beg);

	for(int i=1;i<=no_Pages;i++)
	{
		fd.seekg((i-1)*PAGESIZE,ios::beg);
		fd1.seekp((i-1)*PAGESIZE,ios::beg);
		char *buffer = new char [PAGESIZE];
		fd.read(buffer,PAGESIZE);
		DataPage *d1 = DataPage::getDataPage(buffer);
		int dataSize = data.length();
		short slotCounter = d1->getslotCounter();
		//long cfsPointer = d1->getcfsPointer();
		//int cfs = d1->getcfs();

		// Check if slot counter is not zero.... The page is empty.....
		if(slotCounter != 0)
		{
			for(int j=0;j<slotCounter;j++)
			{
				long slotPointer;
				int slotSize;

				memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
				memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));

				if(slotSize < 0)
				{
					// The slot is already deleted....
					continue;
				}

				char *dataBuf = new char [slotSize];
				memcpy(dataBuf,&buffer[slotPointer],slotSize);

				if(data.compare(dataBuf) == 0)
				{
					cout<<"Data found at pointer "<<slotPointer<<" in page "<<i<<" at slot "<<j+1;
					resCount++;
					slotSize = (-1)*slotSize;
					memcpy(&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],&slotSize,sizeof(int));
				}
			}

			fd1.write(buffer,PAGESIZE);
		}
	}

	return resCount;
}

int insertDataPage(string fname,string data)
{
	ifstream fd;
	fd.open(fname.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return -2;
	}

	fd.seekg(0,ios::end);
	int fileSize = fd.tellg();
	cout<<"\n File size: "<<fileSize;
	int no_Pages = fileSize/PAGESIZE;
	int result;

	for(int i=1;i<=fileSize;i++)
	{
		result = insertData(fname,i,data);
		if(result == 1)
			break;
	}

	return 1;
}

int updateData(string fname,string data,string updatedData)
{
	ifstream fd;
	ofstream fd1;
	fd.open(fname.c_str(),ios::binary);
	fd1.open(fname.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n File does not exist.....";
		return -2;
	}
	if(!fd1)
	{
		cerr<<"\n File does not exist.....";
		return -2;
	}

	fd.seekg(0,ios::end);
	int fileSize = fd.tellg();
	cout<<"\n File size: "<<fileSize;
	int no_Pages = fileSize/PAGESIZE;
	int resCount = 0;
	fd.seekg(0,ios::beg);

	for(int i=1;i<=no_Pages;i++)
	{
		fd.seekg((i-1)*PAGESIZE,ios::beg);
		fd1.seekp((i-1)*PAGESIZE,ios::beg);
		char *buffer = new char [PAGESIZE];
		fd.read(buffer,PAGESIZE);
		DataPage *d1 = DataPage::getDataPage(buffer);
		int dataSize = data.length();
		short slotCounter = d1->getslotCounter();

		// Check if slot counter is not zero.... The page is empty.....
		if(slotCounter != 0)
		{
			for(int j=0;j<slotCounter;j++)
			{
				long slotPointer;
				int slotSize;

				memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
				memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));

				if(slotSize < 0)
				{
					// The slot is already deleted....
					continue;
				}

				char *dataBuf = new char [slotSize];
				int resInsert;
				memcpy(dataBuf,&buffer[slotPointer],slotSize);
				// Can be used for in-place modification.... Implementation later... Not now.....
				//bool updateInPlace = FALSE;

				if(data.compare(dataBuf) == 0)
				{
					cout<<"Data found at pointer "<<slotPointer<<" in page "<<i<<" at slot "<<j+1;
					resCount++;
					slotSize = (-1)*slotSize;
					memcpy(&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],&slotSize,sizeof(int));
					fd1.write(buffer,PAGESIZE);
					resInsert = insertDataPage(fname,updatedData);

					if(resInsert == 1)
					{
						// On successful deletion and insertion
						cout<<"Successfully modified the data.......";
					}
					else
					{
						// On successful deletion and no insertion....
						cout<<"For the following update: "<<resCount<<" in page "<<i<<" slot ID: "<<j<<" Update failed....";
					}
					/*if(updatedData.length() == dataSize)
					{
						updateInPlace = TRUE;
						memcpy(&buffer[slotPointer],updatedData,slotSize);
					}*/
				}
			}

		}
	}

	return resCount;
}

void dataPageFunction(int choice)
{
	string fname,data,updatedData;
	int pageNo,result;
	switch(choice)
	{
		case 1: cout<<"\n Creating data page.....";
			cout<<"\n Enter the name of the data file to be created: ";
			cin>>fname;
			createFileName(fname);
			break;
		case 2: cout<<"\n Opening a data page.....";
			cout<<"\n Enter the name of the data file to be opened: ";
			cin>>fname;
			openFileName(fname);
			break;
		case 3: cout<<"\n Displaying the properties of the data page....";
			cout<<"\n Enter the name of the data file: ";
			cin>>fname;
			cout<<"\n Enter the page, for which details are to be displayed: ";
			cin>>pageNo;
			dispPageProperties(fname,pageNo);
			break;
		case 4: cout<<"\n Displaying the page as hex.....";
			cout<<"\n Enter the name of the data file: ";
			cin>>fname;
			cout<<"\n Enter the page, for which hex dump has to be displayed: ";
			cin>>pageNo;
			printhex(fname,pageNo);
			break;
		case 5: cout<<"\n Inserting data into a data page.....";
			cout<<"\n Enter the name of the data file: ";
			cin>>fname;
			cout<<"\n Enter the page no.: ";
			cin>>pageNo;
			cout<<"\n Enter the data to be inserted: ";
			cin>>data;
			result = insertData(fname,pageNo,data);
			cout<<"\n No. of rows modified: "<<result;
			break;
		case 6: cout<<"\n Deleting data from a data page....";
			cout<<"\n Enter the name of the data file: ";
			cin>>fname;
			cout<<"\n Enter the data to be deleted: ";
			cin>>data;
			result = deleteData(fname,data);
			cout<<"\n No. of rows modified: "<<result;
			break;
		case 7: cout<<"\n Listing all the data pages.....";
			cout<<"\n Enter the name of the data file: ";
			cin>>fname;
			dispPageProperties(fname);
			break;
		case 8: cout<<"\n Modifying data in a data page.....";
			cout<<"\n Enter the name of the data file: ";
			cin>>fname;
			cout<<"\n Enter the data to be modified: ";
			cin>>data;
			cout<<"\n Enter the data to be updated: ";
			cin>>updatedData;
			result = updateData(fname,data,updatedData);
			cout<<"\n No. of rows modified: "<<result;
			break;
		case 9: cout<<"\n Printing all the slots information.....";
			cout<<"\n Enter the name of the data file: ";
			cin>>fname;
			printAllSlotsInformation(fname);
			break;
		case 10: cout<<"\n Exiting.....";
			 break;
	}
}

int main()
{
	int choice;
	do
	{
		cout<<"\n Creation of data pages.....";
		cout<<"\n Enter 1 to create a data page.";
		cout<<"\n Enter <no.> to create multiple pages. <not implemented> <From scratch/With datapage file>";
		cout<<"\n Enter 2 to open a data page.";
		cout<<"\n Enter 3 to display the properties of the data page.";
		cout<<"\n Enter 4 to display the page as hex.";
		cout<<"\n Enter 5 to insert data to a Data Page.";
		cout<<"\n Enter 6 to delete data from a Data Page.";
		cout<<"\n Enter 7 to list all the Data Pages.";
		cout<<"\n Enter 8 to modify data in a Data Page.";
		cout<<"\n Enter 9 to print all the slots information.";
		cout<<"\n Enter 10 to exit.";
		cout<<"\n Enter your choice: ";
		cin>>choice;
		cout<<"\n";
		if(choice > 10 || choice <= 0)
		{
			cout<<"\n Invalid option.";
			continue;
		}
		else
			dataPageFunction(choice);
	}while(choice!=10);
	cout<<endl;
}
