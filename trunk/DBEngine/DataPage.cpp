// CPP file for data page.......

#include"DataPage.h"

#include"DataTypes.h"

/*
class DataPage
{
	private: int _cfs;
		 long _cfsPointer;
		 short _slotCounter;
		 //char *buffer;

	public: DataPage()
		{
			//buffer = new char[PAGESIZE];
		 	_cfs = DATAPAGEMETADATA-SLOTSIZE;
			_cfsPointer = 0;
			_slotCounter = 0;
			//memcpy(&buffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
			//memcpy(&buffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));
			//memcpy(&buffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		}

		DataPage(char *buffer)
		{
			//this->buffer = new char[PAGESIZE];
			//memcpy(&(this->buffer),&buffer,sizeof(buffer));
			memcpy(&_slotCounter,&buffer[PAGESLOTCOUNTER],sizeof(short));
			memcpy(&_cfs,&buffer[PAGECFSSIZEPOINTER],sizeof(int));
			memcpy(&_cfsPointer,&buffer[PAGECFSPOINTER],sizeof(long));
		}

		~DataPage()
		{
			//delete[] buffer;
		}

		static DataPage * getDataPage()
		{
			return new DataPage();
		}

		static DataPage * getDataPage(char *buffer)
		{
			return new DataPage(buffer);
		}

};*/

int DataPage::getcfs()
{
	return _cfs;
}

void DataPage::setcfs(int cfs)
{
	_cfs = cfs;
}

long DataPage::getcfsPointer()
{
	return _cfsPointer;
}

void DataPage::setcfsPointer(long cfsPointer)
{
	_cfsPointer = cfsPointer;
}

short DataPage::getslotCounter()
{
	return _slotCounter;
}

void DataPage::setslotCounter(short slotCounter)
{
	_slotCounter = slotCounter;
}

int DataPage::getPageSize()
{
	return _pageSize;
}

void DataPage::setPageSize(int pageSize)
{
	_pageSize = pageSize;
}

int DataPage::getPageID()
{
	return _pageID;
}

void DataPage::setPageID(int pageID)
{
	_pageID = pageID;
}

short DataPage::getPagePriority()
{
	return _pagePriority;
}

void DataPage::setPagePriority(short pagePriority)
{
	_pagePriority = pagePriority;
}

int DataPage::getNextDataPagePTR()
{
	return _nextDataPagePTR;
}

void DataPage::setNextDataPagePTR(int nextDataPagePTR)
{
	_nextDataPagePTR = nextDataPagePTR;
}

void DataPage::fillBuffer(char *buffer)
{
	memcpy(&buffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
	memcpy(&buffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));
	memcpy(&buffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
}

void DataPage::fillDataBuf(char *buffer,long slotPointer,int slotSize,char *dataBuf)
{
	memcpy(dataBuf,&buffer[slotPointer],slotSize);
}

bool DataPage::checkCFS(int dataSize)
{
	if(_cfs > dataSize + DPSLOTSIZE)
		return 1;
	else
		return 0;
}

int DataPage::insertData(char *dataPageBuffer,char *insertData,int dataSize,int *InsertedSlotID)
{
	// Check if the no. of slots is 0.....
	if(_slotCounter == 0)
	{
		//cout<<"\n This page is empty.....";
		_slotCounter = _slotCounter + 1;
		*InsertedSlotID = _slotCounter;
		memcpy(&dataPageBuffer[_cfsPointer],insertData,dataSize);
		memcpy(&dataPageBuffer[FIRSTSLOTPTR-sizeof(long)],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[FIRSTSLOTPTR-sizeof(long)-sizeof(int)],&dataSize,sizeof(int));

		_cfsPointer += dataSize;
		_cfs -= (dataSize + DPSLOTSIZE);
		memcpy(&dataPageBuffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
		memcpy(&dataPageBuffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));

		return 1; // The data was inserted successfully.....
	}
	// Check for deleted slots first and then, try creating a new slot for insertion
	// Best-fit algorithm to find the best slot for inserting the data....
	bool deletedFlag = 0;
	int bestSize,bestFitSize = 0,position;
	long bestSlotPointer;
	long slotPointer;
	for(int i=0;i<_slotCounter;i++)
	{
		memcpy(&slotPointer,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
		memcpy(&bestSize,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));
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
		memcpy(&dataPageBuffer[bestSlotPointer],insertData,dataSize);
		memcpy(&dataPageBuffer[FIRSTSLOTPTR-(position+1)*sizeof(long)-(position+1)*sizeof(int)],&dataSize,sizeof(int));
		*InsertedSlotID = position+1;
		return 2; // The data was inserted in a deleted slot....
	}
	// Now check for contiguous free space, and insert there, otherwise decide on defragmentation process......
	if(checkCFS(dataSize) == 0)
	{
		// Decide whether to do defragmentation process now or delay it to some other time
		// Alternatives - No defragmentation now. Set the lastDataPageTried to this data page.
		// This one keeps gets updated. If we have to create a new Data page, then, we can insert in the
		// last data page after defragmenting it.
		// Otherwise, no defragmentation, create a new data page and insert it there
		return -1;
	}
	else
	{
		_slotCounter = _slotCounter + 1;
		*InsertedSlotID = _slotCounter;
		memcpy(&dataPageBuffer[_cfsPointer],insertData,dataSize);
		memcpy(&dataPageBuffer[LASTSLOTPTR-sizeof(long)],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[LASTSLOTPTR-sizeof(long)-sizeof(int)],&dataSize,sizeof(int));

		_cfsPointer += dataSize;
		_cfs -= (dataSize + DPSLOTSIZE);
		memcpy(&dataPageBuffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
		memcpy(&dataPageBuffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));
		return 3; // The data was inserted successfully after a new slot was created.....
	}
}

int DataPage::insertData(char *dataPageBuffer,char *insertData,int dataSize)
{
	// Check if the no. of slots is 0.....
	if(_slotCounter == 0)
	{
		//cout<<"\n This page is empty.....";
		_slotCounter = _slotCounter + 1;

		memcpy(&dataPageBuffer[_cfsPointer],insertData,dataSize);
		memcpy(&dataPageBuffer[FIRSTSLOTPTR-sizeof(long)],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[FIRSTSLOTPTR-sizeof(long)-sizeof(int)],&dataSize,sizeof(int));

		_cfsPointer += dataSize;
		_cfs -= (dataSize + DPSLOTSIZE);
		memcpy(&dataPageBuffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
		memcpy(&dataPageBuffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));

		return 1; // The data was inserted successfully.....
	}
	// Check for deleted slots first and then, try creating a new slot for insertion
	// Best-fit algorithm to find the best slot for inserting the data....
	bool deletedFlag = 0;
	int bestSize,bestFitSize = 0,position;
	long bestSlotPointer;
	long slotPointer;
	for(int i=0;i<_slotCounter;i++)
	{
		memcpy(&slotPointer,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
		memcpy(&bestSize,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));
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
		memcpy(&dataPageBuffer[bestSlotPointer],insertData,dataSize);
		memcpy(&dataPageBuffer[FIRSTSLOTPTR-(position+1)*sizeof(long)-(position+1)*sizeof(int)],&dataSize,sizeof(int));
		return 2; // The data was inserted in a deleted slot....
	}
	// Now check for contiguous free space, and insert there, otherwise decide on defragmentation process......
	if(checkCFS(dataSize) == 0)
	{
		// Decide whether to do defragmentation process now or delay it to some other time
		// Alternatives - No defragmentation now. Set the lastDataPageTried to this data page.
		// This one keeps gets updated. If we have to create a new Data page, then, we can insert in the
		// last data page after defragmenting it.
		// Otherwise, no defragmentation, create a new data page and insert it there
		return -1;
	}
	else
	{
		_slotCounter = _slotCounter + 1;
		memcpy(&dataPageBuffer[_cfsPointer],insertData,dataSize);
		memcpy(&dataPageBuffer[LASTSLOTPTR-sizeof(long)],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[LASTSLOTPTR-sizeof(long)-sizeof(int)],&dataSize,sizeof(int));

		_cfsPointer += dataSize;
		_cfs -= (dataSize + DPSLOTSIZE);
		memcpy(&dataPageBuffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
		memcpy(&dataPageBuffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		memcpy(&dataPageBuffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));
		return 3; // The data was inserted successfully after a new slot was created.....
	}
}

int DataPage::deleteData(char *dataPageBuffer,char *deleteData,int dataPos,short dataType)
{
	// Function returns the total no. of records which matched the data and were deleted.....
	if(_slotCounter == 0)
	{
		// Nothing to delete.....
		// For sanity check.....
		return -3;
	}
	int i,j,resCount=0;
	for(i=0;i<_slotCounter;i++)
	{
		long slotPointer;
		int slotSize;

		memcpy(&slotPointer,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
		memcpy(&slotSize,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));

		if(slotSize < 0)
		{
			// Slot has been deleted
			// So no need to check
			continue;
		}

		char * dataBuf = new char [slotSize];
		memcpy(dataBuf,&dataPageBuffer[slotPointer],slotSize);

		short offsetPointer=0,dataPointer=2,offset;

		for(j=0;j<dataPos;j++)
		{
			memcpy(&offset,&dataBuf[offsetPointer],sizeof(short));
			if(j == dataPos-1)
			{
				// This is the position at which data has to be verified....
				char * posData = new char [offset];
				memcpy(posData,&dataBuf[dataPointer],offset);
				if(/*strcmp(posData,deleteData)==0*/dataCompare(posData,deleteData,dataType)==0)
				{
					cout<<"Data found at slot ID: "<<i<<" ";
					cout<<"Deleting it....."<<endl;
					slotSize = (-1)*slotSize;
					resCount++;
					memcpy(&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],&slotSize,sizeof(int));
				}
				delete posData;
			}
			offsetPointer = dataPointer+offset;
			dataPointer = offsetPointer+sizeof(short);
		}

		delete dataBuf;
	}
	return resCount;
}

int DataPage::searchData(char *dataPageBuffer,char *searchData,int dataPos,short dataType)
{
	// Function returns the total no. of records which matched the data....
	if(_slotCounter == 0)
	{
		// Nothing to search.....
		// For sanity check.....
		return -3;
	}
	int i,j,resCount=0;
	for(i=0;i<_slotCounter;i++)
	{
		long slotPointer;
		int slotSize;

		memcpy(&slotPointer,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
		memcpy(&slotSize,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));

		if(slotSize < 0)
		{
			// Slot has been deleted
			// So no need to check
			continue;
		}

		char * dataBuf = new char [slotSize];
		memcpy(dataBuf,&dataPageBuffer[slotPointer],slotSize);

		short offsetPointer=0,dataPointer=2,offset;

		for(j=0;j<dataPos;j++)
		{
			memcpy(&offset,&dataBuf[offsetPointer],sizeof(short));
			if(j == dataPos-1)
			{
				// This is the position at which data has to be verified....
				char * posData = new char [offset];
				memcpy(posData,&dataBuf[dataPointer],offset);
				if(/*strcmp(posData,searchData)==0*/dataCompare(posData,searchData,dataType)==0)
				{
					cout<<"Data found at slot ID: "<<i<<endl;
					resCount++;
				}
				delete posData;
			}
			offsetPointer = dataPointer+offset;
			dataPointer = offsetPointer+sizeof(short);
		}

		delete dataBuf;
	}
	return resCount;
}

int DataPage::updateData(char *dataPageBuffer,char *oldData,char *newData,int dataPos,short dataType)
{
	// Function returns the total no. of records which matched the data and were updated.....
	// New data integrity check has to be done by the caller... No check will be done here
	if(_slotCounter == 0)
	{
		// Nothing to update.....
		// For sanity check.....
		return -3;
	}
	int i,j,resCount=0;
	for(i=0;i<_slotCounter;i++)
	{
		long slotPointer;
		int slotSize;

		memcpy(&slotPointer,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
		memcpy(&slotSize,&dataPageBuffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));

		if(slotSize < 0)
		{
			// Slot has been deleted
			// So no need to check
			continue;
		}

		char * dataBuf = new char [slotSize];
		memcpy(dataBuf,&dataPageBuffer[slotPointer],slotSize);

		short offsetPointer=0,dataPointer=2,offset;

		for(j=0;j<dataPos;j++)
		{
			memcpy(&offset,&dataBuf[offsetPointer],sizeof(short));
			if(j == dataPos-1)
			{
				// This is the position at which data has to be verified....
				char * posData = new char [offset];
				memcpy(posData,&dataBuf[dataPointer],offset);
				if(/*strcmp(posData,oldData)==0*/dataCompare(posData,oldData,dataType)==0)
				{
					cout<<"Data found at slot ID: "<<i;
					resCount++;
					cout<<"Updating the data....";
					memcpy(&dataBuf[dataPointer],newData,offset);
					cout<<endl;
				}
				delete posData;
			}
			offsetPointer = dataPointer+offset;
			dataPointer = offsetPointer+sizeof(short);
		}

		delete dataBuf;
	}
	return resCount;
}
