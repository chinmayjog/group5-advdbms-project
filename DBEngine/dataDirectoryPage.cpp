// Directory page for data page

#include "dataDirectoryPage.h"

/*class DirectoryPageEntry
{
	int _pageID;
	int _cfs;

	DirectoryPageEntry()
	{
		_pageID = -1;
		_cfs = -1;
	}

	DirectoryPageEntry(int pageID)
	{
		_pageID = pageID;
		_cfs = ;
	}

	~DirectoryPageEntry()
	{
		// Destructor for DirectoryPageEntry()
	}
}

class DirectoryPage
{
	private: int _nextDirectoryPage;
		 int _noe;
		 long _maxtfs;
//		 DE_STRUCT _de[1];

	public:	DirectoryPage()
		{
			_nextDirectoryPage = -1;
			_noe = 0;
			_maxtfs = DEPAGESIZE;
		}

		DirectoryPage(char *buffer)
		{
			memcpy(&_nextDirectoryPage,&buffer[NEXT_PTR],sizeof(long));
			memcpy(&_noe,&buffer[NOE_PTR],sizeof(int));
			memcpy(&_maxtfs,&buffer[MAXTFS_PTR],sizeof(long));
//			memcpy(&_de[1],&buffer[DE_PTR],sizeof(DE_STRUCT));
		}

		~DirectoryPage()
		{
			//Destructor for DirectoryPage object
		};

		static DirectoryPage * getDirectoryPage()
		{
			return new DirectoryPage();
		}

		static DirectoryPage * getDirectoryPage(char *buffer)
		{
			return new DirectoryPage(buffer);
		}
};*/

int DirectoryPageEntry::getPageID()
{
	return _pageID;
}

void DirectoryPageEntry::setPageID(int pageID)
{
	_pageID = pageID;
}

int DirectoryPageEntry::getTFS()
{
	return _tfs;
}

void DirectoryPageEntry::setTFS(int tfs)
{
	_tfs = tfs;
}

void DirectoryPageEntry::fillBuffer(char *buffer)
{
	memcpy(&buffer[0],&_pageID,sizeof(int));
	memcpy(&buffer[4],&_tfs,sizeof(int));
}

bool DirectoryPageEntry::checkTFS(int dataSize)
{
	if(_tfs > dataSize + DPSLOTSIZE)
		return 1;
	else
		return 0;
}

int DirectoryPage::getNextDirectoryPagePointer()
{
	return _nextDirectoryPage;
}

void DirectoryPage::setNextDirectoryPagePointer(int _nextDirectoryPage)
{
	this->_nextDirectoryPage = _nextDirectoryPage;
}

int DirectoryPage::getNoE()
{
	return _noe;
}

void DirectoryPage::setNoE(int _noe)
{
	this->_noe = _noe;
}

int DirectoryPage::getPageSize()
{
	return _pageSize;
}

void DirectoryPage::setPageSize(int _pageSize)
{
	this->_pageSize = _pageSize;
}

int DirectoryPage::getMaxTFS()
{
	return _maxtfs;
}

void DirectoryPage::setMaxTFS(int _maxtfs)
{
	this->_maxtfs = _maxtfs;
}

int DirectoryPage::getRemDirectoryPageSize()
{
	return _remDirectoryPageSize;
}

void DirectoryPage::setRemDirectoryPageSize(int _remDirectoryPageSize)
{
	this->_remDirectoryPageSize = _remDirectoryPageSize;
}

int DirectoryPage::getpageID()
{
	return _pageID;
}

void DirectoryPage::setpageID(int pageID)
{
	_pageID = pageID;
}

short DirectoryPage::getpagePriority()
{
	return _pagePriority;
}

void DirectoryPage::setpagePriority(short pagePriority)
{
	_pagePriority = pagePriority;
}

long DirectoryPage::getdePTR()
{
	return _dePTR;
}

void DirectoryPage::setdePTR(long dePTR)
{
	_dePTR = dePTR;
}

void DirectoryPage::fillBuffer(char *buffer)
{
	memcpy(&buffer[PAGEID_PTR],&_pageID,sizeof(int));
	memcpy(&buffer[PAGEPRIORITY_PTR],&_pagePriority,sizeof(short));
	memcpy(&buffer[NEXT_PTR],&_nextDirectoryPage,sizeof(int));
	memcpy(&buffer[NOE_PTR],&_noe,sizeof(int));
	memcpy(&buffer[MAXTFS_PTR],&_maxtfs,sizeof(int));
	memcpy(&buffer[REMSIZE_PTR],&_remDirectoryPageSize,sizeof(int));
	memcpy(&buffer[NEXTDE_PTR],&_dePTR,sizeof(long));
}

bool DirectoryPage::checkDirectoryEntryInsertion()
{
	if(_remDirectoryPageSize > DESIZE + DESLOTSIZE)
		return 1;
	else
		return 0;
}

int DirectoryPage::insertDirectoryPageEntry(char *entryBuffer,char *directoryBuffer)
{
	// Sanity check for insertion of entry.....
	if(checkDirectoryEntryInsertion() == 0)
	{
		cout<<"Entry cannot be inserted... Create new directory page..."<<endl;
		return -1;
	}
	else
	{
		// Check for deleted slots first...
		// Not implementing it.....
		// Decision is let the slots remain.....
		/*if(_noe != 0)
		{
			// If you find it, insert in the slot, and update the bool flag
			for(int i=0;i<_noe;i++)
			{
				char deleted;
				memcpy(&deleted,&directoryBuffer[FIRSTDESLOTPTR-((i)*sizeof(char))],sizeof(char));
				if(deleted == '0')
				{
					memcpy(&directoryBuffer[0+(i*DESIZE)],entryBuffer,DESIZE);
					deleted = '1';
					memcpy(&directoryBuffer[FIRSTDESLOTPTR-((i)*sizeof(char))],&deleted,sizeof(char));
					int curMaxTFS = _maxtfs;
					for(int i=0;i<_noe;i++)
					{
						int entryTFS;
						memcpy(&entryTFS,&directoryBuffer[0+i*DESIZE+sizeof(int)],sizeof(int));
						if(entryTFS > curMaxTFS)
							curMaxTFS = entryTFS;
					}
					return 1;
				}
			}
		}*/

		// No deleted slots... Create a new slot and insert the data there....

		// Decrease the remaining size.
		_remDirectoryPageSize = _remDirectoryPageSize - DESIZE - DESLOTSIZE;
		memcpy(&directoryBuffer[REMSIZE_PTR],&_remDirectoryPageSize,sizeof(int));
		// Copy the entryBuffer contents to the pointer location
		memcpy(&directoryBuffer[_dePTR],entryBuffer,DESIZE);
		// Update the directory entry pointer
		_dePTR = _dePTR + DESIZE;
		memcpy(&directoryBuffer[NEXTDE_PTR],&_dePTR,sizeof(long));
		// Increment the no. of entries
		_noe = _noe + 1;
		memcpy(&directoryBuffer[NOE_PTR],&_noe,sizeof(int));
		// Update the max_tfs value
		int curMaxTFS = _maxtfs;
		for(int i=0;i<_noe;i++)
		{
			int entryTFS;
			memcpy(&entryTFS,&directoryBuffer[0+i*DESIZE+sizeof(int)],sizeof(int));
			if(entryTFS > curMaxTFS)
				curMaxTFS = entryTFS;
		}
		char inserted = '1';
		if(_noe - 1 == 0)
			memcpy(&directoryBuffer[FIRSTDESLOTPTR],&inserted,sizeof(char));
		else
			memcpy(&directoryBuffer[FIRSTDESLOTPTR-((_noe-1)*sizeof(char))],&inserted,sizeof(char));
		if(curMaxTFS != _maxtfs)
			memcpy(&directoryBuffer[MAXTFS_PTR],&curMaxTFS,sizeof(int));
		return 1;
	}
}

int DirectoryPage::deleteDirectoryPageEntry(int dataPageNo,char *directoryBuffer)
{
	int i;
	bool deletedFlag = 0;
	char alreadyDeleted;
	for(i=0;i<_noe;i++)
	{
		memcpy(&alreadyDeleted,&directoryBuffer[FIRSTDESLOTPTR-((i)*sizeof(char))],sizeof(char));
		if(alreadyDeleted == '0')
		{
			cout<<"Entry deleted..... Do not check there..."<<endl;
			continue;
		}
		char *entryBuffer = new char [DESIZE];
		memcpy(entryBuffer,&directoryBuffer[0+i*DESIZE],DESIZE);
		int dpNo;
		memcpy(&dpNo,entryBuffer,sizeof(int));
		delete entryBuffer;
		if(dpNo == dataPageNo)
		{
			// Check already deleted.....
			deletedFlag = 1;
			
			// Set the boolean flag of the slot to 0
			char inserted = '0';
			int location = (i)*sizeof(char);
			memcpy(&directoryBuffer[FIRSTDESLOTPTR-location],&inserted,sizeof(char));
			// Decrement the no. of entries....
			//_noe = _noe - 1;
			//memcpy(&directoryBuffer[NOE_PTR],&_noe,sizeof(int));
			break;
		}
	}
	if(deletedFlag == 1)
	{
		cout<<"The page no. to be deleted was here.... So, do not check further pages...."<<endl;
		return i+1;
	}
	else
	{
		cout<<"The page no. to be deleted is not in this Directory Page.... Continue searching....."<<endl;
		return -1;
	}
}

int DirectoryPage::updateDirectoryPageEntry(int dataPageNo,int tfsValue,char *directoryBuffer)
{
	int i;
	bool updateFlag = 0;
	char alreadyDeleted;
	for(i=0;i<_noe;i++)
	{
		memcpy(&alreadyDeleted,&directoryBuffer[FIRSTDESLOTPTR-((i)*sizeof(char))],sizeof(char));
		if(alreadyDeleted == '0')
		{
			cout<<"Entry deleted..... Do not check there..."<<endl;
			continue;
		}
		char *entryBuffer = new char [DESIZE];
		memcpy(entryBuffer,&directoryBuffer[0+i*DESIZE],DESIZE);
		int dpNo;
		memcpy(&dpNo,entryBuffer,sizeof(int));
		if(dpNo == dataPageNo)
		{
			updateFlag = 1;
			memcpy(&entryBuffer[4],&tfsValue,sizeof(int));
			memcpy(&directoryBuffer[0+i*DESIZE],entryBuffer,DESIZE);
			delete entryBuffer;
			break;
		}
		delete entryBuffer;
	}
	if(updateFlag == 1)
	{
		cout<<"The page no. to be updated was here.... So, do not check further pages...."<<endl;
		return i+1;
	}
	else
	{
		cout<<"The page no. to be updated is not in this Directory Page.... Continue searching....."<<endl;
		return -1;
	}
}
int DirectoryPage::searchDirectoryPageEntry(int dataPageNo,char *directoryBuffer)
{
	int i;
	bool searchFlag = 0;
	char alreadyDeleted;
	for(i=0;i<_noe;i++)
	{
		memcpy(&alreadyDeleted,&directoryBuffer[FIRSTDESLOTPTR-((i)*sizeof(char))],sizeof(char));
		if(alreadyDeleted == '0')
		{
			cout<<"Entry deleted..... Do not check there..."<<endl;
			continue;
		}
		char *entryBuffer = new char [DESIZE];
		memcpy(entryBuffer,&directoryBuffer[0+i*DESIZE],DESIZE);
		int dpNo;
		memcpy(&dpNo,entryBuffer,sizeof(int));
		delete entryBuffer;
		if(dpNo == dataPageNo)
		{
			searchFlag = 1;
			break;
		}
	}
	if(searchFlag == 1)
	{
		cout<<"The page no. to be searched was here.... So, do not check further pages...."<<endl;
		return i+1;
	}
	else
	{
		cout<<"The page no. to be searched is not in this Directory Page.... Continue searching....."<<endl;
		return -1;
	}
}

void DirectoryPage::printSlotsInformation(char *buffer)
{
	/*char * buffer = new char [DEPAGESIZE];
	fillBuffer(buffer);*/
	for(int i=0;i<_noe;i++)
	{
		//bool flag;
		char deleted;
		memcpy(&deleted,&buffer[FIRSTDESLOTPTR-(i*sizeof(char))],sizeof(char));
		char *entryBuffer = new char [DESIZE];
		memcpy(entryBuffer,&buffer[0+i*DESIZE],DESIZE);
		DirectoryPageEntry *dirPageEntry = new DirectoryPageEntry(entryBuffer);
		if(deleted == '1')
			cout<<dirPageEntry->getPageID()<<"\t\t"<<dirPageEntry->getTFS()<<"\t\t"<<"False"<<endl;
		else
			cout<<dirPageEntry->getPageID()<<"\t\t"<<dirPageEntry->getTFS()<<"\t\t"<<"True"<<endl;
		delete dirPageEntry;
		delete entryBuffer;
	}
	/*delete buffer;*/
}

bool DirectoryPage::checkMaxTFS(int dataSize)
{
	if(_maxtfs < dataSize + DPSLOTSIZE)
		return 0;
	return 1;
}

int DirectoryPage::updateMaxTFS(char *buffer)
{
	int tfs=0;
	for(int i=0;i<_noe;i++)
	{
		//bool flag;
		char deleted;
		memcpy(&deleted,&buffer[FIRSTDESLOTPTR-(i*sizeof(char))],sizeof(char));
		char *entryBuffer = new char [DESIZE];
		memcpy(entryBuffer,&buffer[0+i*DESIZE],DESIZE);
		DirectoryPageEntry *dirPageEntry = new DirectoryPageEntry(entryBuffer);
		if(deleted == '1' && tfs < dirPageEntry->getTFS())
			tfs = dirPageEntry->getTFS();
		delete dirPageEntry;
		delete entryBuffer;
	}
	_maxtfs = tfs;
	memcpy(&buffer[MAXTFS_PTR],&_maxtfs,sizeof(int));
}
