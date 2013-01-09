// DB.cpp
// Main DB Class file

#include"DB.h"
//#include"FileHandling.cpp"

/*class DB
{
	private: int _dbHeaderPTR;
		 int _sysTablesPTR;
		 int _noSysTablePages;
		 int _sysColumnsPTR;
		 int _noSysColumnPages;
		 int _sysIndexPTR;
		 int _noSysIndexPages;
		 int _freePagePTR;
		 int _noFreePages;
		 int _pageSize;
		 int _totSize;

	public: DB()
		{
			_dbHeaderPTR = 1;
			_sysTablesPTR = 2;
			_sysColumnsPTR = 3;
			_sysIndexPTR = 4;
			_freePagePTR = 5;
			_noSysTablePages = 1;
			_noSysColumnPages = 1;
			_noSysIndexPages = 1;
			_pageSize = 2048;
			_totSize = 100*1024*1024;
			_noFreePages = (_totSize/_pageSize)-4;
		}

		~DB()
		{
			// Delete DB
		}

//		static DB * getDB()
//		{
//			return new DB();
//		}
};
*/

int DB::getDBHeaderPTR()
{
	return _dbHeaderPTR;
}

void DB::setDBHeaderPTR(int dbHeaderPTR)
{
	_dbHeaderPTR = dbHeaderPTR;
}

int DB::getSysTablesPTR()
{
	return _sysTablesPTR;
}

void DB::setSysTablesPTR(int sysTablesPTR)
{
	_sysTablesPTR = sysTablesPTR;
}

int DB::getSysColumnsPTR()
{
	return _sysColumnsPTR;
}

void DB::setSysColumnsPTR(int sysColumnsPTR)
{
	_sysColumnsPTR = sysColumnsPTR;
}

int DB::getSysIndexPTR()
{
	return _sysIndexPTR;
}

void DB::setSysIndexPTR(int sysIndexPTR)
{
	_sysIndexPTR = sysIndexPTR;
}

int DB::getFreePagePTR()
{
	return _freePagePTR;
}

void DB::setFreePagePTR(int freePagePTR)
{
	_freePagePTR = freePagePTR;
}

int DB::getLastFreePagePTR()
{
	return _lastFreePagePTR;
}

void DB::setLastFreePagePTR(int lastFreePagePTR)
{
	_lastFreePagePTR = lastFreePagePTR;
}

int DB::getNoSysTablePages()
{
	return _noSysTablePages;
}

void DB::setNoSysTablePages(int noSysTablePages)
{
	_noSysTablePages = noSysTablePages;
}

int DB::getNoSysColumnPages()
{
	return _noSysColumnPages;
}

void DB::setNoSysColumnPages(int noSysColumnPages)
{
	_noSysColumnPages = noSysColumnPages;
}

int DB::getNoSysIndexPages()
{
	return _noSysIndexPages;
}

void DB::setNoSysIndexPages(int noSysIndexPages)
{
	_noSysIndexPages = noSysIndexPages;
}

// DB Maintenance methods

bool DB::extendFreeSpaceCheck()
{
	if(/*Condition for extension....*/_noFreePages*_pageSize < 0.2*_totSize)
		return 1;
	else
		return 0;
}

int DB::extendFreeSpace()
{
	// Chinmay's code to extend the space allocated to the DB file.....
	// We can either extend in no. of bytes or pages......
	// Update the totalDBSize
	// Update the no. of pages
	// Update the free pages
	// Write the page back

	int sizeToBeExtended = 1024*_pageSize;

	BufferManager *bu = BufferManager::getBufferManager();

	int result = (*bu).expandDB(fdID,sizeToBeExtended);

	if(result != fdID)
	{
		// Extend free space has failed....
		return EXTENDFREESPACEERROR;
	}

	int noOfPages = (*bu).getPageCount(fdID);

	if(noOfPages < 0)
	{
		// Get Page Count failure
		return EXTENDFREESPACEGETPAGECOUNTERROR;
	}

	for(int i = noOfPages;i<=noOfPages+1024;i++)
	{
		char * buffer = new char [_pageSize];

		if(i!=noOfPages+1024)
		{
			FreePage * newPage = new FreePage(i,i+1);
			newPage->writeFreePageBuffer(buffer);
			delete newPage;
		}
		else
		{
			FreePage * newPage = new FreePage(i,-1);
			newPage->writeFreePageBuffer(buffer);
			delete newPage;
		}

		short priority = 0;
		bool retWriteDB;

		retWriteDB = (*bu).writeDB(fdID,i,(PagePriority)(priority),buffer);

		if(retWriteDB == false)
		{
			// Unable to write the buffer
			// Return error

			delete buffer;
			return DATABASEWRITEDBERROR;
		}
		
		delete buffer;
	}
	_lastFreePagePTR = noOfPages+1024;
	_totSize = _totSize + (1024*_pageSize);
	return _lastFreePagePTR;
}

int mainDB(DB * curDB,query q)
{
	string queryType = q->type;
	if(strcmp(queryType.c_str(),"CREATEDB") == 0)
		return curDB->createDB(q);
	else if(strcmp(queryType.c_str(),"USEDB") == 0)
		return curDB->useDB(q);
	else if(strcmp(queryType.c_str(),"DROPDB") == 0)
		return curDB->dropDB(q);
	else if(strcmp(queryType.c_str(),"SHOWDB") == 0)
		return curDB->showDB(q);
	else if(strcmp(queryType.c_str(),"SHOWTBL") == 0)
		return curDB->showTables(q);
	else if(strcmp(queryType.c_str(),"CREATETBL") == 0)
		return curDB->createTable(q);
	else if(strcmp(queryType.c_str(),"CREATEIND") == 0)
		return curDB->createIndex(q);
	else if(strcmp(queryType.c_str(),"DROPIND") == 0)
		return curDB->dropIndex(q);
	else if(strcmp(queryType.c_str(),"INSERT") == 0)
		return curDB->insertEntry(q);
	else if(strcmp(queryType.c_str(),"DELETE") == 0)
		return curDB->deleteEntry(q);
	else if(strcmp(queryType.c_str(),"UPDATE") == 0)
		return curDB->updateEntry(q);
	else if(strcmp(queryType.c_str(),"SELECT") == 0)
		return curDB->selectEntry(q);
	else if(strcmp(queryType.c_str(),"ALTER") == 0)
		return curDB->selectEntry(q);
	else
	{
		// Query is either not supported or is wrong
		return QUERYUNSUPPORTEDERROR;
	}
}

bool DB::queryEvaluate(query q,string * columnNames,string * dataTypes,int * ordinalPositions,short * scales,int * columnLengths)
{

}

bool DB::indexUse(condition * rootTree,string tableName,int * indexPageID,int * indexID)
{

}

int getFreePage(DB * curDB)
{
	BufferManager *bu = BufferManager::getBufferManager();
	int pageID = curDB->_freePagePTR;
	char * buffer = new char [curDB->_pageSize];
	int _pageSize = curDB->_pageSize;
	//buffer = readPage(pageID);// Chinmay code here for reading the page
	short priority = 0;
	bool retReadDB;

	retReadDB = (*bu).readDB(fdID,pageID,(PagePriority)(priority),buffer);

	if(retReadDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete buffer;
		return DATABASEREADDBERROR;
	}
	int nextFreePage;
	memcpy(&nextFreePage,&buffer[NEXTPTR],sizeof(int));

	curDB->_freePagePTR = nextFreePage;


	if(curDB->extendFreeSpaceCheck() == 1)
		curDB->extendFreeSpace();

	curDB->_noFreePages--;

	delete buffer;
	return pageID;
}

int addFreePageList(DB * curDB,int pageID)
{
	BufferManager *bu = BufferManager::getBufferManager();
	char * buffer = new char [curDB->_pageSize];
	int _pageSize = curDB->_pageSize;
	//buffer = readPage(pageID);// Chinmay code here for reading the page

	FreePage * newPage = new FreePage(pageID,curDB->_freePagePTR);

	newPage->writeFreePageBuffer(buffer);

	//writePageBuffer(pageID);// Chinmay code here for writing the page
	short priority = 0;
	bool retWriteDB;

	retWriteDB = (*bu).writeDB(fdID,pageID,(PagePriority)(priority),buffer);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete buffer;
		return DATABASEWRITEDBERROR;
	}
	curDB->_freePagePTR = pageID;
	curDB->_noFreePages++;
	delete buffer;
	return 1;
}

int DB::createNewSysTablesEntry(SysTableEntry newSysTableEntry)
{
	BufferManager *bu = BufferManager::getBufferManager();

	char *dataEntry = new char [SYSTABLEENTRYSIZE];

	int result = newSysTableEntry.fillBuffer(dataEntry);

	char * sysTabBuff = new char [_pageSize];

	short priority = 3;
	bool retReadDB;
	bool retWriteDB;

	bool inserted = 0;

	int nextPTR = 1,curPTR = -1,insertedPage;

	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysTabBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete dataEntry;
			delete sysTabBuff;
			return DATABASEREADDBERROR;
		}

		SysTables * sysTables = new SysTables(sysTabBuff,_pageSize);
		curPTR = nextPTR;
		nextPTR = sysTables->getnextSysTablePage();

		if(sysTables->checkEntryInsertion() == 1)
		{
			sysTables->createNewSysTableEntry(dataEntry,sysTabBuff);
			inserted = 1;
			insertedPage = curPTR;
			delete sysTables;
			break;
		}

		if(inserted == 0 && nextPTR == -1)
		{
			// We have to create a new SysTables page for inserting the data
			nextPTR = createNewSysTablesPage();

			// Set the next pointer
			sysTables->setnextSysTablePage(nextPTR);
			sysTables->writeSysTableBuffer(sysTabBuff);

			retWriteDB = (*bu).writeDB(fdID,curPTR,(PagePriority)(priority),sysTabBuff);

			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error

				delete sysTables;
				delete dataEntry;
				delete sysTabBuff;
				return DATABASEWRITEDBERROR;
			}

			// Read the new SysTables page
			retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysTabBuff);
			if(retReadDB == false)
			{
				// Unable to read the buffer
				// Return error
				delete dataEntry;
				delete sysTabBuff;
				delete sysTables;
				return DATABASEREADDBERROR;
			}

			SysTables * newSysTables = new SysTables(sysTabBuff,_pageSize);
			curPTR = nextPTR;

			newSysTables->createNewSysTableEntry(dataEntry,sysTabBuff);
			inserted = 1;
			insertedPage = curPTR;

			delete newSysTables;
			delete sysTables;
			break;
		}
	}

	retWriteDB = (*bu).writeDB(fdID,insertedPage,(PagePriority)(priority),sysTabBuff);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete dataEntry;
		delete sysTabBuff;
		return DATABASEWRITEDBERROR;
	}

	delete dataEntry;
	delete sysTabBuff;

	return insertedPage;
}

int DB::deleteSysTablesEntry(string tableName,string dbName)
{
	BufferManager *bu = BufferManager::getBufferManager();

	char * sysTabBuff = new char [_pageSize];

	short priority = 3;
	bool retReadDB;
	bool retWriteDB;

	int nextPTR = 1,curPTR = -1,retValue;
	bool deleted = false;

	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysTabBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete sysTabBuff;
			return DATABASEREADDBERROR;
		}

		SysTables * sysTables = new SysTables(sysTabBuff,_pageSize);
		curPTR = nextPTR;
		nextPTR = sysTables->getnextSysTablePage();

		retValue = sysTables->deleteSysTableEntry(tableName,dbName,sysTabBuff);
		if(retValue != -1)
		{
			// Entry found
			deleted = true;
			retWriteDB = (*bu).writeDB(fdID,curPTR,(PagePriority)(priority),sysTabBuff);

			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error

				delete sysTables;
				delete sysTabBuff;
				return DATABASEWRITEDBERROR;
			}
			delete sysTables;
			break;
		}

		delete sysTables;
	}

	delete sysTabBuff;
	if(deleted == false)
		return TABLENOTFOUNDERROR;
	return 1;
}

int DB::createNewSysColumnsEntry(SysColumnsEntry newSysColumnsEntry)
{
	BufferManager *bu = BufferManager::getBufferManager();

	char *dataEntry = new char [SYSCOLUMNENTRYSIZE];
	int result = newSysColumnsEntry.fillBuffer(dataEntry);

	char * sysColBuff = new char [_pageSize];

	short priority = 3;
	bool retReadDB;
	bool retWriteDB;

	bool inserted = 0;

	int nextPTR = 2,curPTR = -1,insertedPage;

	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysColBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete dataEntry;
			delete sysColBuff;
			return DATABASEREADDBERROR;
		}

		SysColumns * sysColumns = new SysColumns(sysColBuff,_pageSize);
		curPTR = nextPTR;
		nextPTR = sysColumns->getnextSysColumnsPage();

		if(sysColumns->checkEntryInsertion() == 1)
		{
			sysColumns->createNewSysColumnEntry(dataEntry,sysColBuff);
			inserted = 1;
			insertedPage = curPTR;
			delete sysColumns;
			break;
		}

		if(inserted == 0 && nextPTR == -1)
		{
			// We have to create a new SysColumns page for inserting the data
			nextPTR = createNewSysColumnsPage();

			// Set the next pointer
			sysColumns->setnextSysColumnsPage(nextPTR);
			sysColumns->writeSysColumnsBuffer(sysColBuff);

			retWriteDB = (*bu).writeDB(fdID,curPTR,(PagePriority)(priority),sysColBuff);

			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error

				delete sysColumns;
				delete dataEntry;
				delete sysColBuff;
				return DATABASEWRITEDBERROR;
			}

			// Read the new SysColumns page
			retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysColBuff);
			if(retReadDB == false)
			{
				// Unable to read the buffer
				// Return error
				delete dataEntry;
				delete sysColBuff;
				delete sysColumns;
				return DATABASEREADDBERROR;
			}

			SysColumns * newSysColumns = new SysColumns(sysColBuff,_pageSize);
			curPTR = nextPTR;

			newSysColumns->createNewSysColumnEntry(dataEntry,sysColBuff);
			inserted = 1;
			insertedPage = curPTR;

			delete newSysColumns;
			delete sysColumns;
			break;
		}
	}

	retWriteDB = (*bu).writeDB(fdID,insertedPage,(PagePriority)(priority),sysColBuff);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete dataEntry;
		delete sysColBuff;
		return DATABASEWRITEDBERROR;
	}

	delete dataEntry;
	delete sysColBuff;

	return insertedPage;
}

int DB::deleteSysColumnsEntry(string _dbName,string _tableName,string _columnName,string _dataType)
{
	BufferManager *bu = BufferManager::getBufferManager();

	char * sysColBuff = new char [_pageSize];

	short priority = 3;
	bool retReadDB;
	bool retWriteDB;

	int nextPTR = 2,curPTR = -1,retValue;
	bool deleted = false;

	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysColBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete sysColBuff;
			return DATABASEREADDBERROR;
		}

		SysColumns * sysColumns = new SysColumns(sysColBuff,_pageSize);
		curPTR = nextPTR;
		nextPTR = sysColumns->getnextSysColumnsPage();

		retValue = sysColumns->deleteSysColumnEntry(_columnName,_tableName,_dbName,_dataType,sysColBuff);
		if(retValue != -1)
		{
			// Entry found
			deleted = true;
			retWriteDB = (*bu).writeDB(fdID,curPTR,(PagePriority)(priority),sysColBuff);

			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error

				delete sysColumns;
				delete sysColBuff;
				return DATABASEWRITEDBERROR;
			}
			delete sysColumns;
			break;
		}

		delete sysColumns;
	}

	delete sysColBuff;
	if(deleted == false)
		return COLUMNNOTFOUNDERROR;
	return 1;
}

int DB::createNewSysIndexEntry(SysIndexEntry newSysIndexEntry)
{
	BufferManager *bu = BufferManager::getBufferManager();

	char *dataEntry = new char [SYSINDEXENTRYSIZE];
	int result = newSysIndexEntry.fillBuffer(dataEntry);

	char * sysIndexBuf = new char [_pageSize];

	short priority = 3;
	bool retReadDB;
	bool retWriteDB;

	bool inserted = 0;

	int nextPTR = 3,curPTR = -1,insertedPage;

	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysIndexBuf);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete dataEntry;
			delete sysIndexBuf;
			return DATABASEREADDBERROR;
		}

		SysIndex * sysIndex = new SysIndex(sysIndexBuf,_pageSize);
		curPTR = nextPTR;
		nextPTR = sysIndex->getNextSysIndexPage();

		if(sysIndex->checkEntryInsertion() == 1)
		{
			sysIndex->createNewSysIndexEntry(dataEntry,sysIndexBuf);
			inserted = 1;
			insertedPage = curPTR;
			delete sysIndex;
			break;
		}

		if(inserted == 0 && nextPTR == -1)
		{
			// We have to create a new SysIndex page for inserting the data
			nextPTR = createNewSysIndexPage();

			// Set the next pointer
			sysIndex->setNextSysIndexPage(nextPTR);
			sysIndex->writeSysIndexBuffer(sysIndexBuf);

			retWriteDB = (*bu).writeDB(fdID,curPTR,(PagePriority)(priority),sysIndexBuf);

			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error

				delete sysIndex;
				delete dataEntry;
				delete sysIndexBuf;
				return DATABASEWRITEDBERROR;
			}

			// Read the new SysIndex page
			retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysIndexBuf);
			if(retReadDB == false)
			{
				// Unable to read the buffer
				// Return error
				delete dataEntry;
				delete sysIndexBuf;
				delete sysIndex;
				return DATABASEREADDBERROR;
			}

			SysIndex * newSysIndex = new SysIndex(sysIndexBuf,_pageSize);
			curPTR = nextPTR;

			newSysIndex->createNewSysIndexEntry(dataEntry,sysIndexBuf);
			inserted = 1;
			insertedPage = curPTR;

			delete newSysIndex;
			delete sysIndex;
			break;
		}
	}

	retWriteDB = (*bu).writeDB(fdID,insertedPage,(PagePriority)(priority),sysIndexBuf);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete dataEntry;
		delete sysIndexBuf;
		return DATABASEWRITEDBERROR;
	}

	delete dataEntry;
	delete sysIndexBuf;

	return insertedPage;
}

int DB::deleteSysIndexEntry(string indexName,string tableName)
{
	BufferManager *bu = BufferManager::getBufferManager();

	char * sysIndexBuf = new char [_pageSize];

	short priority = 3;
	bool retReadDB;
	bool retWriteDB;

	int nextPTR = 3,curPTR = -1,retValue;
	bool deleted = false;

	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysIndexBuf);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete sysIndexBuf;
			return DATABASEREADDBERROR;
		}

		SysIndex * sysIndex = new SysIndex(sysIndexBuf,_pageSize);
		curPTR = nextPTR;
		nextPTR = sysIndex->getNextSysIndexPage();

		retValue = sysIndex->deleteSysIndexEntry(indexName,tableName,sysIndexBuf);
		if(retValue != -1)
		{
			// Entry found
			deleted = true;
			retWriteDB = (*bu).writeDB(fdID,curPTR,(PagePriority)(priority),sysIndexBuf);

			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error

				delete sysIndex;
				delete sysIndexBuf;
				return DATABASEWRITEDBERROR;
			}
			delete sysIndex;
			break;
		}

		delete sysIndex;
	}

	delete sysIndexBuf;
	if(deleted == false)
		return INDEXNOTFOUNDERROR;
	return 1;
}

int DB::createNewDirectoryPageEntry(int directoryPageID,int newDataPageID,int tfs,bool *noOfPagesChanged)
{
	char *buffer = new char [_pageSize];
	int nextPTR = directoryPageID;
	bool found = 0;
	int noOfEntries;
	short dirPagePriority = 2;
	bool retReadDB,retWriteDB;
	int pageID,resultID,result;
	string debugMessage;
	bool retWriteLog;

	BufferManager *bu = BufferManager::getBufferManager();

	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(dirPagePriority),buffer);
		if(retReadDB == false)
		{
			// Unable to read the buffer
			// Return error
			delete buffer;
			return DATABASEREADDBERROR;
		}
		DirectoryPage * directoryPage = new DirectoryPage(buffer,_pageSize);
		nextPTR = directoryPage->getNextDirectoryPagePointer();
		noOfEntries = directoryPage->getNoE();
		pageID = directoryPage->getpageID();
		if(noOfEntries < 0)
		{
			delete directoryPage;
			continue;
		}
		if(directoryPage->checkDirectoryEntryInsertion() != 0)
		{
			found = 1;
			resultID = pageID;
			delete directoryPage;
			break;
		}
		if(found == 0 && nextPTR == -1)
		{
			// Create a new directory page and then insert there.....
			// Creating a new directory page....
			int createNewDirPage = createNewDirectoryPage();
			*noOfPagesChanged = true;
			if(createNewDirPage < 0)
			{
				// Return new Directory page creation error
				delete directoryPage;
				delete buffer;
				return CREATEDIRECTORYPAGEENTRY;
			}
			resultID = createNewDirPage;
			//result = pageWrite(fileName,totPages+1,newBuffer,_pageSize);
			// Update the next pointer of the previous directory page and then copy the buffer back.....
			directoryPage->setNextDirectoryPagePointer(createNewDirPage);
			directoryPage->fillBuffer(buffer);
			retWriteDB = (*bu).writeDB(fdID,pageID,(PagePriority)(dirPagePriority),buffer);
			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error
				delete directoryPage;
				delete buffer;
				return DATABASEWRITEDBERROR;
			}
			//result = pageWrite(fileName,pageID,buffer,_pageSize);
		}
		delete directoryPage;
	}
	retReadDB = (*bu).readDB(fdID,resultID,(PagePriority)(dirPagePriority),buffer);
	if(retReadDB == false)
	{
		// Unable to read the buffer
		// Return error
		delete buffer;
		return DATABASEREADDBERROR;
	}
	DirectoryPage * directoryPage = new DirectoryPage(buffer,_pageSize);
	DirectoryPageEntry * newDPEntry = new DirectoryPageEntry(newDataPageID,tfs);
	char * entryBuff = new char [DESIZE];
	newDPEntry->fillBuffer(entryBuff);
	result = directoryPage->insertDirectoryPageEntry(entryBuff,buffer);
	directoryPage->updateMaxTFS(buffer);
	directoryPage->fillBuffer(buffer);
	retWriteDB = (*bu).writeDB(fdID,resultID,(PagePriority)(dirPagePriority),buffer);
	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete directoryPage;
		delete buffer;
		delete entryBuff;
		delete newDPEntry;
		return DATABASEWRITEDBERROR;
	}
	delete entryBuff;
	delete newDPEntry;
	delete directoryPage;
	delete buffer;
	if(debugFlag == true)
	{
		debugMessage = "The entry was inserted into Directory Page ID: "+resultID;
		retWriteLog = writeLog(debugMessage);
	}
	return resultID;
}

int DB::insertDataBaseEntry(int directoryPageID,char *dataBuffer,bool *noOfPagesChanged)
{
	int nextPTR = directoryPageID;
	int curPTR = -1;
	bool insertedFlag = false;
	string debugMessage;
	bool retWriteLog;

	int dataSize = strlen(dataBuffer);

	BufferManager *bu = BufferManager::getBufferManager();

	while(nextPTR != -1 && insertedFlag == 0)
	{
		char * dirPageBuff = new char [_pageSize];

		short dirPagePriority = 2,dataPagePriority = 1;
		bool retReadDB;
		bool retWriteDB;

		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(dirPagePriority),dirPageBuff);
		if(retReadDB == false)
		{
			// Unable to read the buffer
			// Return error
			delete dirPageBuff;
			return DATABASEREADDBERROR;
		}

		DirectoryPage * directoryPage = new DirectoryPage(dirPageBuff,_pageSize);
		curPTR = nextPTR;
		nextPTR = directoryPage->getNextDirectoryPagePointer();
		int noOfEntries = directoryPage->getNoE();
		int maxTFS = directoryPage->getMaxTFS();
		bool newDirectoryPageCreate = 0;
		bool newDirectoryEntryCreate = 0;
		if(noOfEntries == 0)
		{
			// There are no entries here Add debug flag here
			if(debugFlag == true)
			{
				debugMessage = "There are no entries "+curPTR;
				debugMessage = debugMessage+" in the page for data page.";
				retWriteLog = writeLog(debugMessage);
			}
		}
		else if(directoryPage->checkMaxTFS(dataSize) == 0)
		{
			if(debugFlag == true)
			{
				debugMessage = "There is no data page in this directory page "+curPTR;
				debugMessage = debugMessage+" where we can insert this data.";
				retWriteLog = writeLog(debugMessage);
			}
			if(nextPTR == -1)
				newDirectoryPageCreate = 1;
		}
		else
		{
			for(int j = 0;j<noOfEntries;j++)
			{
				if(insertedFlag == 1)
					break;
				char * entryBuf = new char [DESIZE];
				memcpy(entryBuf,&dirPageBuff[0+j*DESIZE],DESIZE);
				DirectoryPageEntry *dirPageEntry = new DirectoryPageEntry(entryBuf);
				int pageID = dirPageEntry->getPageID(),tfs = dirPageEntry->getTFS();
				if(tfs < dataSize + DPSLOTSIZE)
				{
					if(debugFlag == true)
					{
						debugMessage = "Cannot insert into this data page "+pageID;
						debugMessage = debugMessage+".";
						retWriteLog = writeLog(debugMessage);
					}
					if(j+1 == noOfEntries)
					{
						newDirectoryEntryCreate = 1;
						if(debugFlag == true)
						{
							debugMessage = "Create a new data page, insert its entry into the directory page and then insert the data";
							retWriteLog = writeLog(debugMessage);
						}
					}
				}
				else
				{
					char * dataPageBuf = new char [_pageSize];
					retReadDB = (*bu).readDB(fdID,dirPageEntry->getPageID(),(PagePriority)(dataPagePriority),dataPageBuf);
					if(retReadDB == false)
					{
						// Unable to read the buffer
						// Return error
						delete dataPageBuf;
						delete dirPageBuff;
						delete entryBuf;
						delete dirPageEntry;
						return DATABASEREADDBERROR;
					}
					DataPage * newDataPage1 = new DataPage(dataPageBuf,_pageSize);
					int inserted1 = newDataPage1->insertData(dataPageBuf,dataBuffer,dataSize);
					if(inserted1 == -1)
					{
						if(debugFlag == true)
						{
							debugMessage = "Data was not inserted to the data page "+dirPageEntry->getPageID();
							retWriteLog = writeLog(debugMessage);
						}
						newDirectoryEntryCreate = 1;
					}
					else
					{
						if(debugFlag == true)
						{
							debugMessage = "Data was inserted to the data page "+dirPageEntry->getPageID();
							retWriteLog = writeLog(debugMessage);
						}
						tfs = tfs - (dataSize + DPSLOTSIZE);
						dirPageEntry->setTFS(tfs);
						dirPageEntry->fillBuffer(entryBuf);
						memcpy(&dirPageBuff[0+j*DESIZE],entryBuf,DESIZE);
						directoryPage->updateMaxTFS(dirPageBuff);
						directoryPage->fillBuffer(dirPageBuff);
						retWriteDB = (*bu).writeDB(fdID,dirPageEntry->getPageID(),(PagePriority)(dataPagePriority),dataPageBuf);
						if(retWriteDB == false)
						{
							// Unable to write the buffer
							// Return error

							delete dataPageBuf;
							delete newDataPage1;
							delete dirPageBuff;
							delete entryBuf;
							delete dirPageEntry;
							delete directoryPage;
							return DATABASEWRITEDBERROR;
						}
						retWriteDB = (*bu).writeDB(fdID,directoryPage->getpageID(),(PagePriority)(dirPagePriority),dirPageBuff);
						if(retWriteDB == false)
						{
							// Unable to write the buffer
							// Return error

							delete dataPageBuf;
							delete newDataPage1;
							delete dirPageBuff;
							delete entryBuf;
							delete dirPageEntry;
							delete directoryPage;
							return DATABASEWRITEDBERROR;
						}
						//result = pageWrite(fileName,dirPageEntry->getPageID(),dataPageBuf,_pageSize);
						//result = pageWrite(fileName,directoryPage->getpageID(),deBuf,_pageSize);
						insertedFlag = 1;
					}
					delete newDataPage1;
					delete dataPageBuf;
				}
				delete dirPageEntry;
				delete entryBuf;
			}
		}
		if(insertedFlag == 0 && nextPTR == -1)
		{
			// Data was not inserted.... 2 reasons create new directory entry or create new directory page
			int newDataPageID = createNewDataPage();
			char * newDataPageBuffer = new char [_pageSize];
			retReadDB = (*bu).readDB(fdID,newDataPageID,(PagePriority)(dataPagePriority),newDataPageBuffer);
			if(retReadDB == false)
			{
				// Unable to read the buffer
				// Return error
				delete dirPageBuff;
				delete newDataPageBuffer;
				return DATABASEREADDBERROR;
			}
			DataPage * newDataPage1 = new DataPage(newDataPageBuffer,_pageSize);
			int inserted1 = newDataPage1->insertData(newDataPageBuffer,dataBuffer,dataSize);
			if(debugFlag == true)
			{
				debugMessage = "Data was inserted in "+newDataPageID;
				debugMessage = debugMessage+"th page.";
				retWriteLog = writeLog(debugMessage);
			}
			retWriteDB = (*bu).writeDB(fdID,newDataPageID,(PagePriority)(dataPagePriority),newDataPageBuffer);
			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error
				delete dirPageBuff;
				delete newDataPageBuffer;
				delete newDataPage1;
				return DATABASEWRITEDBERROR;
			}
			int createNewDirectoryEntry = createNewDirectoryPageEntry(directoryPageID,newDataPageID,newDataPage1->getcfs(),noOfPagesChanged);
			if(createNewDirectoryEntry < 0)
			{
				// Unable to insert directory page entry
				delete newDataPage1;
				delete newDataPageBuffer;
				return INSERTDIRECTORYPAGEENTRY;
			}
			delete newDataPage1;
			delete newDataPageBuffer;
		}
		delete dirPageBuff;
	}
}

int DB::createNewSysTablesPage()
{

	int freePageID = getFreePage(this);

	if(freePageID < 0)
		return FREEPAGEERROR;

	BufferManager *bu = BufferManager::getBufferManager();

	SysTables * sysTables = new SysTables(freePageID,_pageSize);
	char * tabBuff = new char [_pageSize];
	sysTables->writeSysTableBuffer(tabBuff);

	short priority = 3;
	bool retWriteDB;
	retWriteDB = (*bu).writeDB(fdID,freePageID,(PagePriority)(priority),tabBuff);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete sysTables;
		delete tabBuff;
		return DATABASEWRITEDBERROR;
	}

	delete sysTables;
	delete tabBuff;

	_noSysTablePages++;

	return freePageID;
}

int DB::createNewSysColumnsPage()
{
	int freePageID = getFreePage(this);

	if(freePageID < 0)
		return FREEPAGEERROR;

	BufferManager *bu = BufferManager::getBufferManager();

	SysColumns * sysColumns = new SysColumns(freePageID,_pageSize);
	char * colBuff = new char [_pageSize];
	sysColumns->writeSysColumnsBuffer(colBuff);

	short priority = 3;
	bool retWriteDB;
	retWriteDB = (*bu).writeDB(fdID,freePageID,(PagePriority)(priority),colBuff);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete sysColumns;
		delete colBuff;
		return DATABASEWRITEDBERROR;
	}

	delete sysColumns;
	delete colBuff;

	_noSysColumnPages++;

	return freePageID;
}

int DB::createNewSysIndexPage()
{
	int freePageID = getFreePage(this);

	if(freePageID < 0)
		return FREEPAGEERROR;

	BufferManager *bu = BufferManager::getBufferManager();

	char * indexBuff = new char [_pageSize];
	SysIndex * sysIndex = new SysIndex(freePageID,_pageSize);
	sysIndex->writeSysIndexBuffer(indexBuff);

	short priority = 3;
	bool retWriteDB;
	retWriteDB = (*bu).writeDB(fdID,freePageID,(PagePriority)(priority),indexBuff);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete sysIndex;
		delete indexBuff;
		return DATABASEWRITEDBERROR;
	}

	delete sysIndex;
	delete indexBuff;

	_noSysIndexPages++;

	return freePageID;
}

int DB::createNewDirectoryPage()
{
	int freePageID = getFreePage(this);

	if(freePageID < 0)
		return FREEPAGEERROR;

	BufferManager *bu = BufferManager::getBufferManager();

	char * newDirPageBuffer = new char [_pageSize];

	//buffer = readPage(freePageID);// Chinmay code here for reading the page
	/*short priority = 0;
	bool retReadDB;
	retReadDB = (*bu).readDB(fdID,0,(PagePriority)(priority),newDirPageBuffer);
	if(retReadDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete newDirPageBuffer;
		//delete dbHeader;
		return DATABASEREADDBERROR;
	}*/

	DirectoryPage * newDirPage = new DirectoryPage(_pageSize,freePageID);
	newDirPage->fillBuffer(newDirPageBuffer);

	// Write the page to database
	// pageWrite(buffer,pageID);
	short priority = 2;
	bool retWriteDB;
	retWriteDB = (*bu).writeDB(fdID,freePageID,(PagePriority)(priority),newDirPageBuffer);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete newDirPage;
		delete newDirPageBuffer;
		return DATABASEWRITEDBERROR;
	}

	delete newDirPage;
	delete newDirPageBuffer;

	return freePageID;
}

int DB::createNewDataPage()
{
	int freePageID = getFreePage(this);

	if(freePageID < 0)
		return FREEPAGEERROR;

	BufferManager *bu = BufferManager::getBufferManager();

	DataPage dataPage(_pageSize,freePageID);
	char * buffer = new char [_pageSize];
	dataPage.fillBuffer(buffer);

	short priority = 1;
	bool retWriteDB;
	retWriteDB = (*bu).writeDB(fdID,freePageID,(PagePriority)(priority),buffer);

	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error

		delete buffer;
		return DATABASEWRITEDBERROR;
	}

	delete buffer;

	return freePageID;
}

int DB::deleteSysTablesPage(int pageID)
{

}

int DB::deleteSysColumnsPage(int pageID)
{

}

int DB::deleteSysIndexPage(int pageID)
{

}

int DB::deleteDirectoryPage(int pageID)
{

}

int DB::deleteDataPage(int pageID)
{

}


// DB Queries

int DB::createDB(query q)
{
	// Add code to check whether database name already exists.......
	// Steps: In the database root get the list of the names of the files
	// Extract the file names....
	// Check by comparing the database name looping over the list of file names
	// If exists return error DBNAMEUNIQUEERROR -2001
	// Else continue
	// Create file of size*pageSize bytes... Call Chinmay's code here

	BufferManager *bu = BufferManager::getBufferManager();
	int size = 100*1024*1024;

	int pageSize = (*bu).getPageSize();
	_pageSize = pageSize;

	vector<string> files = vector<string>();
	string filePath = DBROOT;

	bool listDBres = (*bu).listDBs(filePath,files);
	if(listDBres == false)
	{
		// Unable to open the data directory
		return UNABLETOOPENFOLDERERROR;
	}

	for (unsigned int listCount = 0;listCount < files.size();listCount++)
	{
	        string curFileName = files[listCount].substr(0,files[listCount].find_last_of("_"));
		if(curFileName == q->dbname)
		{
			// DataBase Name already exists....
			// Cannot create new database....
			return DBNAMEUNIQUEERROR;
		}
	}

	DBHeader * dbHeader = new DBHeader(size,pageSize,q->dbname);
	char * dbBuff = new char [pageSize];
	dbHeader->writeDBHeader(dbBuff);
	string fileName = DBROOT;
	fileName = fileName+(q->dbname);
	fileName = fileName+"_";
	//fileName = fileName+pageSize;
	string pageSz;
	int tempPageSize = pageSize;
	while(tempPageSize!=0)
	{
		int d = tempPageSize%10;
		char digit = '0'+d;
	    	pageSz = digit+pageSz;
		tempPageSize = tempPageSize/10;
	}
	fileName = fileName+pageSz;
	fileName = fileName+".dat";
	int resCreateDB = (*bu).createDB(filePath,fileName,size);
	if(resCreateDB < 0)
	{
		// Unable to create the Database file
		return DATABASEFILECREATEERROR;
	}
	//fileCreate(fileName);
	fdID = (*bu).openDB(filePath,fileName);
	if(fdID < 0)
	{
		// Unable to open the Database file
		return DATABASEFILEOPENERRROR;
	}
	//pageWrite(fileName,1,dbBuff,pageSize);
	short priority = 3;
	bool retWriteDB;
	retWriteDB = (*bu).writeDB(fdID,_dbHeaderPTR,(PagePriority)(priority),dbBuff);
	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		delete dbHeader;
		return DATABASEWRITEDBERROR;
	}
	SysTables * sysTables = new SysTables(_sysTablesPTR,pageSize);
	char * tabBuff = new char [pageSize];
	//sysTables->setPageSize(pageSize);
	sysTables->writeSysTableBuffer(tabBuff);
	//pageWrite(fileName,2,tabBuff,pageSize);
	retWriteDB = (*bu).writeDB(fdID,_sysTablesPTR,(PagePriority)(priority),tabBuff);
	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		delete tabBuff;
		delete sysTables;
		delete dbHeader;
		return DATABASEWRITEDBERROR;
	}
	SysColumns * sysColumns = new SysColumns(_sysColumnsPTR,pageSize);
	char * colBuff = new char [pageSize];
	sysColumns->setPageSize(pageSize);
	sysColumns->writeSysColumnsBuffer(colBuff);
	//pageWrite(fileName,3,colBuff,pageSize);
	retWriteDB = (*bu).writeDB(fdID,_sysColumnsPTR,(PagePriority)(priority),colBuff);
	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		delete tabBuff;
		delete colBuff;
		delete sysColumns;
		delete sysTables;
		delete dbHeader;
		return DATABASEWRITEDBERROR;
	}
	char * indexBuff = new char [pageSize];
	SysIndex * sysIndex = new SysIndex(_sysIndexPTR,pageSize);
	sysIndex->writeSysIndexBuffer(indexBuff);
	//pageWrite(fileName,4,indexBuff,pageSize);
	retWriteDB = (*bu).writeDB(fdID,_sysIndexPTR,(PagePriority)(priority),indexBuff);
	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		delete tabBuff;
		delete colBuff;
		delete indexBuff;
		delete sysIndex;
		delete sysColumns;
		delete sysTables;
		delete dbHeader;
		return DATABASEWRITEDBERROR;
	}
	delete dbBuff;
	delete tabBuff;
	delete colBuff;
	delete indexBuff;
	delete sysIndex;
	delete sysColumns;
	delete sysTables;
	delete dbHeader;
	int totPages = dbHeader->getTotalPages();
	priority = 0;
	for(int i = 4;i<totPages;i++)
	{
		if(i == totPages)
		{
			FreePage * freePage = new FreePage(i,-1,pageSize);
			char *idxbuffer = new char [pageSize];
			freePage->writeFreePageBuffer(idxbuffer);
			//pageWrite(fileName,i,idxbuffer,pageSize);
			retWriteDB = (*bu).writeDB(fdID,i,(PagePriority)(priority),idxbuffer);
			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error
				delete idxbuffer;
				delete freePage;
				return DATABASEWRITEDBERROR;
			}
			delete idxbuffer;
			delete freePage;
		}
		else
		{
			FreePage * freePage = new FreePage(i,i+1,pageSize);
			char *idxbuffer = new char [pageSize];
			freePage->writeFreePageBuffer(idxbuffer);
			//pageWrite(fileName,i,idxbuffer,pageSize);
			retWriteDB = (*bu).writeDB(fdID,i,(PagePriority)(priority),idxbuffer);
			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error
				delete idxbuffer;
				delete freePage;
				return DATABASEWRITEDBERROR;
			}
			delete idxbuffer;
			delete freePage;
		}
	}

	// Write all buffers to pages......
	// Call Chinmay's code for writing pages

	// delete sysIndex

	return 1;
}

int DB::useDB(query q)
{
	// For use database command
	// If fdID != -1, then commit the cache contents to the hard disk.....
	// Search whether the database exists, if it does not flag an error
	// If it exists, then perform the following steps
	// Read the database file..... (first 100 pages maybe)
	// Read the database header....
	// Check if page size matches with the DB cache page size, if it does not, flag an error, that page sizes do not match and reset the page size from the cache panel
	// Read all the SysTables files.....
	// Read all the SysColumns files.....
	// Read all the SysIndex files......

	BufferManager *bu = BufferManager::getBufferManager();

	int pageSize = (*bu).getPageSize();

	if(fdID != -1)
	{
		// Rewrite the DBHeader
		char * dbBuff = new char [pageSize];
		short priority = 3,dirPagePriority = 1;
		bool retReadDB;
		retReadDB = (*bu).readDB(fdID,_dbHeaderPTR,(PagePriority)(priority),dbBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete dbBuff;
			return DATABASEREADDBERROR;
		}
		
		DBHeader * dbHeader = new DBHeader(dbBuff,_pageSize);
		dbHeader->setFreePagePointer(_freePagePTR);
		dbHeader->setNoFreePages(_noFreePages);
		dbHeader->setSysTablesPointer(_sysTablesPTR);
		dbHeader->setSysColumnsPointer(_sysColumnsPTR);
		dbHeader->setSysIndexPointer(_sysIndexPTR);
		dbHeader->setTotalDBSize(_totSize);
		dbHeader->setTotalPages(_totSize/_pageSize);
		dbHeader->writeDBHeader(dbBuff);
		
		// Commit the cache contents to the disk to read the new data base....
		
		bool retCommitCache;
		retCommitCache = (*bu).commitCache();
		if(retCommitCache == false)
		{
			// Commiting to cache failed
			return COMMITCACHEERROR;
		}
		delete dbHeader;
		delete dbBuff;
	}
	
	vector<string> files = vector<string>();
	string filePath = DBROOT;

	bool listDBres = (*bu).listDBs(filePath,files);
	if(listDBres == false)
	{
		// Unable to open the data directory
		return UNABLETOOPENFOLDERERROR;
	}
	bool dbFound = false;

	for (unsigned int listCount = 0;listCount < files.size();listCount++)
	{
		int lastPos = files[listCount].find_last_of("_");
		string curFileName = files[listCount].substr(0,lastPos);
		if(curFileName == q->dbname)
		{
			int lastDotPos = files[listCount].find_last_of(".");
			string pgSz = files[listCount].substr(lastPos+1,lastDotPos-lastPos-1);
			int dbPageSize = 0;
			for(int i=0;i<pgSz.length();i++)
			{
				char ch = pgSz[i];
				int digit = ch - '0';
				dbPageSize = dbPageSize*10+digit;
			}
			if(dbPageSize != pageSize)
			{
				// Page Sizes do not match.... Return error...
				// Change the cache page size....
				return CACHEPAGESIZEERROR;
			}
			dbFound = true;
			break;
		}
	}

	if(dbFound == false)
	{
		// Database not found error
		return DATABASENOTFOUNDERROR;
	}

	// Open the database.....
	string fileName = q->dbname;
	string pageSz;
	int tempPageSize = pageSize;
	while(tempPageSize!=0)
	{
		int d = tempPageSize%10;
		char digit = '0'+d;
	    	pageSz = digit+pageSz;
		tempPageSize = tempPageSize/10;
	}
	fileName = fileName+pageSz;
	fdID = (*bu).openDB(filePath,fileName);
	if(fdID < 0)
	{
		// Unable to open the Database file
		return DATABASEFILEOPENERRROR;
	}

	// Read the DatabaseHeader,SysTables,SysColumns,SysIndex and Directory Pages

	// Reading the DBHeader page
	char * dbBuff = new char [pageSize];
	short priority = 3,dirPagePriority = 1;
	bool retReadDB;
	retReadDB = (*bu).readDB(fdID,_dbHeaderPTR,(PagePriority)(priority),dbBuff);
	if(retReadDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		return DATABASEREADDBERROR;
	}
	DBHeader * dbHeader = new DBHeader(dbBuff,_pageSize);
	_freePagePTR = dbHeader->getFreePagePointer();
	_noFreePages = dbHeader->getNoFreePages();
	_sysTablesPTR = dbHeader->getSysTablesPointer();
	_sysColumnsPTR = dbHeader->getSysColumnsPointer();
	_sysIndexPTR = dbHeader->getSysIndexPointer();
	_totSize = dbHeader->getTotalDBSize();
	_lastFreePagePTR = _totSize/_pageSize - 1;
	delete dbHeader;
	delete dbBuff;

	// Reading all the SysTable pages
	char * sysTabBuff = new char [pageSize];
	int nextPTR = _sysTablesPTR,curPTR = -1;
	_noSysTablePages = 0;
	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysTabBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete sysTabBuff;
			return DATABASEREADDBERROR;
		}
		SysTables * curSysTables = new SysTables(sysTabBuff,pageSize);
		_noSysTablePages++;
		int noOfEntries = curSysTables->getNoOfEntries();

		if(noOfEntries != 0)
		{
			char deleted;
			int directoryPagePointer;
			for(int i=0;i<noOfEntries;i++)
			{
				memcpy(&deleted,&sysTabBuff[FIRSTSYSTABSLOTPTR-(i*SYSTABSLOTSIZE)],SYSTABSLOTSIZE);
				if(deleted == '0')
					continue;
				char * newEntryBuff = new char [SYSTABLEENTRYSIZE];
				memcpy(newEntryBuff,&sysTabBuff[0+(i*SYSTABLEENTRYSIZE)],SYSTABLEENTRYSIZE);
				memcpy(&directoryPagePointer,&newEntryBuff[SYSTABDIRPAGEPTR],sizeof(int));
				while(directoryPagePointer != -1)
				{
					char * dirPageBuff = new char [pageSize];
					retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(dirPagePriority),dirPageBuff);
					if(retReadDB == false)
					{
						// Unable to write the buffer
						// Return error
						delete dirPageBuff;
						delete newEntryBuff;
						delete curSysTables;
						delete sysTabBuff;
						return DATABASEREADDBERROR;
					}
					memcpy(&directoryPagePointer,&dirPageBuff[NEXTPTR],sizeof(int));
					delete dirPageBuff;
				}
				delete newEntryBuff;
			}
		}

		delete curSysTables;
	}

	delete sysTabBuff;

	// Read all the SysColumn pages
	char * sysColBuff = new char [pageSize];
	nextPTR = _sysColumnsPTR,curPTR = -1;
	_noSysColumnPages = 0;
	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysColBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete sysColBuff;
			return DATABASEREADDBERROR;
		}
		_noSysColumnPages++;
	}

	delete sysColBuff;

	// Read all the SysIndex Pages
	char * sysIndexBuff = new char [pageSize];
	nextPTR = _sysIndexPTR,curPTR = -1;
	_noSysIndexPages = 0;
	while(nextPTR != -1)
	{
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(priority),sysIndexBuff);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete sysIndexBuff;
			return DATABASEREADDBERROR;
		}
		_noSysIndexPages++;
		// Decide whether to read in the index or not
	}

	delete sysIndexBuff;

	return 1;
}

int DB::showDB(query q)
{

}

int DB::dropDB(query q)
{
	// For drop database command
	// Delete the database file.....
	BufferManager *bu = BufferManager::getBufferManager();

	int pageSize = (*bu).getPageSize();

	// Reading the DBHeader page
	char * dbBuff = new char [pageSize];
	short priority = 3,dirPagePriority = 1;
	bool retReadDB;
	retReadDB = (*bu).readDB(fdID,_dbHeaderPTR,(PagePriority)(priority),dbBuff);
	if(retReadDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		return DATABASEREADDBERROR;
	}

	string curDBName;
	string filePath = DBROOT;
	int lastPos = curDBName.find_last_of("_");
	string fileName = curDBName.substr(0,lastPos);
	bool curDBdropped = false;
	DBHeader * curDBHeader = new DBHeader(dbBuff,pageSize);
	curDBName = curDBHeader->getDatabaseName();
	if(fileName == q->dbname)
		curDBdropped = true;
	delete curDBHeader;
	delete dbBuff;

	bool dbFound = false;

	if(curDBdropped == false)
	{
		vector<string> files = vector<string>();
		bool listDBres = (*bu).listDBs(filePath,files);
		if(listDBres == false)
		{
			// Unable to open the data directory
			return UNABLETOOPENFOLDERERROR;
		}
		for (unsigned int listCount = 0;listCount < files.size();listCount++)
		{
			int lastPos = files[listCount].find_last_of("_");
			string curFileName = files[listCount].substr(0,lastPos);
			if(curFileName == q->dbname)
			{
				fileName = curFileName;
				dbFound = true;
				break;
			}
		}
	}

	if(dbFound == false && curDBdropped == false)
	{
		// Database not found error
		return DROPDBDATABASENOTFOUNDERROR;
	}

	// Call Chinmay's function to delete the database file.
	int resDropDB = (*bu).dropDB(filePath,fileName);
	if(resDropDB < 0)
	{
		// Error in dropping the database
		return DROPDBERROR;
	}
	if(curDBdropped == true)
		fdID = -1; // This should be set only if the current loaded database was deleted.
	return 1;
}

int DB::createTable(/*Query Parameter Structure*/query q)
{
	// For create table command
	// Create a SysTable Entry for the table
	// Create SysColumns entry for each of the columns of the table, if columnCount is not 0
	// Create a Directory and Data Page for the table, updating the corresponding pages
	int nextSysTabPointer = _sysTablesPTR, nextSysColumnsPointer = _sysColumnsPTR;
	int curSysTabPointer = -1,curSysColsPointer = -1,i,j;
	string tableName = q->table; // get the first table name

	BufferManager *bu = BufferManager::getBufferManager();
	char * dbheaderBuff = new char [_pageSize];
	short priority = 3;
	bool retReadDB;
	retReadDB = (*bu).readDB(fdID,_dbHeaderPTR,(PagePriority)(priority),dbheaderBuff);
	if(retReadDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbheaderBuff;
		return DATABASEREADDBERROR;
	}
	DBHeader * dbHeader = new DBHeader(dbheaderBuff,_pageSize);
	string dbName = dbHeader->getDatabaseName();
	delete dbHeader;
	delete dbheaderBuff;

	char rowFormat = 'F',encodingScheme = 'A',tableType='T';
	unsigned short dataLength,recLength;
	int dirPagePointer,tableRows = 0,noOfPages = 0;
	short charLength;
	string remarks;
	unsigned short colCount=(q->cntColumns),curDataLength,curRecordLength,keyColumns=0;
	bool createSysIndexEntry;
	int colIndex = -1,avgRowLength = 0;
	// Check if table name already exists, if so return TABLENAMEUNIQUEERROR
	while(nextSysTabPointer != -1)
	{
		char * curSysTabBuffer = new char [_pageSize];
		// Read the file to the buffer
		retReadDB = (*bu).readDB(fdID,nextSysTabPointer,(PagePriority)(priority),curSysTabBuffer);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete curSysTabBuffer;
			return DATABASEREADDBERROR;
		}
		SysTables * curSysTable = new SysTables(curSysTabBuffer,_pageSize);
		curSysTabPointer = nextSysTabPointer;
		nextSysTabPointer = curSysTable->getnextSysTablePage();
		if(curSysTable->searchSysTableEntry(tableName,dbName,curSysTabBuffer) != -1)
		{
			delete curSysTable;
			delete curSysTabBuffer;
			return TABLENAMEUNIQUEERROR;
			// TABLENAMEUNIQUEERROR = -2002
		}
		delete curSysTable;
		delete curSysTabBuffer;
	}
	for(i=0;i<colCount;i++)
	{
		// Check if column names specified are unique, if they are not, return COLUMNNAMEUNIQUEERROR
		for(j=i+1;j<colCount;j++)
		{
			if(strcmp((q->columns[i]).name,(q->columns[j]).name)==0 && strcmp((q->columns[i]).type,(q->columns[j]).type)==0)
				return COLUMNNAMEUNIQUEERROR;
		}
		curDataLength = 0,curRecordLength=0;
		if(strncmp((q->columns[i]).type,"VARCHAR$",8)==0)
			rowFormat = 'D';
		if((q->columns[i]).isPrimary == 1)
		{
			createSysIndexEntry = true;
			colIndex = i;
			keyColumns=1;
		}
		if(strncmp((q->columns[i]).type,"VARCHAR$",8)==0 || strncmp((q->columns[i]).type,"CHAR$$$$",8)==0)
		{
			charLength = (q->columns[i]).sizeofField1; // length of the character or varchar
			short dataTypeID = retDataTypeID((q->columns[i]).type);
			curDataLength = retDataTypeSize(dataTypeID,charLength);
			if(strcmp((q->columns[i]).type,"VARCHAR$")==0)
				curRecordLength = curDataLength + sizeof(short);
			else
				curRecordLength = curDataLength;
		}
		else
		{
			short dataTypeID = retDataTypeID((q->columns[i]).type);
			curDataLength = retDataTypeSize(dataTypeID,1);
			curRecordLength = curDataLength;
		}
		dataLength += curDataLength,recLength += curRecordLength;
	}
	dirPagePointer = createNewDirectoryPage();
	if(dirPagePointer < 0)
	{
		return DIRPAGECREATIONERROR;
	}
	// Create a SysTable Entry for the table
	// Create a new SysTableEntry object.....
	SysTableEntry newSysTableEntry;
	newSysTableEntry.setTableName(tableName);
	newSysTableEntry.setDBName(dbName);
	newSysTableEntry.setTableType(tableType);
	newSysTableEntry.setColCount(colCount);
	newSysTableEntry.setRowFormat(rowFormat);
	newSysTableEntry.setTableRows(tableRows);
	newSysTableEntry.setNoPages(noOfPages);
	newSysTableEntry.setRemarks(remarks);
	newSysTableEntry.setKeyColumns(keyColumns);
	newSysTableEntry.setDataLength(dataLength);
	newSysTableEntry.setRecLength(recLength);
	newSysTableEntry.setEncodingScheme(encodingScheme);
	newSysTableEntry.setAvgRowLength(avgRowLength);
	newSysTableEntry.setDirectoryPagePointer(dirPagePointer);

	int sysTableEntryCreateResult = createNewSysTablesEntry(newSysTableEntry);
	// int sysTableEntryCreateResult = createNewSysTablesEntry(tableName,dbName,'T',colCount,rowFormat,0,0,NULL,keyColumns,dataLength,recLength,'A',0,dirPagePointer);
//(string tableName,string dbName,char tableType,unsigned short colCount,char rowFormat,int tableRows,int noOfPages,string remarks,unsigned short keyColumns,unsigned short dataLength,unsigned short recordLength,char encodingScheme,int avgRowLength,int directoryPagePointer)

	if(sysTableEntryCreateResult < 0)
	{
		// Return error
		return SYSTABLEENTRYCREATEERROR;
	}

	for(i=0;i<colCount;i++)
	{
		SysColumnsEntry newSysColumnEntry;
		short dataType = retDataTypeID((q->columns[i]).type);
		string col_default;
		int curRecordLength=0,curDataLength = 0;
		// get the default value for the column, if default is not set, get the default values from DataTypes.cpp file
		if(strncmp((q->columns[i]).type,"VARCHAR$",8)==0 || strncmp((q->columns[i]).type,"CHAR$$$$",8)==0)
		{
			col_default = getDefaultValue(dataType,(q->columns[i]).sizeofField1);
		}
		else
		{
			col_default = getDefaultValue(dataType,1);
		}

		// Getting the Record length
		if(strncmp((q->columns[i]).type,"VARCHAR$",8)==0 || strncmp((q->columns[i]).type,"CHAR$$$$",8)==0)
		{
			charLength = (q->columns[i]).sizeofField1; // length of the character or varchar
			short dataTypeID = retDataTypeID((q->columns[i]).type);
			curDataLength = retDataTypeSize(dataTypeID,charLength);
			if(strcmp((q->columns[i]).type,"VARCHAR$")==0)
				curRecordLength = curDataLength + sizeof(short);
			else
				curRecordLength = curDataLength;
		}
		else
		{
			short dataTypeID = retDataTypeID((q->columns[i]).type);
			curDataLength = retDataTypeSize(dataTypeID,1);
			curRecordLength = curDataLength;
		}

		char autoGenerated = ' ';
		
		if((q->columns[i]).isAutoIncrement == 1 && strncmp((q->columns[i]).type,"INT$$$$$",8) == 0)
			autoGenerated = 'A';
		else if((q->columns[i]).isAutoIncrement == 1 && strncmp((q->columns[i]).type,"INT$$$$$",8) == 0)
		{
			// Other columns cannot be auto incremented
			return SYSCOLUMNSAUTOINCREMENTERROR;
		}
		else if(/*Check for default set*/(q->columns[i]).isDefault == 1)
		{
			// Get the default value as well
			autoGenerated = 'D';
			string newColDefault;
			if(strncmp((q->columns[i]).type,"INT$$$$$",8) == 0 || strncmp((q->columns[i]).type,"UINT$$$$",8) == 0)
			{
				// DataType is an integer
				int DefaultInt = (q->columns[i]).defaultint;
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				memcpy(defString,&DefaultInt,sizeof(int));
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i]).type,"SMALLINT",8) == 0 || strncmp((q->columns[i]).type,"USMALL$$",8) == 0)
			{
				// DataType is a short
				short DefaultShort = (q->columns[i]).defaultshort;
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				memcpy(defString,&DefaultShort,sizeof(short));
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i]).type,"BIGINT$$",8) == 0 || strncmp((q->columns[i]).type,"UBIG$$$$",8) == 0)
			{
				// DataType is a long
				long DefaultLong = (q->columns[i]).defaultlong;
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				memcpy(defString,&DefaultLong,sizeof(long));
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i]).type,"VARCHAR$",8) == 0 || strncmp((q->columns[i].type),"CHAR$$$$",8) == 0)
			{
				// DataType is a char
				string defStr = (q->columns[i]).defaultstring;
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				for(int charCount = 0;charCount<defStr.length();charCount++)
					defString[charCount] = defStr[charCount];
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i]).type,"FLOAT$$$",8) == 0)
			{
				// DataType is an float
				float DefaultFloat = (q->columns[i]).defaultfloat;
				// Round off the float precision here....
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				memcpy(defString,&DefaultFloat,sizeof(float));
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i].type),"DOUBLE$$",8) == 0)
			{
				// DataType is an double
				double DefaultDouble = (q->columns[i]).defaultdouble;
				// Round off the double precision here....
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				memcpy(defString,&DefaultDouble,sizeof(double));
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i]).type,"DATE$$$$",8) == 0)
			{
				// DataType is an date
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				for(int charCount = 0;charCount<8;charCount++)
					defString[charCount] = (q->columns[i]).defaultdate.date[charCount];
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i]).type,"TIME$$$$",8) == 0)
			{
				// DataType is an time
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				for(int charCount = 0;charCount<6;charCount++)
					defString[charCount] = (q->columns[i]).defaultdate.time[charCount];
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}
			else if(strncmp((q->columns[i]).type,"DATETIME",8) == 0)
			{
				// DataType is an datetime
				char * defString = new char [256];
				
				for(int charCount = 0;charCount<256;charCount++)
					defString[charCount] = '$';
				for(int charCount = 0;charCount<8;charCount++)
					defString[charCount] = (q->columns[i]).defaultdate.date[charCount];
				for(int charCount = 8;charCount<14;charCount++)
					defString[charCount] = (q->columns[i]).defaultdate.time[charCount-8];
				for(int charCount = 0;charCount<256;charCount++)
					newColDefault = newColDefault + defString[charCount];
				delete defString;
			}

			col_default = newColDefault;
		}
		newSysColumnEntry.setTableName(tableName);
		newSysColumnEntry.setDBName(dbName);
		newSysColumnEntry.setColumnName((q->columns[i]).name);
		newSysColumnEntry.setColumnDefault(col_default);
		newSysColumnEntry.setDataType((q->columns[i]).type);
		newSysColumnEntry.setRemarks(NULL);
		if((q->columns[i]).isNullable == 1)
			newSysColumnEntry.setIsNullable('Y');
		else
			newSysColumnEntry.setIsNullable('N');
		newSysColumnEntry.setUpdatable('Y');
		newSysColumnEntry.setForeignKey('N');
		newSysColumnEntry.setGeneratedAttribute(autoGenerated);
		newSysColumnEntry.setHidden('N');
		if(strcmp((q->columns[i]).type,"DOUBLE$$")==0)
			newSysColumnEntry.setScale(9);
		else if(strcmp((q->columns[i]).type,"FLOAT$$$")==0)
			newSysColumnEntry.setScale(5);
		else
			newSysColumnEntry.setScale(0);
		if((q->columns[i]).isPrimary == 1)
			newSysColumnEntry.setKeyPos(i);
		else
			newSysColumnEntry.setKeyPos(0);
		newSysColumnEntry.setOrdinalPosition(i);
		newSysColumnEntry.setLength(curRecordLength);

		int sysColumnEntryCreateResult = createNewSysColumnsEntry(newSysColumnEntry);

		//int sysColumnEntryCreateResult = createNewSysColumnsEntry(dbName,tableName,colName,i+1,col_default,isNullable,dataType,length,scale,isUpdatable,remarks,keyPos,foreignKey,generatedAttribute,isHidden);
//(string _dbName;string _tableName;string _columnName;int _ordinalPosition;string _columnDefault;char _isNullable;string _dataType;int _length; short _scale;char _updatable;string _remarks;short _keyPos;char _foreignKey;char _generatedAttribute;char _hidden;)

		if(sysColumnEntryCreateResult < 0)
		{
			// Return error
			return SYSCOLUMNENTRYCREATEERROR;
		}
	}

	if(colIndex != -1)
	{
		// Primary key exists Entry in SysIndex should be created
		string indexName = (q->columns[colIndex]).name;
		string indexType = (q->columns[colIndex]).type;
		int charLength = (q->columns[colIndex]).sizeofField1;
		short curDataLength = 0,curRecordLength = 0;

		// Getting the Record length
		if(strncmp((q->columns[colIndex]).type,"VARCHAR$",8)==0 || strncmp((q->columns[colIndex]).type,"CHAR$$$$",8)==0)
		{
			short dataTypeID = retDataTypeID((q->columns[colIndex]).type);
			curDataLength = retDataTypeSize(dataTypeID,charLength);
			if(strcmp((q->columns[colIndex]).type,"VARCHAR$")==0)
				curRecordLength = curDataLength + sizeof(short);
			else
				curRecordLength = curDataLength;
		}
		else
		{
			short dataTypeID = retDataTypeID((q->columns[i]).type);
			curDataLength = retDataTypeSize(dataTypeID,1);
			curRecordLength = curDataLength;
		}

		// Call the Index Query code here.......
		int rootPageID = 6;

		SysIndexEntry newSysIndexEntry;
		newSysIndexEntry.setIndexName(indexName);
		newSysIndexEntry.setDBName(dbName);
		newSysIndexEntry.setTableName(tableName);
		newSysIndexEntry.setUniqueRule('P');
		newSysIndexEntry.setColCount(1);
		newSysIndexEntry.setFanOutNo(3);
		newSysIndexEntry.setIndexPageID(rootPageID);
		newSysIndexEntry.setPageSize(_pageSize);
		newSysIndexEntry.setEraseRule('N');
		newSysIndexEntry.setCloseRule('N');
		newSysIndexEntry.setRemarks("None");
		newSysIndexEntry.setAvgKeyLength(curRecordLength);

		int sysIndexEntryCreateResult = createNewSysIndexEntry(newSysIndexEntry);

		if(sysIndexEntryCreateResult < 0)
		{
			// Return error
			return SYSINDEXENTRYCREATEERROR;
		}
	}

	// Successful
	return 1;
}

int DB::dropTable(query q)
{
	// For drop table command
	// Delete all the data pages
	// Delete all the directory pages
	// Delete index if it exists
	// Delete all the entries related to SysColumns
	// Delete the SysTable entry for the table
}

int DB::createIndex(query q)
{
	// For create index command
	// Check if index name already exists... If so return error....
	// Check if for the table, index already exists on that key.... If so return error.....
	// Add sysIndex entry
	// If index is created before any insertions..... Then nothing to do from here on return....
	// Else
	// Call B+ tree code with data pages' data and its corresponding entry in the directory entry
	// Pass Data and RID(PageNo,SlotID pair)
}

int DB::dropIndex(query q)
{
	// For drop index command
	// Recover all the pages related to the index
	// Delete the sysIndex entry
}

int DB::insertEntry(query q)
{
	// For insert command
	// Index check required
	// Send index type and key value to check if the index entry can be created.....
	// If yes, then, insert the entry, and send the key,dataPage,slotPointer to the index for inserting into the B+ tree
	// If no, flag an error telling entry already exists
	// Check whether entry has to be inserted and then the index updated or index will provide me the location to insert
	// If SysTables Entry exists, and SysColumns Entries are 0 for the table, then return error COLUMNSNOTSPECIFIEDERROR

	string tableName = q->table;
	string dbName;

	BufferManager *bu = BufferManager::getBufferManager();
	short priority = 3;
	bool retReadDB;

	// To get the dbName, have to read the DBHeader
	char * dbBuff = new char [_pageSize];
	retReadDB = (*bu).readDB(fdID,_dbHeaderPTR,(PagePriority)(priority),dbBuff);
	if(retReadDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		return DATABASEREADDBERROR;
	}
	DBHeader * curDBHeader = new DBHeader(dbBuff,_pageSize);
	dbName = curDBHeader->getDatabaseName();
	delete curDBHeader;
	delete dbBuff;

	// Check whether table exists
	int nextSysTabPointer = _sysTablesPTR, nextSysColumnsPointer = _sysColumnsPTR;
	int curSysTabPointer = -1,curSysColsPointer = -1;
		
	int entryPosition = -1,entryPage = -1;

	while(nextSysTabPointer != -1)
	{
		char * curSysTabBuffer = new char [_pageSize];
		// Read the file to the buffer
		retReadDB = (*bu).readDB(fdID,nextSysTabPointer,(PagePriority)(priority),curSysTabBuffer);
		if(retReadDB == false)
		{
			// Unable to read the buffer
			// Return error
			delete curSysTabBuffer;
			return DATABASEREADDBERROR;
		}
		SysTables * curSysTable = new SysTables(curSysTabBuffer,_pageSize);
		curSysTabPointer = nextSysTabPointer;
		nextSysTabPointer = curSysTable->getnextSysTablePage();
		entryPosition = curSysTable->searchSysTableEntry(tableName,dbName,curSysTabBuffer);
		if(entryPosition != -1)
		{
			entryPage = curSysTabPointer;
			delete curSysTable;
			delete curSysTabBuffer;
			break;
		}
		delete curSysTable;
		delete curSysTabBuffer;
	}

	if(entryPage == -1)
	{
		// Table name not found error
		return INSERTTABLETBLNOTFOUNDERROR;
	}

	char * sysTabBuffer = new char [_pageSize];
	retReadDB = (*bu).readDB(fdID,entryPage,(PagePriority)(priority),sysTabBuffer);
	if(retReadDB == false)
	{
		// Unable to read the buffer
		// Return error
		delete sysTabBuffer;
		return DATABASEREADDBERROR;
	}

	char * newEntryBuff = new char [SYSTABLEENTRYSIZE];
	memcpy(newEntryBuff,&sysTabBuffer[0+((entryPosition-1)*SYSTABLEENTRYSIZE)],SYSTABLEENTRYSIZE);

	SysTableEntry newSysTablesEntry;
	newSysTablesEntry.getDataBuffer(newEntryBuff);

	delete newEntryBuff;
	delete sysTabBuffer;

	int recordLength = newSysTablesEntry.getRecLength();
	//short keyCols = newSysTablesEntry.getKeyColumns();
	short colCount = newSysTablesEntry.getColCount();
	int noOfRows = newSysTablesEntry.getTableRows();
	int dirPageEntry = newSysTablesEntry.getDirectoryPagePointer();
	int noOfPages = newSysTablesEntry.getNoPages();
	bool isTableDynamic = false;
	if(newSysTablesEntry.getRowFormat() != 'F')
		isTableDynamic = true;
	bool defaultValues = false,checkNullable = false;
	bool * insertDataFlags = new bool [colCount];
	string * insertedValues = new string [colCount];

	if(colCount != q->cntColumns)
	{
		// We have to get the default values or some fields can be Null
		defaultValues = true;
		checkNullable = true;
	}

	// Populate these values for all the columns

	string * defaultStrings = new string [colCount];
	string * dataTypes = new string [colCount];
	string * columnNames = new string [colCount];
	int * ordinalPositions = new int [colCount];
	char * isNullables = new char [colCount];
	char * genAttributes = new char [colCount];
	int * columnLengths = new int [colCount];
	short * scales = new short [colCount];
	short * keyPositions = new short [colCount];

	char * insertData = new char [recordLength];

	// Get all the data about the columns

	for(short curColCount = 0;curColCount<colCount;curColCount++)
	{
		// Get the SysColumns entry for the column at curColCount position
		insertDataFlags[curColCount] = false;
		char * curSysColBuff = new char [_pageSize];
		while(nextSysColumnsPointer != -1)
		{
			retReadDB = (*bu).readDB(fdID,nextSysColumnsPointer,(PagePriority)(priority),sysTabBuffer);
			if(retReadDB == false)
			{
				// Unable to read the buffer
				// Return error
				delete curSysColBuff;
				delete insertData;
				delete insertDataFlags;
				delete insertedValues;
				delete defaultStrings;
				delete columnNames;
				delete dataTypes;
				delete ordinalPositions;
				delete isNullables;
				delete genAttributes;
				delete columnLengths;
				delete scales;
				delete keyPositions;
				return DATABASEREADDBERROR;
			}
		}
		SysColumns * curSysColumns = new SysColumns(curSysColBuff,_pageSize);
		curSysColsPointer = nextSysColumnsPointer;
		nextSysColumnsPointer = curSysColumns->getnextSysColumnsPage();

		int resSearchSysCols = curSysColumns->searchSysColumnEntry(curColCount,tableName,curSysColBuff);
		if(resSearchSysCols > 0)
		{
			char * curSysColumnsEntry = new char [SYSCOLUMNENTRYSIZE];
			memcpy(curSysColumnsEntry,&curSysColBuff[0+((resSearchSysCols-1)*SYSCOLUMNENTRYSIZE)],SYSCOLUMNENTRYSIZE);
			SysColumnsEntry curEntry;
			curEntry.getDataBuffer(curSysColumnsEntry);
			columnNames[curColCount] = curEntry.getColumnName();
			dataTypes[curColCount] = curEntry.getColumnName();
			ordinalPositions[curColCount] = curEntry.getOrdinalPosition();
			defaultStrings[curColCount] = curEntry.getColumnDefault();
			isNullables[curColCount] = curEntry.getIsNullable();
			genAttributes[curColCount] = curEntry.getGeneratedAttribute();
			columnLengths[curColCount] = curEntry.getLength();
			scales[curColCount] = curEntry.getScale();
			keyPositions[curColCount] = curEntry.getKeyPos();
			delete curSysColumnsEntry;
		}
		delete curSysColumns;
		delete curSysColBuff;
	}

	int insertPos = 0;
	string testDefault = (q->ins[0]).colname;
	if(strncmp(testDefault.c_str(),"###",3) == 0)
	{
		// This is the default way of entering the data
		for(short curColCount = 0;curColCount<colCount;curColCount++)
		{
			if(colCount != q->cntColumns)
			{
				// We have to get the default values or some fields can be Null
				defaultValues = true;
				checkNullable = true;
			}
			insertDataFlags[curColCount] = false;

			char * curSysColBuff = new char [_pageSize];
			while(nextSysColumnsPointer != -1)
			{
				retReadDB = (*bu).readDB(fdID,nextSysColumnsPointer,(PagePriority)(priority),sysTabBuffer);
				if(retReadDB == false)
				{
					// Unable to read the buffer
					// Return error
					delete curSysColBuff;
					delete insertData;
					delete insertDataFlags;
					delete insertedValues;
					delete defaultStrings;
					delete columnNames;
					delete dataTypes;
					delete ordinalPositions;
					delete isNullables;
					delete genAttributes;
					delete columnLengths;
					delete scales;
					delete keyPositions;
					return DATABASEREADDBERROR;
				}
			}
			SysColumns * curSysColumns = new SysColumns(curSysColBuff,_pageSize);
			curSysColsPointer = nextSysColumnsPointer;
			nextSysColumnsPointer = curSysColumns->getnextSysColumnsPage();

			int resSearchSysCols = curSysColumns->searchSysColumnEntry(curColCount,tableName,curSysColBuff);
			if(resSearchSysCols > 0)
			{
				char * curSysColumnsEntry = new char [SYSCOLUMNENTRYSIZE];
				memcpy(curSysColumnsEntry,&curSysColBuff[0+((resSearchSysCols-1)*SYSCOLUMNENTRYSIZE)],SYSCOLUMNENTRYSIZE);
				SysColumnsEntry curEntry;
				curEntry.getDataBuffer(curSysColumnsEntry);
				string columnDataType = curEntry.getDataType();
				int columnLength = curEntry.getLength();
				if(strncmp((q->ins[curColCount]).str,"###",3) == 0)
				{
					// Default string has to be inserted or it is NULL
					if(curEntry.getIsNullable() == 'N' && (curEntry.getGeneratedAttribute() != 'D' || curEntry.getGeneratedAttribute() != 'A'))
					{
						delete curSysColBuff;
						delete curSysColumns;
						delete insertData;
						delete insertDataFlags;
						delete insertedValues;
						delete defaultStrings;
						delete columnNames;
						delete dataTypes;
						delete ordinalPositions;
						delete isNullables;
						delete genAttributes;
						delete columnLengths;
						delete scales;
						delete keyPositions;
						return INSERTTABLEDATANULLINSERTERROR;
					}
					string columnDefaultString = curEntry.getColumnDefault();
					if(curEntry.getGeneratedAttribute() == 'D')
					{
						if(strncmp(columnDataType.c_str(),"VARCHAR$",8) == 0)
						{
							char * currentData = new char [columnLength+sizeof(short)+sizeof(int)];
							short offset = sizeof(int)+columnLength;
							memcpy(&currentData[0],&offset,sizeof(short));
							memcpy(&currentData[0+sizeof(short)],&columnLength,sizeof(int));
							memcpy(&currentData[0+sizeof(short)+sizeof(int)],columnDefaultString.c_str(),columnLength);
							memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short)+sizeof(int));
							insertPos = insertPos+columnLength+sizeof(short)+sizeof(int);
							insertDataFlags[curColCount] = true;
							insertedValues[curColCount] = columnDefaultString;
							delete currentData;
						}
						else
						{
							char * currentData = new char [columnLength+sizeof(short)];
							short offset = columnLength;
							memcpy(&currentData[0],&offset,sizeof(short));
							memcpy(&currentData[0+sizeof(short)],columnDefaultString.c_str(),columnLength);
							memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
							insertPos = insertPos+columnLength+sizeof(short);
							insertDataFlags[curColCount] = true;
							insertedValues[curColCount] = "";
							for(int k1 = 1;k1<columnLength;k1++)
								insertedValues[curColCount] = insertedValues[curColCount]+columnDefaultString[k1];
							delete currentData;
						}
					}
					else if(curEntry.getGeneratedAttribute() == 'A')
					{
						if(strncmp(columnDataType.c_str(),"INT$$$$$",8) == 0)
						{
							int curRows = noOfRows+1;
							char * currentData = new char [columnLength+sizeof(short)];
							short offset = columnLength;
							memcpy(&currentData[0],&offset,sizeof(short));
							memcpy(&currentData[0+sizeof(short)],&curRows,columnLength);
							memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
							insertPos = insertPos+columnLength+sizeof(short);
							insertDataFlags[curColCount] = true;
							insertedValues[curColCount] = curRows;
							delete currentData;
						}
					}
				}
				else
				{
					// Data from the column structure
					string dataToInsert = (q->ins[curColCount]).str;
					if(dataToInsert.length() > columnLength)
					{
						delete curSysColBuff;
						delete curSysColumns;
						delete insertData;
						delete insertDataFlags;
						delete defaultStrings;
						delete columnNames;
						delete dataTypes;
						delete ordinalPositions;
						delete isNullables;
						delete genAttributes;
						delete columnLengths;
						delete scales;
						delete keyPositions;
						return INSERTTABLEDATABOUNDARYEXCEEDSERROR;
					}
					if((strncmp(columnDataType.c_str(),"INT$$$$$",8) == 0) || (strncmp(columnDataType.c_str(),"UINT$$$$",8) == 0))
					{
						int curData = atoi(dataToInsert.c_str());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
					}
					else if((strncmp(columnDataType.c_str(),"SMALLINT",8) == 0) || (strncmp(columnDataType.c_str(),"USMALL$$",8) == 0))
					{
						short curData = atoi(dataToInsert.c_str());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
					}
					else if((strncmp(columnDataType.c_str(),"BIGINT$$",8) == 0) || (strncmp(columnDataType.c_str(),"UBIG$$$$",8) == 0))
					{
						long curData = atol(dataToInsert.c_str());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
					}
					else if((strncmp(columnDataType.c_str(),"FLOAT$$$",8) == 0))
					{
						float curData = atof(dataToInsert.c_str());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
					}
					else if((strncmp(columnDataType.c_str(),"DOUBLE$$",8) == 0))
					{
						double curData = atof(dataToInsert.c_str());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
					}
					else if((strncmp(columnDataType.c_str(),"CHAR$$$$",8) == 0))
					{
						char * curData = new char [columnLength];
						for(int j1 = 0;j1<columnLength;j1++)
							curData[j1] = '$';
						memcpy(curData,dataToInsert.c_str(),dataToInsert.length());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
						delete curData;
					}
					else if((strncmp(columnDataType.c_str(),"VARCHAR$",8) == 0))
					{
						char * curData = new char [dataToInsert.length()];
						memcpy(curData,dataToInsert.c_str(),dataToInsert.length());
						char * currentData = new char [dataToInsert.length()+sizeof(short)];
						short offset = dataToInsert.length();
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,dataToInsert.length());
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+dataToInsert.length()+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
					}
					else if((strncmp(columnDataType.c_str(),"DATE$$$$",8) == 0))
					{
						dataToInsert = ((q->ins[curColCount]).d)->date;
						char * curData = new char [columnLength];
						memcpy(curData,dataToInsert.c_str(),dataToInsert.length());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
						delete curData;
					}
					else if((strncmp(columnDataType.c_str(),"TIME$$$$",8) == 0))
					{
						dataToInsert = ((q->ins[curColCount]).d)->time;
						char * curData = new char [columnLength];
						memcpy(curData,dataToInsert.c_str(),dataToInsert.length());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
						delete curData;
					}
					else if((strncmp(columnDataType.c_str(),"DATETIME",8) == 0))
					{
						dataToInsert = ((q->ins[curColCount]).d)->date;
						char * curData = new char [columnLength];
						memcpy(curData,dataToInsert.c_str(),dataToInsert.length());
						dataToInsert = ((q->ins[curColCount]).d)->time;
						memcpy(&curData[8*sizeof(char)],dataToInsert.c_str(),dataToInsert.length());
						char * currentData = new char [columnLength+sizeof(short)];
						short offset = columnLength;
						memcpy(&currentData[0],&offset,sizeof(short));
						memcpy(&currentData[0+sizeof(short)],&curData,columnLength);
						memcpy(&insertData[insertPos],currentData,columnLength+sizeof(short));
						insertPos = insertPos+columnLength+sizeof(short);
						insertDataFlags[curColCount] = true;
						insertedValues[curColCount] = dataToInsert;
						delete currentData;
						delete curData;
					}
				}
				delete curSysColumnsEntry;
				if(insertDataFlags[curColCount] == false)
				{
					// Data was not provided error. Return error.
					delete curSysColBuff;
					delete curSysColumns;
					delete insertData;
					delete insertDataFlags;
					delete insertedValues;
					delete defaultStrings;
					delete columnNames;
					delete dataTypes;
					delete ordinalPositions;
					delete isNullables;
					delete genAttributes;
					delete columnLengths;
					delete scales;
					delete keyPositions;
					return INSERTTABLEDATANOTINSERTERROR;
				}
			}
			else
			{
				// return column not found error
				delete curSysColBuff;
				delete curSysColumns;
				delete insertData;
				delete insertDataFlags;
				delete insertedValues;
				delete defaultStrings;
				delete columnNames;
				delete dataTypes;
				delete ordinalPositions;
				delete isNullables;
				delete genAttributes;
				delete columnLengths;
				delete scales;
				delete keyPositions;
				return INSERTTABLECOLNOTFOUNDERROR;
			}
			delete curSysColumns;
			delete curSysColBuff;
		}
	}
	else
	{
		// This is the non-default way of entering the data
		for(short curColCount = 0;curColCount<colCount;curColCount++)
		{
			short entryPosition = -1;
			for(short columnCount = 0;columnCount<(q->cntColumns);columnCount++)
			{
				string columnName = (q->ins[columnCount]).colname;
				if(columnNames[curColCount] == columnName)
				{
					entryPosition = columnCount;
					break;
				}
			}
			if(entryPosition == -1)
			{
				// Data was not provided....
				// Check whether it is autoGenerated by default
				if(genAttributes[curColCount] == 'D')
				{
					// Insert the default string
					
				}
				else
				{
					if(isNullables[curColCount] == 'Y')
					{

					}
				}
			}
			else
			{
				string data = (q->ins[entryPosition]).str;
				if(strncmp(dataTypes[curColCount].c_str(),"VARCHAR$",8) == 0 || strncmp(dataTypes[curColCount].c_str(),"CHAR$$$$",8) == 0)
				{
				
				}
			}
		}
	}

	bool * noOfPagesChanged = new bool [1];

	int resultInsert = insertDataBaseEntry(dirPageEntry,insertData,noOfPagesChanged);

	// Change the no. of rows and no. of pages for the SysTables entry.
	nextSysTabPointer = _sysTablesPTR;
	curSysTabPointer = -1;

	entryPosition = -1,entryPage = -1;
	// This is not necessary check whether you can reuse the old entryPosition and entryPage...

	while(nextSysTabPointer != -1)
	{
		char * curSysTabBuffer = new char [_pageSize];
		// Read the file to the buffer
		retReadDB = (*bu).readDB(fdID,nextSysTabPointer,(PagePriority)(priority),curSysTabBuffer);
		if(retReadDB == false)
		{
			// Unable to write the buffer
			// Return error
			delete newEntryBuff;
			delete curSysTabBuffer;
			delete insertData;
			delete insertDataFlags;
			delete noOfPagesChanged;
			delete insertedValues;
			delete defaultStrings;
			delete columnNames;
			delete dataTypes;
			delete ordinalPositions;
			delete isNullables;
			delete genAttributes;
			delete columnLengths;
			delete scales;
			delete keyPositions;
			return DATABASEREADDBERROR;
		}
		SysTables * curSysTable = new SysTables(curSysTabBuffer,_pageSize);
		curSysTabPointer = nextSysTabPointer;
		nextSysTabPointer = curSysTable->getnextSysTablePage();
		entryPosition = curSysTable->searchSysTableEntry(tableName,dbName,curSysTabBuffer);
		if(entryPosition != -1)
		{
			entryPage = curSysTabPointer;
			// Entry found Update it.....
			char * newEntryBuff = new char [SYSTABLEENTRYSIZE];
			memcpy(newEntryBuff,&curSysTabBuffer[0+((entryPosition-1)*SYSTABLEENTRYSIZE)],SYSTABLEENTRYSIZE);

			SysTableEntry newSysTablesEntry;
			newSysTablesEntry.getDataBuffer(newEntryBuff);

			noOfRows++;
			newSysTablesEntry.setTableRows(noOfRows);
			if(*noOfPagesChanged == true)
			{
				noOfPages++;
				newSysTablesEntry.setNoPages(noOfPages);
			}

			newSysTablesEntry.fillBuffer(newEntryBuff);
			memcpy(&curSysTabBuffer[0+((entryPosition-1)*SYSTABLEENTRYSIZE)],newEntryBuff,SYSTABLEENTRYSIZE);

			bool retWriteDB;
			retWriteDB = (*bu).writeDB(fdID,entryPage,(PagePriority)(priority),curSysTabBuffer);
			if(retWriteDB == false)
			{
				// Unable to write the buffer
				// Return error
				delete newEntryBuff;
				delete curSysTable;
				delete curSysTabBuffer;
				delete insertData;
				delete insertDataFlags;
				delete noOfPagesChanged;
				delete insertedValues;
				delete defaultStrings;
				delete columnNames;
				delete dataTypes;
				delete ordinalPositions;
				delete isNullables;
				delete genAttributes;
				delete columnLengths;
				delete scales;
				delete keyPositions;
				return DATABASEWRITEDBERROR;
			}
			delete newEntryBuff;
			delete curSysTable;
			delete curSysTabBuffer;
			break;
		}
		delete curSysTable;
		delete curSysTabBuffer;
	}

	// If index is there, we have to insert in them the data

	delete insertData;
	delete noOfPagesChanged;
	delete insertDataFlags;
	delete insertedValues;

	delete defaultStrings;
	delete columnNames;
	delete dataTypes;
	delete ordinalPositions;
	delete isNullables;
	delete genAttributes;
	delete columnLengths;
	delete scales;
	delete keyPositions;

	return 1;
}

int DB::deleteEntry(query q)
{
	// For delete command
	// Index check required
	// On delete entry, update the total free space for the data page entry in the Directory page. This is a must!!!!
	// Update all the indices as well

	string debugMessage;
	bool retWriteLog;

	string tableName = q->table;

	// Check if index can be used - or and not is not there in query and columns have index Entries
	// Returns which index to use as well - Mostly index based on primary key, otherwise last index used
	// Similar to insert query, create arrays for indexes as well and then use them

	// Get the Directory page pointer
	// Create SysColumn columnNames and dataTypes arrays
	// The result to delete or not will be from queryEvaluate()

	string dbName;
	int results = 0;

	BufferManager *bu = BufferManager::getBufferManager();
	short priority = 3;
	bool retReadDB;

	// To get the dbName, have to read the DBHeader
	char * dbBuff = new char [_pageSize];
	retReadDB = (*bu).readDB(fdID,_dbHeaderPTR,(PagePriority)(priority),dbBuff);
	if(retReadDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete dbBuff;
		return DATABASEREADDBERROR;
	}
	DBHeader * curDBHeader = new DBHeader(dbBuff,_pageSize);
	dbName = curDBHeader->getDatabaseName();
	delete curDBHeader;
	delete dbBuff;

	// Check whether table exists
	int nextSysTabPointer = _sysTablesPTR, nextSysColumnsPointer = _sysColumnsPTR;
	int curSysTabPointer = -1,curSysColsPointer = -1;
		
	int entryPosition = -1,entryPage = -1;

	while(nextSysTabPointer != -1)
	{
		char * curSysTabBuffer = new char [_pageSize];
		// Read the file to the buffer
		retReadDB = (*bu).readDB(fdID,nextSysTabPointer,(PagePriority)(priority),curSysTabBuffer);
		if(retReadDB == false)
		{
			// Unable to read the buffer
			// Return error
			delete curSysTabBuffer;
			return DATABASEREADDBERROR;
		}
		SysTables * curSysTable = new SysTables(curSysTabBuffer,_pageSize);
		curSysTabPointer = nextSysTabPointer;
		nextSysTabPointer = curSysTable->getnextSysTablePage();
		entryPosition = curSysTable->searchSysTableEntry(tableName,dbName,curSysTabBuffer);
		if(entryPosition != -1)
		{
			entryPage = curSysTabPointer;
			delete curSysTable;
			delete curSysTabBuffer;
			break;
		}
		delete curSysTable;
		delete curSysTabBuffer;
	}

	if(entryPage == -1)
	{
		// Table name not found error
		return DELETETABLETBLNOTFOUNDERROR;
	}

	char * sysTabBuffer = new char [_pageSize];
	retReadDB = (*bu).readDB(fdID,entryPage,(PagePriority)(priority),sysTabBuffer);
	if(retReadDB == false)
	{
		// Unable to read the buffer
		// Return error
		delete sysTabBuffer;
		return DATABASEREADDBERROR;
	}

	char * newEntryBuff = new char [SYSTABLEENTRYSIZE];
	memcpy(newEntryBuff,&sysTabBuffer[0+((entryPosition-1)*SYSTABLEENTRYSIZE)],SYSTABLEENTRYSIZE);

	SysTableEntry newSysTablesEntry;
	newSysTablesEntry.getDataBuffer(newEntryBuff);

	delete newEntryBuff;
	delete sysTabBuffer;

	int recordLength = newSysTablesEntry.getRecLength();
	short keyCols = newSysTablesEntry.getKeyColumns();
	short colCount = newSysTablesEntry.getColCount();
	int noOfRows = newSysTablesEntry.getTableRows();
	int dirPageEntry = newSysTablesEntry.getDirectoryPagePointer();
	int noOfPages = newSysTablesEntry.getNoPages();
	bool isTableDynamic = false;
	if(newSysTablesEntry.getRowFormat() != 'F')
		isTableDynamic = true;

	string * dataTypes = new string [colCount];
	string * columnNames = new string [colCount];
	int * ordinalPositions = new int [colCount];
	int * columnLengths = new int [colCount];
	short * scales = new short [colCount];

	for(short curColCount = 0;curColCount<colCount;curColCount++)
	{
		// Get the SysColumns entry for the column at curColCount position
		char * curSysColBuff = new char [_pageSize];
		while(nextSysColumnsPointer != -1)
		{
			retReadDB = (*bu).readDB(fdID,nextSysColumnsPointer,(PagePriority)(priority),curSysColBuff);
			if(retReadDB == false)
			{
				// Unable to read the buffer
				// Return error
				delete curSysColBuff;
				delete dataTypes;
				delete columnNames;
				delete ordinalPositions;
				delete scales;
				delete columnLengths;
				return DATABASEREADDBERROR;
			}
			SysColumns * curSysColumns = new SysColumns(curSysColBuff,_pageSize);
			curSysColsPointer = nextSysColumnsPointer;
			nextSysColumnsPointer = curSysColumns->getnextSysColumnsPage();

			int resSearchSysCols = curSysColumns->searchSysColumnEntry(curColCount,tableName,curSysColBuff);
			if(resSearchSysCols > 0)
			{
				char * curSysColumnsEntry = new char [SYSCOLUMNENTRYSIZE];
				memcpy(curSysColumnsEntry,&curSysColBuff[0+((resSearchSysCols-1)*SYSCOLUMNENTRYSIZE)],SYSCOLUMNENTRYSIZE);
				SysColumnsEntry curEntry;
				curEntry.getDataBuffer(curSysColumnsEntry);
				columnNames[curColCount] = curEntry.getColumnName();
				dataTypes[curColCount] = curEntry.getColumnName();
				ordinalPositions[curColCount] = curEntry.getOrdinalPosition();
				columnLengths[curColCount] = curEntry.getLength();
				scales[curColCount] = curEntry.getScale();
				delete curSysColumnsEntry;
			}
			delete curSysColumns;
		}
		delete curSysColBuff;
	}

	int nextPTR = dirPageEntry;
	int curPTR = -1;

	while(nextPTR != -1)
	{
		char * deBuf = new char [_pageSize];
		short dirPagePriority = 2,dataPagePriority = 1;
		bool retReadDB,retWriteDB;
		//int res = pageRead(fileName,nextPTR,deBuf,_pageSize);
		retReadDB = (*bu).readDB(fdID,nextPTR,(PagePriority)(dirPagePriority),deBuf);
		if(retReadDB == false)
		{
			// Unable to read the buffer
			// Return error
			delete deBuf;
			delete dataTypes;
			delete columnNames;
			delete ordinalPositions;
			delete scales;
			delete columnLengths;
			return DATABASEREADDBERROR;
		}
		DirectoryPage * directoryPage = new DirectoryPage(deBuf,_pageSize);
		curPTR = nextPTR;
		nextPTR = directoryPage->getNextDirectoryPagePointer();
		int noOfEntries = directoryPage->getNoE();
		if(noOfEntries == 0)
		{
			if(debugFlag == true)
			{
				debugMessage = "There are no entries in this page for data page "+curPTR;
				retWriteLog = writeLog(debugMessage);
			}
		}
		for(int i = 0;i < noOfEntries;i++)
		{
			int resCount = 0;
			char * entryBuffer = new char [DESIZE];
			memcpy(entryBuffer,&deBuf[0+i*DESIZE],DESIZE);
			DirectoryPageEntry * newDirectoryPageEntry = new DirectoryPageEntry(entryBuffer);
			int pageID = newDirectoryPageEntry->getPageID();
			int tfs = newDirectoryPageEntry->getTFS();
			char * dataPageBuffer = new char [_pageSize];
			//res = pageRead(fileName,pageID,dataPageBuffer,_pageSize);
			retReadDB = (*bu).readDB(fdID,pageID,(PagePriority)(dataPagePriority),dataPageBuffer);
			if(retReadDB == false)
			{
				// Unable to read the buffer
				// Return error
				delete deBuf;
				delete directoryPage;
				delete newDirectoryPageEntry;
				delete entryBuffer;
				delete dataPageBuffer;
				delete dataTypes;
				delete columnNames;
				delete ordinalPositions;
				delete scales;
				delete columnLengths;
				return DATABASEREADDBERROR;
			}
			DataPage * curDataPage = new DataPage(dataPageBuffer,_pageSize);
			int noOfSlots = curDataPage->getslotCounter();
			if(noOfSlots == 0)
			{
				if(debugFlag == true)
				{
					debugMessage = "There are no entries in this data page"+pageID;
					retWriteLog = writeLog(debugMessage);
				}
			}
			else
			{
				for(int j = 0;j<noOfSlots;j++)
				{
					int slotSize;
					long slotPointer;
					memcpy(&slotPointer,&dataPageBuffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
					memcpy(&slotSize,&dataPageBuffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
					if(slotSize<0)
					{
						// Slot has been already deleted....
						// No need to delete.....
						continue;
					}
					else
					{
						char * dataBuffer = new char [slotSize];
						memcpy(dataBuffer,&dataPageBuffer[slotPointer],slotSize);

						bool toBeDeleted = queryEvaluate(q,columnNames,dataTypes,ordinalPositions,scales,columnLengths);

						if(toBeDeleted == true)
						{
							tfs = tfs + slotSize;
							slotSize = (-1)*slotSize;
							resCount++;
							memcpy(&dataPageBuffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],&slotSize,sizeof(int));
						}
						delete dataBuffer;
					}
				}
				//res = pageWrite(fileName,pageID,dataPageBuffer,_pageSize);
				retWriteDB = (*bu).writeDB(fdID,pageID,(PagePriority)(dataPagePriority),dataPageBuffer);
				if(retWriteDB == false)
				{
					// Unable to read the buffer
					// Return error
					delete deBuf;
					delete directoryPage;
					delete curDataPage;
					delete dataPageBuffer;
					delete newDirectoryPageEntry;
					delete entryBuffer;
					delete dataTypes;
					delete columnNames;
					delete ordinalPositions;
					delete scales;
					delete columnLengths;
					return DATABASEWRITEDBERROR;
				}
			}
			delete curDataPage;
			delete dataPageBuffer;

			newDirectoryPageEntry->setTFS(tfs);
			newDirectoryPageEntry->fillBuffer(entryBuffer);
			memcpy(&deBuf[0+i*DESIZE],entryBuffer,DESIZE);
			delete newDirectoryPageEntry;
			delete entryBuffer;
			results = results + resCount;
		}
		directoryPage->updateMaxTFS(deBuf);
		//pageWrite(fileName,curPTR,deBuf,_pageSize);
		retWriteDB = (*bu).writeDB(fdID,curPTR,(PagePriority)(dirPagePriority),deBuf);
		if(retWriteDB == false)
		{
			// Unable to read the buffer
			// Return error
			delete deBuf;
			delete directoryPage;
			delete dataTypes;
			delete columnNames;
			delete ordinalPositions;
			delete scales;
			delete columnLengths;
			return DATABASEWRITEDBERROR;
		}
		delete directoryPage;
		delete deBuf;
	}

	delete dataTypes;
	delete columnNames;
	delete ordinalPositions;
	delete scales;
	delete columnLengths;

	// Read the file to the buffer
	retReadDB = (*bu).readDB(fdID,entryPage,(PagePriority)(priority),sysTabBuffer);
	if(retReadDB == false)
	{
		// Unable to read the buffer
		// Return error
		delete sysTabBuffer;
		return DATABASEREADDBERROR;
	}

	memcpy(newEntryBuff,&sysTabBuffer[0+((entryPosition-1)*SYSTABLEENTRYSIZE)],SYSTABLEENTRYSIZE);

	newSysTablesEntry.getDataBuffer(newEntryBuff);
	noOfRows = noOfRows - results;
	newSysTablesEntry.setTableRows(noOfRows);
	newSysTablesEntry.fillBuffer(newEntryBuff);
	memcpy(&sysTabBuffer[0+((entryPosition-1)*SYSTABLEENTRYSIZE)],newEntryBuff,SYSTABLEENTRYSIZE);

	bool retWriteDB = (*bu).writeDB(fdID,entryPage,(PagePriority)(priority),sysTabBuffer);
	if(retWriteDB == false)
	{
		// Unable to write the buffer
		// Return error
		delete sysTabBuffer;
		return DATABASEWRITEDBERROR;
	}

	delete newEntryBuff;
	delete sysTabBuffer;
	return results;
}

int DB::updateEntry(query q)
{
	// For update command
	// Index check required
	// If varchar field is getting updated - delete and insert.....
	// Call DataPage.getData giving the slot pointer and size, Compare the data here.
	// If it matches, create new data buffer, from the old data buffer but with modified content at position
	// Mark the slot as deleted and insert the data
	// Else, the field is fixed length, so, just call the modify data page data directly...
	// Delete only after the entry is inserted. Else, do not delete it !!!!
}

int DB::alterTable(query q)
{
	// For alter table command
	// Index check required
}

int DB::selectEntry(query q)
{
	// For select command
	// Index check required
	// Call DataPage.SearchData to get the data search count in each data page
	// This will give the no. of results that match in that data page.....
	// Start filling the result buffer with the entries until it gets filled....
	// As soon as it gets filled, set the dataPage and slotIDs from which to search data for the next time
	// Use DataPage.getSlotEntry and pass the buffer to be filled
	// For the next iteration..., from that data page do DataPage.SearchData, and get data from that position
	// Be careful about the logic for this
	// If $ or #'s are present replace with '\0'
	// Give column delimiters for the entries
}

int DB::showTables(query q)
{
	// For show tables command
}
