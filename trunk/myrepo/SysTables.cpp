// Sys Tables....

#include "SysTables.h"

//using namespace std;

/*class SysTableEntry
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

		 static int _tableID;

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
		
		static int next_sysTableEntry_id()
		{
			_tableID++;
			return _tableID;
		}

//		static SysTableEntry * newSysTableEntry()
//		{
//			return new SysTableEntry();
//		}
};*/


string SysTableEntry::getTableName()
{
	return _tableName;
}

void SysTableEntry::setTableName(string _tableName)
{
	this->_tableName = _tableName;
}

string SysTableEntry::getDBName()
{
	return _dbName;
}

void SysTableEntry::setDBName(string _dbName)
{
	this->_dbName = _dbName;
}

string SysTableEntry::getRemarks()
{
	return _remarks;
}

void SysTableEntry::setRemarks(string _remarks)
{
	this->_remarks = _remarks;
}

char SysTableEntry::getTableType()
{
	return _typeTable;
}

void SysTableEntry::setTableType(char _typeTable)
{
	this->_typeTable = _typeTable;
}

char SysTableEntry::getRowFormat()
{
	return _rowFormat;
}

void SysTableEntry::setRowFormat(char _rowFormat)
{
	this->_rowFormat = _rowFormat;
}

char SysTableEntry::getEncodingScheme()
{
	return _encodingScheme;
}

void SysTableEntry::setEncodingScheme(char _encodingScheme)
{
	this->_encodingScheme = _encodingScheme;
}

unsigned short SysTableEntry::getColCount()
{
	return _colCount;
}

void SysTableEntry::setColCount(unsigned short _colCount)
{
	this->_colCount = _colCount;
}

unsigned short SysTableEntry::getKeyColumns()
{
	return _keyColumns;
}

void SysTableEntry::setKeyColumns(unsigned short _keyColumns)
{
	this->_keyColumns = _keyColumns;
}

unsigned short SysTableEntry::getRecLength()
{
	return _recLength;
}

void SysTableEntry::setRecLength(unsigned short _recLength)
{
	this->_recLength = _recLength;
}

unsigned short SysTableEntry::getDataLength()
{
	return _dataLength;
}

void SysTableEntry::setDataLength(unsigned short _dataLength)
{
	this->_dataLength = _dataLength;
}

int SysTableEntry::getTableRows()
{
	return _tableRows;
}

void SysTableEntry::setTableRows(int _tableRows)
{
	this->_tableRows = _tableRows;
}

int SysTableEntry::getNoPages()
{
	return _noPages;
}

void SysTableEntry::setNoPages(int _noPages)
{
	this->_noPages = _noPages;
}

int SysTableEntry::getAvgRowLength()
{
	return _avgRowLength;
}

void SysTableEntry::setAvgRowLength(int _avgRowLength)
{
	this->_avgRowLength = _avgRowLength;
}

int SysTableEntry::getDirectoryPagePointer()
{
	return _directoryPagePointer;
}

void SysTableEntry::setDirectoryPagePointer(int _directoryPagePointer)
{
	this->_directoryPagePointer = _directoryPagePointer;
}

/*int SysTableEntry::getTID()
{
	return _tID;
}

void SysTableEntry::setTID()
{
	this->_tID = next_sysTableEntry_id();
}*/

void SysTableEntry::deleteSysTableEntry(SysTableEntry * _sysTableEntry)
{
	delete _sysTableEntry;
}

int SysTableEntry::fillBuffer(char *buffer)
{
	char * tabNameBuf = new char [64];
	char * dbNameBuf = new char [64];
	char * remarksBuf = new char [256];

	for(int i = 0;i<64;i++)
	{
		tabNameBuf[i] = '$';
		dbNameBuf[i] = '$';
	}
	for(int i = 0;i<256;i++)
		remarksBuf[i] = '$';

	//const char * tabNameBuf1 = _tableName.c_str();
	//const char * dbNameBuf1 = _dbName.c_str();
	//const char * remarksBuf1 = _remarks.c_str();

	int len = _tableName.length();

	for(int i = 0;i<len;i++)
		tabNameBuf[i] = _tableName[i];
	len = _dbName.length();
	for(int i = 0;i<len;i++)
		dbNameBuf[i] = _dbName[i];
	len = _remarks.length();
	for(int i = 0;i<len;i++)
		remarksBuf[i] = _remarks[i];

	memcpy(&buffer[SYSTABTABLENAMEPTR],tabNameBuf,64);
	memcpy(&buffer[SYSTABDBNAMEPTR],dbNameBuf,64);
	memcpy(&buffer[SYSTABTABLETYPEPTR],&_typeTable,sizeof(char));
	memcpy(&buffer[SYSTABCOLCOUNTPTR],&_colCount,sizeof(unsigned short));
	memcpy(&buffer[SYSTABROWFORMATPTR],&_rowFormat,sizeof(char));
	memcpy(&buffer[SYSTABTABLEROWSPTR],&_tableRows,sizeof(int));
	memcpy(&buffer[SYSTABNOPAGESPTR],&_noPages,sizeof(int));
	memcpy(&buffer[SYSTABREMARKSPTR],remarksBuf,256);
	memcpy(&buffer[SYSTABKEYCOLSPTR],&_keyColumns,sizeof(unsigned short));
	memcpy(&buffer[SYSTABDATALENGTHPTR],&_dataLength,sizeof(unsigned short));
	memcpy(&buffer[SYSTABRECLENGTHPTR],&_recLength,sizeof(unsigned short));
	memcpy(&buffer[SYSTABENCODINGSCHEMEPTR],&_encodingScheme,sizeof(char));
	memcpy(&buffer[SYSTABAVGROWLENGTHPTR],&_avgRowLength,sizeof(int));
	memcpy(&buffer[SYSTABDIRPAGEPTR],&_directoryPagePointer,sizeof(int));

	delete tabNameBuf;
	delete dbNameBuf;
	delete remarksBuf;

	return 1;
}

int SysTableEntry::getDataBuffer(char *buffer)
{
	char * tabNameBuf = new char [64];
	char * dbNameBuf = new char [64];
	char * remarksBuf = new char [256];

	memcpy(tabNameBuf,&buffer[SYSTABTABLENAMEPTR],64);
	memcpy(dbNameBuf,&buffer[SYSTABDBNAMEPTR],64);
	memcpy(&_typeTable,&buffer[SYSTABTABLETYPEPTR],sizeof(char));
	memcpy(&_colCount,&buffer[SYSTABCOLCOUNTPTR],sizeof(unsigned short));
	memcpy(&_rowFormat,&buffer[SYSTABROWFORMATPTR],sizeof(char));
	memcpy(&_tableRows,&buffer[SYSTABTABLEROWSPTR],sizeof(int));
	memcpy(&_noPages,&buffer[SYSTABNOPAGESPTR],sizeof(int));
	memcpy(remarksBuf,&buffer[SYSTABREMARKSPTR],256);
	memcpy(&_keyColumns,&buffer[SYSTABKEYCOLSPTR],sizeof(unsigned short));
	memcpy(&_dataLength,&buffer[SYSTABDATALENGTHPTR],sizeof(unsigned short));
	memcpy(&_recLength,&buffer[SYSTABRECLENGTHPTR],sizeof(unsigned short));
	memcpy(&_encodingScheme,&buffer[SYSTABENCODINGSCHEMEPTR],sizeof(char));
	memcpy(&_avgRowLength,&buffer[SYSTABAVGROWLENGTHPTR],sizeof(int));
	memcpy(&_directoryPagePointer,&buffer[SYSTABDIRPAGEPTR],sizeof(int));

	for(int i=0;i<64;i++)
	{
		if(tabNameBuf[i] == '$')
			break;
		_tableName = _tableName+tabNameBuf[i];
	}
	for(int i=0;i<64;i++)
	{
		if(dbNameBuf[i] == '$')
			break;
		_dbName = _dbName+dbNameBuf[i];
	}
	for(int i=0;i<256;i++)
	{
		if(remarksBuf[i] == '$')
			break;
		_remarks = _remarks+remarksBuf[i];
	}

	delete tabNameBuf;
	delete dbNameBuf;
	delete remarksBuf;
}

/*class SysTables
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
			_pagePriority = 50;
			_nextSysTablePage = -1;
			_curSysTableSize = SYSTABLEMETADATASIZE;
			_remSysTableSize = SYSTABLEENTRYSPACE;
			_sysTableEntryPointer = 0;
			_noOfEntries = 0;
			_pageID = 2;
		}

		SysTables(int pageID)
		{
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

};*/

int SysTables::getpageID()
{
	return _pageID;
}

void SysTables::setpageID(int pageID)
{
	_pageID = pageID;
}

short SysTables::getpagePriority()
{
	return _pagePriority;
}

void SysTables::setpagePriority(short pagePriority)
{
	_pagePriority = pagePriority;
}

int SysTables::getnextSysTablePage()
{
	return _nextSysTablePage;
}

void SysTables::setnextSysTablePage(int nextSysTablePage)
{
	_nextSysTablePage = nextSysTablePage;
}

int SysTables::getCurSysTableSize()
{
	return _curSysTableSize;
}

void SysTables::setCurSysTableSize(int curSysTableSize)
{
	_curSysTableSize = curSysTableSize;
}

int SysTables::getRemSysTableSize()
{
	return _remSysTableSize;
}

void SysTables::setRemSysTableSize(int remSysTableSize)
{
	_remSysTableSize = remSysTableSize;
}

long SysTables::getSysTableEntryPointer()
{
	return _sysTableEntryPointer;
}

void SysTables::setSysTableEntryPointer(long sysTableEntryPointer)
{
	_sysTableEntryPointer = sysTableEntryPointer;
}

int SysTables::getNoOfEntries()
{
	return _noOfEntries;
}

void SysTables::setNoOfEntries(int noOfEntries)
{
	_noOfEntries = noOfEntries;
}

int SysTables::getPageSize()
{
	return _pageSize;
}

void SysTables::setPageSize(int pageSize)
{
	_pageSize = pageSize;
}

void SysTables::writeSysTableBuffer(char *buffer)
{
	//char *buffer = new char [_pageSize];
	int result;

	/*result = pageRead(_pageID,buffer); // From Chinmay, get buffer and page from cache........
	if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}*/

	// The buffer will be part of the function call, fill it, the caller will take care of reading or writing

	// Copying contents to buffer.....
	cout<<"PageID PTR: "<<PAGEIDPTR<<" Value: "<<_pageID<<endl;
	memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
	cout<<"PagePriority PTR: "<<PAGEPRIPTR<<" Value: "<<_pagePriority<<endl;
	memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
	cout<<"Next PTR: "<<NEXTPTR<<" Value: "<<_nextSysTablePage<<endl;
	memcpy(&buffer[NEXTPTR],&_nextSysTablePage,sizeof(int));
	cout<<"CurrentSysTabSize PTR: "<<" Value: "<<_curSysTableSize<<endl;
	memcpy(&buffer[CURSYSTABSIZEPTR],&_curSysTableSize,sizeof(int));
	cout<<"RemSysTabSize PTR: "<<" Value: "<<_remSysTableSize<<endl;
	memcpy(&buffer[REMSYSTABSIZEPTR],&_remSysTableSize,sizeof(int));
	cout<<"SysTabEntry PTR: "<<SYSTABLEENTRYPTR<<" Value: "<<_sysTableEntryPointer<<endl;
	memcpy(&buffer[SYSTABLEENTRYPTR],&_sysTableEntryPointer,sizeof(long));
	cout<<"NoOfEnt PTR: "<<SYSTABNOEPTR<<" Value: "<<_noOfEntries<<endl;
	memcpy(&buffer[SYSTABNOEPTR],&_noOfEntries,sizeof(int));

	// WriteBuffer.... Chinmay's function call here.....
	//result = pageWrite(_pageID,buffer);

	/*if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}*/

	// All writes will be taken care of by the calling function
	return;
}

/*int SysTables::writeSysTableBuffer(char *buffer)
{
	// WriteBuffer.... Chinmay's function call here.....
	// After inserting a new entry this function is called.....

	int result = pageWrite(_pageID,buffer);

	if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}
	return _pageSize;
}*/

bool SysTables::checkEntryInsertion()
{
	if(SYSTABLEENTRYSIZE + SYSTABSLOTSIZE > _remSysTableSize)
		return 0;
	else
		return 1;
}

int SysTables::createNewSysTableEntry(char *entryBuffer,char *sysTableBuffer)
{
	if(checkEntryInsertion() == 0)
		return -1; // SysTable entry cannot be inserted, have to create a new SysTablePage and then add the entry
	// The above if condition is not required as, the calling method can check the size directly....
	else
	{
		//SysTableEntry *sysTableEntry = new SysTableEntry();
		// Updating the SysTable sizes... Current SysTableSize increases by EntrySize and RemainingSize decreases by same amount

		//char * curSysTableBuff = new char [_pageSize];

		//int result = pageRead(_pageID,curSysTableBuff); // From Chinmay, get buffer and page from cache........
		/*if(result == -1)
		{
			cout<<"Page could not be read...."<<endl;
			return -1;
		}*/

		// Write code to check for insertion in deleted slots....

		memcpy(&sysTableBuffer[_sysTableEntryPointer],entryBuffer,SYSTABLEENTRYSIZE);

				
		_curSysTableSize = _curSysTableSize + SYSTABLEENTRYSIZE + SYSTABSLOTSIZE;
		_remSysTableSize = _remSysTableSize - SYSTABLEENTRYSIZE - SYSTABSLOTSIZE;
		_noOfEntries = _noOfEntries + 1;
		_sysTableEntryPointer = _sysTableEntryPointer + SYSTABLEENTRYSIZE;

		memcpy(&sysTableBuffer[CURSYSTABSIZEPTR],&_curSysTableSize,sizeof(int));
		memcpy(&sysTableBuffer[REMSYSTABSIZEPTR],&_remSysTableSize,sizeof(int));
		memcpy(&sysTableBuffer[SYSTABNOEPTR],&_noOfEntries,sizeof(int));
		memcpy(&sysTableBuffer[SYSTABLEENTRYPTR],&_sysTableEntryPointer,sizeof(long));
		// Write SysTable entry to the buffer of this SysTablePage

		/*result = pageWrite(_pageID,curSysTableBuff);

		if(result == -1)
		{
			cout<<"Page could not be written...."<<endl;
			return -1;
		}

		delete curSysTableBuff;*/
		char inserted = '1';
		if(_noOfEntries - 1 == 0)
			memcpy(&sysTableBuffer[FIRSTSYSTABSLOTPTR],&inserted,sizeof(char));
		else
			memcpy(&sysTableBuffer[FIRSTSYSTABSLOTPTR-((_noOfEntries-1)*sizeof(char))],&inserted,sizeof(char));
		return 1; // SysTableEntry can be added
	}
}

// Check if I can write it as a list of SysTableEntry objects instead of passing the buffer around.......

int SysTables::deleteSysTableEntry(string tabName,char * sysTableBuffer)
{
	// Subtract the pointer by SysTableEntrysize
	// Decrement the no. of SysTableEntries
	/*char * curSysTableBuff = new char [_pageSize];

	int result = pageRead(_pageID,curSysTableBuff); // From Chinmay, get buffer and page from cache........
	if(result == -1)
	{
		cout<<"Page could not be read...."<<endl;
		return -1;
	}*/

	bool found = 0;
	int entryID,i;
	char alreadyDeleted;

	for(i = 0;i<_noOfEntries;i++)
	{
		memcpy(&alreadyDeleted,&sysTableBuffer[FIRSTSYSTABSLOTPTR-(i*SYSTABSLOTSIZE)],SYSTABSLOTSIZE);

		if(alreadyDeleted == '0')
		{
			cout<<"The entry is deleted... Don't search there.....";
			continue;
		}

		char * newEntryBuff = new char [SYSTABLEENTRYSIZE];

		memcpy(newEntryBuff,&sysTableBuffer[0+(i*SYSTABLEENTRYSIZE)],SYSTABLEENTRYSIZE);

		char * tableName = new char [64];

		memcpy(tableName,&newEntryBuff[SYSTABTABLENAMEPTR],64);
		string entryTableName;
		for(int j=0;j<64;j++)
		{
			if(tableName[j] == '$')
				break;
			entryTableName = entryTableName+tableName[j];
		}

		if(tabName == entryTableName)
		{
			found = 1;
			entryID = i;
			delete tableName;
			delete newEntryBuff;
			break;
		}

		delete tableName;
		delete newEntryBuff;
	}

	if(found == 0)
	{
		cout<<"Table not found... Continue searching..."<<endl;
		return -1;// Entry not found
	}
	// When deleting the entry
	/*if(alreadyDeleted == '0')
	{
		cout<<"The table is already deleted... No need to delete it again..."<<endl;
		return 1;
	}*/
	alreadyDeleted = '0';
	memcpy(&sysTableBuffer[FIRSTSYSTABSLOTPTR-(i*SYSTABSLOTSIZE)],&alreadyDeleted,SYSTABSLOTSIZE);
	/*_curSysTableSize = _curSysTableSize - SYSTABLEENTRYSIZE;
	_remSysTableSize = _remSysTableSize + SYSTABLEENTRYSIZE;
	_noOfEntries = _noOfEntries - 1;
	_sysTableEntryPointer = _sysTableEntryPointer - SYSTABLEENTRYSIZE;

	memcpy(&sysTableBuffer[CURSYSTABSIZEPTR],&_curSysTableSize,sizeof(int));
	memcpy(&sysTableBuffer[REMSYSTABSIZEPTR],&_remSysTableSize,sizeof(int));
	memcpy(&sysTableBuffer[SYSTABNOEPTR],&_noOfEntries,sizeof(int));
	memcpy(&sysTableBuffer[SYSTABLEENTRYPTR],&_sysTableEntryPointer,sizeof(long));*/
	// Write SysTable entry to the buffer of this SysTablePage

	/*result = pageWrite(_pageID,curSysTableBuff);

	if(result == -1)
	{
		cout<<"Page could not be written...."<<endl;
		return -1;
	}

	delete curSysTableBuff;*/  // The calling function will take care of writing the buffer
	return i+1; // SysTableEntry deleted Found at (i+1)th slot
}

int SysTables::searchSysTableEntry(string tabName,char * sysTableBuffer)
{
	// Function to search for SysTable entry
	// Gets the no. of entries and searches from 0 to noOfEntries*sizeof(Entry)
	// Returns position + 1 so... When getting the position, the caller should subtract 1

	bool found = 0;
	int entryID,i;
	char entryDeleted;

	for(i=0;i<_noOfEntries;i++)
	{

		memcpy(&entryDeleted,&sysTableBuffer[FIRSTSYSTABSLOTPTR-(i*SYSTABSLOTSIZE)],SYSTABSLOTSIZE);
		if(entryDeleted == '0')
		{
			cout<<"Entry has been deleted... Continue searching....";
			continue;
		}
		char * newEntryBuff = new char [SYSTABLEENTRYSIZE];

		memcpy(newEntryBuff,&sysTableBuffer[0+(i*SYSTABLEENTRYSIZE)],SYSTABLEENTRYSIZE);

		char * tableName = new char [64];

		memcpy(tableName,&newEntryBuff[SYSTABTABLENAMEPTR],64);
		string entryTableName;
		for(int j=0;j<64;j++)
		{
			if(tableName[j] == '$')
				break;
			entryTableName = entryTableName+tableName[j];
		}

		if(tabName == entryTableName)
		{
			found = 1;
			entryID = i;
			delete tableName;
			delete newEntryBuff;
			break;
		}

		delete tableName;
		delete newEntryBuff;
	}

	if(found == 1)
	{
		cout<<"Entry found... Do not search in other pages......"<<endl;
		cout<<"Entry found at "<<i+1<<"th slot"<<endl;
		return i+1;
	}
	cout<<"Entry not found in this page.... Continue searching..."<<endl;
	return -1;
}
