// Sys Tables....

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#include<math.h>

#include"globalDBEngine.h"

#define CURSYSTABSIZEPTR (NEXTPTR-sizeof(int))
#define REMSYSTABSIZEPTR (CURSYSTABSIZEPTR-sizeof(int))
#define SYSTABLEENTRYPTR (REMSYSTABSIZEPTR-sizeof(long))
#define SYSTABNOEPTR (SYSTABLEENTRYPTR-sizeof(int))
#define SYSTABLEMETADATASIZE (sizeof(int)+sizeof(short)+sizeof(int)+sizeof(int)+sizeof(int)+sizeof(long)+sizeof(int))
#define SYSTABLEENTRYSPACE PAGESIZE-(SYSTABLEMETADATASIZE)
#define SYSTABLEENTRYSIZE (64+64+sizeof(char)+sizeof(short)+sizeof(char)+sizeof(int)+sizeof(int)+256+sizeof(short)+sizeof(short)+sizeof(short)+sizeof(char)+sizeof(int)+sizeof(int))

#define FIRSTSYSTABSLOTPTR (SYSTABNOEPTR-sizeof(char))
#define SYSTABSLOTSIZE (sizeof(char))

#define SYSTABTABLENAMEPTR 0
#define SYSTABDBNAMEPTR (SYSTABTABLENAMEPTR+64*sizeof(char))
#define SYSTABTABLETYPEPTR (SYSTABDBNAMEPTR+64*sizeof(char))
#define SYSTABCOLCOUNTPTR (SYSTABTABLETYPEPTR+sizeof(char))
#define SYSTABROWFORMATPTR (SYSTABCOLCOUNTPTR+sizeof(short))
#define SYSTABTABLEROWSPTR (SYSTABROWFORMATPTR+sizeof(char))
#define SYSTABNOPAGESPTR (SYSTABTABLEROWSPTR+sizeof(int))
#define SYSTABREMARKSPTR (SYSTABNOPAGESPTR+sizeof(int))
#define SYSTABKEYCOLSPTR (SYSTABREMARKSPTR+256*sizeof(char))
#define SYSTABDATALENGTHPTR (SYSTABKEYCOLSPTR+sizeof(short))
#define SYSTABRECLENGTHPTR (SYSTABDATALENGTHPTR+sizeof(short))
#define SYSTABENCODINGSCHEMEPTR (SYSTABRECLENGTHPTR+sizeof(short))
#define SYSTABAVGROWLENGTHPTR (SYSTABENCODINGSCHEMEPTR+sizeof(char))
#define SYSTABDIRPAGEPTR (SYSTABAVGROWLENGTHPTR+sizeof(int))

using namespace std;

class SysTableEntry
{
	private: string _tableName; // Name of the table...//Limit 64
		 string _dbName; // Name of the database....//Limit 64
		 char _typeTable; // Type of table - T -> Table; V -> View
		 unsigned short _colCount; // No. of columns
		 char _rowFormat; // Format of rows - D -> Dynamic; F -> Fixed
		 int _tableRows; // Total no. of rows
		 int _noPages; // Total no. of pages in which the table appears
		 string _remarks; // Comment statement//Limit 256
		 unsigned short _keyColumns; // No. of columns in the primary key of the table - Not implemented now
		 unsigned short _dataLength; // Data length of the record.....
		 unsigned short _recLength; // Avg. length of the record.......
		 char _encodingScheme; // Encoding scheme for tables - E -> EBCDIC; A -> ASCII; M -> Multiple CCSID sets; U -> UNICODE
		 int _avgRowLength; // Average row length... If table is compressed - value is compressed row length; Else - value is uncompressed row length
		 int _directoryPagePointer; // Pointer to the first directory page containing the table
		 //int _tID;

	//	 static int _tableID;

	public: SysTableEntry()
		{
			//strcpy(_tableName,"Table Name");
			_tableName = "Table Name";
			//strcpy(_dbName,"Database Name");
			_dbName = "Database Name";
			_typeTable = 'T';
			_colCount = 0;
			_rowFormat = 'F';
			_tableRows = 0;
			_noPages = 0;
			//_remarks[0] = '\0';
			_remarks = "\0";
			_keyColumns = 0;
			_dataLength = 0;
			_recLength = 0;
			_encodingScheme = 'A';
			_avgRowLength = 0;
			_directoryPagePointer = -1;
		}

		~SysTableEntry()
		{
			// Destructor for SysTableEntry object
		}
		
	/*	static int next_sysTableEntry_id()
		{
			_tableID++;
			return _tableID;
		}*/

		/*static SysTableEntry * newSysTableEntry()
		{
			return new SysTableEntry();
		}*/

		string getTableName();
		void setTableName(string _tableName);
		string getDBName();
		void setDBName(string _dbName);
		string getRemarks();
		void setRemarks(string _remarks);
		char getTableType();
		void setTableType(char _typeTable);
		char getRowFormat();
		void setRowFormat(char _rowFormat);
		char getEncodingScheme();
		void setEncodingScheme(char _encodingScheme);
		unsigned short getColCount();
		void setColCount(unsigned short _colCount);
		unsigned short getKeyColumns();
		void setKeyColumns(unsigned short _keyColumns);
		unsigned short getRecLength();
		void setRecLength(unsigned short _recLength);
		unsigned short getDataLength();
		void setDataLength(unsigned short _dataLength);
		int getTableRows();
		void setTableRows(int _tableRows);
		int getNoPages();
		void setNoPages(int _noPages);
		int getAvgRowLength();
		void setAvgRowLength(int _avgRowLength);
		int getDirectoryPagePointer();
		void setDirectoryPagePointer(int _directoryPagePointer);
//		int getTID();
//		void setTID();
		void deleteSysTableEntry(SysTableEntry * _sysTableEntry);
		int fillBuffer(char *buffer);
		int getDataBuffer(char *buffer);
};

//int SysTableEntry::_tableID = 0;
// How to implement this in persistent phase....... so that across launches it uses proper table ids......

class SysTables
{
	private: int _pageID;
		 short _pagePriority;
		 int _nextSysTablePage;
		 int _curSysTableSize; // Check this is not required... This can be got by METADATASIZE + noe*TABLEENTRYSIZE
		 int _remSysTableSize;
		 long _sysTableEntryPointer;
		 int _noOfEntries;
		 int _pageSize; // This variable is not used anywhere by this class.... This is used instead of having a global variable pageSize.... Set this variable immediately after the object creation....
		 

	public: SysTables()
		{
			_pageSize = 8192;
			_pagePriority = 50;
			_nextSysTablePage = -1;
			_curSysTableSize = SYSTABLEMETADATASIZE;
			_remSysTableSize = SYSTABLEENTRYSPACE;
			_sysTableEntryPointer = 0;
			_noOfEntries = 0;
			_pageID = 1;
		}

		SysTables(int pageID)
		{
			_pageSize = 8192;
			_pagePriority = 50;
			_nextSysTablePage = -1;
			_curSysTableSize = SYSTABLEMETADATASIZE;
			_remSysTableSize = SYSTABLEENTRYSPACE;
			_sysTableEntryPointer = 0;
			_noOfEntries = 0;
			_pageID = pageID;
		}

		SysTables(int pageID,int pageSize)
		{
			_pageSize = pageSize;
			_pagePriority = 50;
			_nextSysTablePage = -1;
			_curSysTableSize = SYSTABLEMETADATASIZE;
			_remSysTableSize = SYSTABLEENTRYSPACE;
			_sysTableEntryPointer = 0;
			_noOfEntries = 0;
			_pageID = pageID;
		}

		SysTables(char *buffer)
		{
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextSysTablePage,&buffer[NEXTPTR],sizeof(int));
			memcpy(&_curSysTableSize,&buffer[CURSYSTABSIZEPTR],sizeof(int));
			memcpy(&_remSysTableSize,&buffer[REMSYSTABSIZEPTR],sizeof(int));
			memcpy(&_sysTableEntryPointer,&buffer[SYSTABLEENTRYPTR],sizeof(long));
			memcpy(&_noOfEntries,&buffer[SYSTABNOEPTR],sizeof(int));
		}

		SysTables(char *buffer,int pageSize)
		{
			_pageSize = pageSize;
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextSysTablePage,&buffer[NEXTPTR],sizeof(int));
			memcpy(&_curSysTableSize,&buffer[CURSYSTABSIZEPTR],sizeof(int));
			memcpy(&_remSysTableSize,&buffer[REMSYSTABSIZEPTR],sizeof(int));
			memcpy(&_sysTableEntryPointer,&buffer[SYSTABLEENTRYPTR],sizeof(long));
			memcpy(&_noOfEntries,&buffer[SYSTABNOEPTR],sizeof(int));
		}

		~SysTables()
		{
			// Destructor for SysTables
		}


		static SysTables * newSysTables()
		{
			return new SysTables();
		}

		static SysTables * newSysTables(int pageID)
		{
			return new SysTables(pageID);
		}

		static SysTables * newSysTables(char *buffer)
		{
			return new SysTables(buffer);
		}

		int getpageID();
		void setpageID(int pageID);
		short getpagePriority();
		void setpagePriority(short pagePriority);
		int getnextSysTablePage();
		void setnextSysTablePage(int nextSysTablePage);
		int getCurSysTableSize();
		void setCurSysTableSize(int curSysTableSize);
		int getRemSysTableSize();
		void setRemSysTableSize(int remSysTableSize);
		long getSysTableEntryPointer();
		void setSysTableEntryPointer(long sysTableEntryPointer);
		int getNoOfEntries();
		void setNoOfEntries(int noOfEntries);
		int getPageSize();
		void setPageSize(int pageSize);
		void writeSysTableBuffer(char *buffer);
//		int writeSysTableBuffer(char *buffer);
		bool checkEntryInsertion();
		int createNewSysTableEntry(char *entryBuffer,char *sysTableBuffer);
		int searchSysTableEntry(string tableName,char * sysTableBuffer);
		int searchSysTableEntry(string tableName,string dbName,char * sysTableBuffer);
		// Check if I can write it as a list of SysTableEntry objects instead of passing the buffer around.......
		int deleteSysTableEntry(string tabName,char * sysTableBuffer);
		int deleteSysTableEntry(string tabName,string dbName,char * sysTableBuffer);

		bool writeLog(string message)
		{
			ofstream myfile;
			myfile.open (DEBUGFILENAME);
			myfile << "From SysTables: "<<message<<endl;
			myfile.close();
		}
};
