// Sys Index....

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>

#define PAGESIZE 1024
#define DBROOT "/home/krishna/advdbms/group5-advdbms-project/kptestsample"
#define PAGEIDPTR PAGESIZE-sizeof(int)
#define PAGEPRIPTR PAGEIDPTR-sizeof(short)
#define NEXTPTR PAGEPRIPTR-sizeof(int)

using namespace std;

class SysIndex
{
	private: int _pageID;
		 short _pagePriority;
		 int _nextSysIndexPage;

		 SysIndex()
		 {
			_pagePriority = 50;
			_nextSysIndexPage = 0;
		 }

		 SysIndex(char *buffer)
		 {
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextSysIndexPage,&buffer[NEXTPTR],sizeof(int));
		 }

		 ~SysIndex()
		 {
			// Destructor for SysIndex
		 };

	public: static SysIndex * newSysIndex()
		{
			return new SysIndex();
		}

		static SysIndex * newSysIndex(char *buffer)
		{
			return new SysIndex(buffer);
		}

		int getpageID()
		{
			return _pageID;
		}

		void setpageID(int pageID)
		{
			_pageID = pageID;
		}

		short getpagePriority()
		{
			return _pagePriority;
		}

		void setpagePriority(short pagePriority)
		{
			_pagePriority = pagePriority;
		}

		int getnextSysIndexPage()
		{
			return _nextSysIndexPage;
		}

		void setnextSysIndexPage(int nextSysIndexPage)
		{
			_nextSysIndexPage = nextSysIndexPage;
		}
};
