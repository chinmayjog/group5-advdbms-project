// Free List.cpp

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>

#include"globalDBEngine.h"

class FreePage
{
	private: int _pageID;
		 short _pagePriority;
		 int _nextFreePage;
		 int _pageSize;

	public: FreePage()
		{
			_pagePriority = 0;
			_pageSize = 8192;
		}

		FreePage(int pageID,int pageSize)
		{
			_pagePriority = 0;
			_pageID = pageID;
			_pageSize = pageSize;
		}

		FreePage(int pageID,int nextFreePage,int pageSize)
		{
			_pagePriority = 0;
			_pageID = pageID;
			_nextFreePage = nextFreePage;
			_pageSize = pageSize;
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

		int getpageID();
		void setpageID(int pageID);
		short getpagePriority();
		void setpagePriority(short pagePriority);
		int getnextFreePage();
		void setnextFreePage(int nextFreePage);
		int getpageSize();
		void setpageSize(int pageSize);
		void writeFreePageBuffer();
		void writeFreePageBuffer(char *buffer);
		//int pageWrite(int _pno,char *buffer);
		//char * pageRead(int _pno);

};
