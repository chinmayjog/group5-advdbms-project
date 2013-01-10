// CPP file for data page.......

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#include<stdlib.h>

//#define FALSE 0
//#define TRUE 1
//#define PAGESIZE 8192

#include "globalDBEngine.h"

#define PAGESLOTCOUNTER (NEXTPTR-sizeof(short))
#define PAGECFSSIZEPOINTER (PAGESLOTCOUNTER-sizeof(int))
#define PAGECFSPOINTER (PAGECFSSIZEPOINTER-sizeof(long))

#define DATAPAGEMETADATA (sizeof(int)+sizeof(short)+sizeof(int)+sizeof(short)+sizeof(int)+sizeof(long))

#define FIRSTSLOTPTR PAGECFSPOINTER
#define LASTSLOTPTR (FIRSTSLOTPTR-(_slotCounter-1)*sizeof(long)-(_slotCounter-1)*sizeof(int))

using namespace std;

class DataPage
{
	private: int _cfs;
		 long _cfsPointer;
		 short _slotCounter;
		 int _pageSize;
		 int _pageID;
		 short _pagePriority;
		 int _nextDataPagePTR;
		 //char *buffer;

	public: DataPage()
		{
			//buffer = new char[PAGESIZE];
			_pageSize = 8192;
		 	_cfs = _pageSize-DATAPAGEMETADATA;
			_cfsPointer = 0;
			_slotCounter = 0;
			_pageID = -1;
			_pagePriority = 1;
			_nextDataPagePTR = -1;
			//memcpy(&buffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
			//memcpy(&buffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));
			//memcpy(&buffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		}

		DataPage(int pageSize,int pageID)
		{
			//buffer = new char[PAGESIZE];
			_pageSize = pageSize;
		 	_cfs = _pageSize-DATAPAGEMETADATA;
			_cfsPointer = 0;
			_slotCounter = 0;
			_pageID = pageID;
			_pagePriority = 1;
			_nextDataPagePTR = -1;
			//memcpy(&buffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
			//memcpy(&buffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));
			//memcpy(&buffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		}

		DataPage(char *buffer)
		{
			//this->buffer = new char[PAGESIZE];
			//memcpy(&(this->buffer),&buffer,sizeof(buffer));
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextDataPagePTR,&buffer[NEXTPTR],sizeof(int));
			memcpy(&_slotCounter,&buffer[PAGESLOTCOUNTER],sizeof(short));
			memcpy(&_cfs,&buffer[PAGECFSSIZEPOINTER],sizeof(int));
			memcpy(&_cfsPointer,&buffer[PAGECFSPOINTER],sizeof(long));
		}

		DataPage(char *buffer,int pageSize)
		{
			//this->buffer = new char[PAGESIZE];
			//memcpy(&(this->buffer),&buffer,sizeof(buffer));
			_pageSize = pageSize;
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextDataPagePTR,&buffer[NEXTPTR],sizeof(int));
			memcpy(&_slotCounter,&buffer[PAGESLOTCOUNTER],sizeof(short));
			memcpy(&_cfs,&buffer[PAGECFSSIZEPOINTER],sizeof(int));
			memcpy(&_cfsPointer,&buffer[PAGECFSPOINTER],sizeof(long));
		}

		~DataPage()
		{
			//delete[] buffer;
		}

		/*static DataPage * getDataPage()
		{
			return new DataPage();
		}
		static DataPage * getDataPage(char *buffer)
		{
			return new DataPage(buffer);
		}*/

		int getcfs();
		void setcfs(int cfs);
		long getcfsPointer();
		void setcfsPointer(long cfsPointer);
		short getslotCounter();
		void setslotCounter(short slotCounter);
		int getPageSize();
		void setPageSize(int pageSize);
		int getPageID();
		void setPageID(int pageID);
		short getPagePriority();
		void setPagePriority(short pagePriority);
		int getNextDataPagePTR();
		void setNextDataPagePTR(int nextDataPagePTR);

		void fillBuffer(char *buffer);
		void fillDataBuf(char *buffer,long slotPointer,int slotSize,char *dataBuf);
		bool checkCFS(int dataSize);

		int insertData(char *dataPageBuffer,char *insertData,int dataSize);
		int insertData(char *dataPageBuffer,char *insertData,int dataSize,int *InsertedSlotID);
		// It is better to add data types as comparing data as char * does not yield proper results...
		int deleteData(char *dataPageBuffer,char *deleteData,int dataPos,short dataType);
		int deleteData(char *dataPageBuffer,char *deleteData,int dataPos,short dataType,int *InsertedSlotID);
		int searchData(char *dataPageBuffer,char *searchData,int dataPos,short dataType);
		// Use this update function only for fixed length updates not for data types like varchar....
		int updateData(char *dataPageBuffer,char *oldData,char *newData,int dataPos,short dataType);
		int updateData(char *dataPageBuffer,char *oldData,char *newData,int dataPos,short dataType,int *InsertedSlotID);
};
