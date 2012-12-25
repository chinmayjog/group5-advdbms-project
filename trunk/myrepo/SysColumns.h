// Sys Columns....

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>

#include"globalDBEngine.h"

#define CURSYSCOLSIZEPTR (NEXTPTR-sizeof(int))
#define REMSYSCOLSIZEPTR (CURSYSCOLSIZEPTR-sizeof(int))
#define SYSCOLUMNENTRYPTR (REMSYSCOLSIZEPTR-sizeof(long))
#define SYSCOLNOEPTR (SYSCOLUMNENTRYPTR-sizeof(int))
#define SYSCOLUMNMETADATASIZE (sizeof(int)+sizeof(short)+sizeof(int)+sizeof(int)+sizeof(int)+sizeof(long)+sizeof(int))
#define SYSCOLUMNENTRYSPACE PAGESIZE-(SYSCOLUMNMETADATASIZE)
#define SYSCOLUMNENTRYSIZE (64+64+64+sizeof(int)+256+sizeof(char)+8+sizeof(int)+sizeof(short)+sizeof(char)+256+sizeof(short)+sizeof(char)+sizeof(char)+sizeof(char))

#define FIRSTSYSCOLSLOTPTR (SYSCOLNOEPTR-sizeof(char))
#define SYSCOLSLOTSIZE (sizeof(char))

#define SYSCOLTABLENAMEPTR 0
#define SYSCOLDBNAMEPTR (SYSCOLTABLENAMEPTR+64)
#define SYSCOLCOLNAMEPTR (SYSCOLDBNAMEPTR+64)
#define SYSCOLORDPOSPTR (SYSCOLCOLNAMEPTR+64)
#define SYSCOLCOLDEFPTR (SYSCOLORDPOSPTR+sizeof(int))
#define SYSCOLISNULLPTR (SYSCOLCOLDEFPTR+256)
#define SYSCOLDATATYPEPTR (SYSCOLISNULLPTR+sizeof(char))
#define SYSCOLLENPTR (SYSCOLDATATYPEPTR+8)
#define SYSCOLSCALEPTR (SYSCOLLENPTR+sizeof(int))
#define SYSCOLUPDATABLEPTR (SYSCOLSCALEPTR+sizeof(short))
#define SYSCOLREMARKSPTR (SYSCOLUPDATABLEPTR+sizeof(char))
#define SYSCOLKEYPOSPTR (SYSCOLREMARKSPTR+256)
#define SYSCOLFORKEYPTR (SYSCOLKEYPOSPTR+sizeof(short))
#define SYSCOLGENATTRPTR (SYSCOLFORKEYPTR+sizeof(char))
#define SYSCOLHIDDENPTR (SYSCOLGENATTRPTR+sizeof(char))

using namespace std;

class SysColumnsEntry
{
	private: string _dbName; // Name of the database 64 bytes long
		 string _tableName; // Name of the table 64 bytes long
		 string _columnName; // Name of the column 64 bytes long
		 int _ordinalPosition; // Position of the column in the row
		 string _columnDefault; // Default value // Length has to be specified.... 256 bytes long
		 char _isNullable; // Indicates whether the column can have NULL values or not
		 string _dataType; // Data Type of the column // 8 bytes long
		 //string _columnType; // Column type
		 // string _columnKey; // If it is part of a key or not Redundant because of _keyPos variable
		 int _length; // Length of the column
		 short _scale; // Scale of the column - Needed for precision.... Decimal, Timestamp with/wo Time zone
		 char _updatable; // Whether the column is updatable or not
		 string _remarks; // Remarks for the column.... 256 bytes long
		 short _keyPos; // Position in the key.... 0 if not part of primary key
		 char _foreignKey; // Whether it is a foreign key or not
		 char _generatedAttribute; // A if generated always or D if generated by Default value or blank
		 char _hidden; // If its hidden or not when select * query is run
		 //int _cID; // Not using this as of now

		 static int _colID;


	public: SysColumnsEntry()
		{
			_dbName = "Database Name";
			_tableName = "Table Name";
			_columnName = "Column Name";
			_ordinalPosition = -1;
			_columnDefault = "########################";
			_isNullable = 'Y';
			_dataType = "\0";
			_length = -1;
			_scale = 0;
			_updatable = 'Y';
			_remarks ="\0";
			_keyPos = 0;
			_foreignKey = 'N';
			_generatedAttribute = ' ';
			_hidden = 'N';
		}

		~SysColumnsEntry()
		{
			// Destructor for SysColumnsEntry
		}

		static SysColumnsEntry * getSysColumnsEntry()
		{
			return new SysColumnsEntry();
		}

		static int next_sysColumnsEntry_id()
		{
			_colID++;
			return _colID;
		}

		string getTableName();
		void setTableName(string _tableName);
		string getDBName();
		void setDBName(string _dbName);
		string getColumnName();
		void setColumnName(string _colName);
		string getColumnDefault();
		void setColumnDefault(string _colDefault);
		string getDataType();
		void setDataType(string _dataType);
		string getRemarks();
		void setRemarks(string _remarks);
		char getIsNullable();
		void setIsNullable(char _isNullable);
		char getUpdatable();
		void setUpdatable(char _updatable);
		char getForeignKey();
		void setForeignKey(char _foreignKey);
		char getGeneratedAttribute();
		void setGeneratedAttribute(char _generatedAttribute);
		char getHidden();
		void setHidden(char _hidden);
		short getScale();
		void setScale(short _scale);
		short getKeyPos();
		void setKeyPos(short _keyPos);
		int getOrdinalPosition();
		void setOrdinalPosition(int _ordinalPosition);
		int getLength();
		void setLength(int _length);

		int fillBuffer(char *buffer);
		int getDataBuffer(char *buffer);
};

//int SysColumnsEntry::_colID = 0;

class SysColumns
{
	private: int _pageID;
		 short _pagePriority;
		 int _nextSysColumnsPage;
		 int _curSysColumnsSize; // Check this is not required... This can be got by METADATASIZE + noe*COLUMNENTRYSIZE
		 int _remSysColumnsSize;
		 long _sysColumnsEntryPointer;
		 int _noOfEntries;
		 int _pageSize;

	public: SysColumns()
		{
			_pagePriority = 50;
			_nextSysColumnsPage = -1;
			_pageID = 3;
			_curSysColumnsSize = SYSCOLUMNMETADATASIZE;
			_remSysColumnsSize = SYSCOLUMNENTRYSPACE;
			_sysColumnsEntryPointer = 0;
			_noOfEntries = 0;
		}

		SysColumns(int pageID)
		{
			_pagePriority = 50;
			_nextSysColumnsPage = -1;
			_pageID = pageID;
			_curSysColumnsSize = SYSCOLUMNMETADATASIZE;
			_remSysColumnsSize = SYSCOLUMNENTRYSPACE;
			_sysColumnsEntryPointer = 0;
			_noOfEntries = 0;
		}

		SysColumns(int pageID,int pageSize)
		{
			_pagePriority = 50;
			_pageSize = pageSize;
			_nextSysColumnsPage = -1;
			_pageID = pageID;
			_curSysColumnsSize = SYSCOLUMNMETADATASIZE;
			_remSysColumnsSize = SYSCOLUMNENTRYSPACE;
			_sysColumnsEntryPointer = 0;
			_noOfEntries = 0;
		}

		SysColumns(char *buffer)
		{
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextSysColumnsPage,&buffer[NEXTPTR],sizeof(int));
			memcpy(&_curSysColumnsSize,&buffer[CURSYSCOLSIZEPTR],sizeof(int));
			memcpy(&_remSysColumnsSize,&buffer[REMSYSCOLSIZEPTR],sizeof(int));
			memcpy(&_sysColumnsEntryPointer,&buffer[SYSCOLUMNENTRYPTR],sizeof(long));
			memcpy(&_noOfEntries,&buffer[SYSCOLNOEPTR],sizeof(int));
		}

		SysColumns(char *buffer,int pageSize)
		{
			_pageSize = pageSize;
			memcpy(&_pageID,&buffer[PAGEIDPTR],sizeof(int));
			memcpy(&_pagePriority,&buffer[PAGEPRIPTR],sizeof(short));
			memcpy(&_nextSysColumnsPage,&buffer[NEXTPTR],sizeof(int));
			memcpy(&_curSysColumnsSize,&buffer[CURSYSCOLSIZEPTR],sizeof(int));
			memcpy(&_remSysColumnsSize,&buffer[REMSYSCOLSIZEPTR],sizeof(int));
			memcpy(&_sysColumnsEntryPointer,&buffer[SYSCOLUMNENTRYPTR],sizeof(long));
			memcpy(&_noOfEntries,&buffer[SYSCOLNOEPTR],sizeof(int));
		}

		~SysColumns()
		{
			// Destructor for SysColumns
		};


		/*static SysColumns * newSysColumns()
		{
			return new SysColumns();
		}

		static SysColumns * newSysColumns(int pageID)
		{
			return new SysColumns(pageID);
		}

		static SysColumns * newSysColumns(char *buffer)
		{
			return new SysColumns(buffer);
		}*/

		int getpageID();
		void setpageID(int pageID);
		short getpagePriority();
		void setpagePriority(short pagePriority);
		int getnextSysColumnsPage();
		void setnextSysColumnsPage(int nextSysColumnsPage);
		int getCurSysColumnsSize();
		void setCurSysColumnsSize(int curSysColumnsSize);
		int getRemSysColumnsSize();
		void setRemSysColumnsSize(int remSysColumnsSize);
		long getSysColumnsEntryPointer();
		void setSysColumnsEntryPointer(long sysColumnsEntryPointer);
		int getNoOfEntries();
		void setNoOfEntries(int noOfEntries);
		int getPageSize();
		void setPageSize(int pageSize);
		void writeSysColumnsBuffer(char *buffer);
//		int writeSysColumnsBuffer(char *buffer);
		bool checkEntryInsertion();
		int createNewSysColumnEntry(char *entryBuffer,char * sysColumnBuffer);
		int searchSysColumnEntry(string columnName,char * sysColumnBuffer);
		// Check if I can write it as a list of SysColumnEntry objects instead of passing the buffer around.......
		int deleteSysColumnEntry(string columnName,char * sysColumnBuffer);
};
