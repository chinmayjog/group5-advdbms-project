// Free List.cpp

#include"FreeList.h"

using namespace std;

/*class FreePage
{
	private: int _pageID;
		 short _pagePriority;
		 int _nextFreePage;

		 /*FreePage()
		 {
			_pagePriority = 0;
		 }

		 FreePage(int pageID)
		 {
			_pagePriority = 0;
			_pageID = pageID;
		 }

		 FreePage(int pageID,int nextFreePage)
		 {
			_pagePriority = 0;
			_pageID = pageID;
			_nextFreePage = nextFreePage;
		 }

		 FreePage(char *buffer)
		 {
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextFreePage,&buffer[NEXTPTR],sizeof(int));
		 }

		 ~FreePage()
		 {
			// Destructor for FreePage
		 }

	public: FreePage()
		{
			_pagePriority = 0;
		}

		FreePage(int pageID)
		{
			_pagePriority = 0;
			_pageID = pageID;
		}

		FreePage(int pageID,int nextFreePage)
		{
			_pagePriority = 0;
			_pageID = pageID;
			_nextFreePage = nextFreePage;
		}

		FreePage(char *buffer)
		{
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextFreePage,&buffer[NEXTPTR],sizeof(int));
		}

		~FreePage()
		{
			// Destructor for FreePage
		}*/

		/*static FreePage * getFreePage()
		{
			return new FreePage();
		}

		static FreePage * getFreePage(int pageID)
		{
			return new FreePage(pageID);
		}

		static FreePage * getFreePage(int pageID,int nextFreePage)
		{
			return new FreePage(pageID,nextFreePage);
		}

		static FreePage * getFreePage(char *buffer)
		{
			return new FreePage(buffer);
		}

};*/

int FreePage::getpageID()
{
	return _pageID;
}

void FreePage::setpageID(int pageID)
{
	_pageID = pageID;
}

short FreePage::getpagePriority()
{
	return _pagePriority;
}

void FreePage::setpagePriority(short pagePriority)
{
	_pagePriority = pagePriority;
}

int FreePage::getnextFreePage()
{
	return _nextFreePage;
}

void FreePage::setnextFreePage(int nextFreePage)
{
	this->_nextFreePage = nextFreePage;
}

int FreePage::getpageSize()
{
	return _pageSize;
}

void FreePage::setpageSize(int pageSize)
{
	this->_pageSize = pageSize;
}

void FreePage::writeFreePageBuffer()
{
	char *buffer = new char [PAGESIZE];
	// Chinmay's code to read the buffer

	memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
	memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
	memcpy(&buffer[NEXTPTR],&_nextFreePage,sizeof(int));

	// Chinmay's code to write the buffer

	delete buffer;
}

void FreePage::writeFreePageBuffer(char *buffer)
{
//	int i;                       // I need not do this check with Chinmay first.....
//	for(i=0;i<PAGESIZE;i++)
//	buffer[i]=0;

	memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
	memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
	memcpy(&buffer[NEXTPTR],&_nextFreePage,sizeof(int));

}

/*int FreePage::pageWrite(int _pno,char *buffer)
{
	ofstream fd;
	string fileName = "_dataBaseName"+".dat";//Change it to the database name, when you want to write
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return;
	}

	fd.seekp((_pno-1)*_pageSize,ofstream::beg);
	fd.write(buffer,_pageSize);
}

char * FreePage::pageRead(int _pno)
{
	ifstream fd;
	string fileName = "_dataBaseName"+".dat";//Change it to the database name, when you want to write
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return;
	}

	fd.seekg((_pno-1)*_pageSize,ifstream::beg);
	fd.read(buffer,_pageSize);
}*/
