//Database header....

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>

#ifndef dbHeaderA
#define DBHEADERPAGESIZE 8192
#endif
#ifndef dbHeaderB
//#define DBROOT "/home/krishna/advdbms/group5-advdbms-project/kptestsample"
#define DBROOT "./"
#endif
#define FREEPAGEPTR 0
#define SYSTABLEPTR FREEPAGEPTR+sizeof(int)
#define SYSCOLUMNPTR SYSTABLEPTR+sizeof(int)
#define SYSINDEXPTR SYSCOLUMNPTR+sizeof(int)
#define TOTDBSIZEPTR SYSINDEXPTR+sizeof(int)
#define FILEPATHPTR TOTDBSIZEPTR+sizeof(int)
#define PAGESIZEPTR FILEPATHPTR+256 // Check this once.... _filePath.length() Not using this because DBHEADER is static table
#define FREELISTPTR PAGESIZEPTR+sizeof(int)
#define DBNAMEPTR FREELISTPTR+sizeof(int)
#define PAGEPRIORITYPTR DBNAMEPTR+64 // Check this once.... _dataBaseName.length() Not using this because DBHEADER is a static table
#define TOTPAGEPTR PAGEPRIORITYPTR+sizeof(short)

using namespace std;

class DBHeader
{
	private: int _noFreePages;
		 int _sysTables; // This has to be a page no.
		 int _sysColumns; // This has to be a page no.
		 int _sysIndex; // This has to be a page no.
		 //DirectoryPage *_directoryPage; // This has to be a page no.//This should not be here..... This should be part of entry in SysTables....
		 int _totalDBSize;
		 string _filePath;
		 int _pageSize;
		 int _freePtr; // This has to be a page no.
		 string _dataBaseName;
		 short _priority;
		 int _totalPages;
		 int _pageID;
		 //int _curLTableID;

	public: DBHeader()
		{
			_sysTables = 2;
			_sysColumns = 3;
			_sysIndex = 4;
			//_directoryPage = NULL;
			_totalDBSize = 1024*1024*100;
			_filePath = "File Path";
			_pageSize = DBHEADERPAGESIZE;
			_freePtr = 5;
			_dataBaseName = "Database Name";
			_priority = 50;
			_totalPages = _totalDBSize/_pageSize;
			_noFreePages = _totalPages - 4;
			//_curLTableID = 0;
		}

		DBHeader(char *buffer)
		{
			char * filePathBuf = new char [256];
			char * dbNameBuf = new char [64];

			memcpy(&_noFreePages,&buffer[FREEPAGEPTR],sizeof(int));//memcpy operation here
			memcpy(&_sysTables,&buffer[SYSTABLEPTR],sizeof(int));//memcpy operation here
			memcpy(&_sysColumns,&buffer[SYSCOLUMNPTR],sizeof(int));//memcpy operation here
			memcpy(&_sysIndex,&buffer[SYSINDEXPTR],sizeof(int));//memcpy operation here
			//_directoryPage = NULL;//memcpy operation here
			memcpy(&_totalDBSize,&buffer[TOTDBSIZEPTR],sizeof(int));//_totalDBSize = 1024*1024*100;
			memcpy(filePathBuf,&buffer[FILEPATHPTR],256);//memcpy operation here
			memcpy(&_pageSize,&buffer[PAGESIZEPTR],sizeof(int));//memcpy operation here
			memcpy(&_freePtr,&buffer[FREELISTPTR],sizeof(int));//memcpy operation here
			memcpy(dbNameBuf,&buffer[DBNAMEPTR],64);//memcpy operation here
			memcpy(&_priority,&buffer[PAGEPRIORITYPTR],sizeof(short));//memcpy operation here
			_totalPages = _totalDBSize/_pageSize;

			for(int i=0;i<64;i++)
			{
				if(dbNameBuf[i] == '$')
					break;
				_dataBaseName = _dataBaseName+dbNameBuf[i];
			}
			for(int i=0;i<256;i++)
			{
				if(filePathBuf[i] == '$')
					break;
				_filePath = _filePath+filePathBuf[i];
			}

			delete dbNameBuf;
			delete filePathBuf;
		}

		DBHeader(int pageSize,string dataBaseName)
		{
			_sysTables = 2;
			_sysColumns = 3;
			_sysIndex = 4;
			//_directoryPage = NULL;
			_totalDBSize = 1024*1024*100;
			_filePath = DBROOT;
			//_filePath = _filePath+"/";
			_filePath = _filePath+dataBaseName;
			_filePath = _filePath+".dat";
			_pageSize = pageSize;
			_freePtr = 5;
			_dataBaseName = dataBaseName;
			_priority = 50;
			_totalPages = _totalDBSize/_pageSize;
			_noFreePages = _totalPages - 4;
		}

		DBHeader(int size,int pageSize,string dataBaseName)
		{
			_sysTables = 2;
			_sysColumns = 3;
			_sysIndex = 4;
			//_directoryPage = NULL;
			_totalDBSize = size;
			_filePath = DBROOT;
			cout<<_filePath;
			//_filePath = _filePath+"/";
			_filePath = _filePath+dataBaseName;
			cout<<_filePath;
			_filePath = _filePath+".dat";
			cout<<_filePath;
			_pageSize = pageSize;
			_freePtr = 5;
			_dataBaseName = dataBaseName;
			_priority = 50;
			_totalPages = _totalDBSize/_pageSize;
			_noFreePages = _totalPages - 4;
		}

		~DBHeader()
		{
			// Destructor for DBHeader
		};

		int getNoFreePages();
		void setNoFreePages(int _noFreePages);
		int getSysTablesPointer();
		void setSysTablesPointer(int _sysTables);
		int getSysColumnsPointer();
		void setSysColumnsPointer(int _sysColumns);
		int getSysIndexPointer();
		void setSysIndexPointer(int _sysIndex);
//		DirectoryPage * getDirectoryPagePointer();
//		void setDirectoryPagePointer(DirectoryPage *_directoryPage);
		int getTotalDBSize();
		void setTotalDBSize(int _totalDBSize);
		string getFilePath();
		void setFilePath(string _filePath);
		int getPageSize();
		void setPageSize(int _pageSize);
		int getFreePagePointer();
		void setFreePagePointer(int _freePtr);
		string getDatabaseName();
		void setDatabaseName(string _dataBaseName);
		int getTotalPages();
		void setTotalPages(int _totalPages);
		/*static DBHeader * getDBHeader()
		{
			return new DBHeader();
		}

		static DBHeader * getDBHeader(char *buffer)
		{
			return new DBHeader(char *buffer);
		}

		static DBHeader * getDBHeader(int pageSize,string dataBaseName)
		{
			return new DBHeader(pageSize,dataBaseName);
		}

		static DBHeader * getDBHeader(int size,int pageSize,string dataBaseName)
		{
			return new DBHeader(size,pageSize,dataBaseName);
		}*/

		void writeDBHeader(char *buffer);
//		int pageWrite(int _pno,char *buffer);
//		char * pageRead(int _pno);
};
