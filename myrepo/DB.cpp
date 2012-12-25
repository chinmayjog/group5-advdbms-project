// DB.cpp
// Main DB Class file

#include"DB.h"
#include"DBHeader.h"
#include"SysTables.h"
#include"SysColumns.h"
#include"SysIndex.h"
#include"FreeList.h"

#include"FileHandling.cpp"

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
}

int DB::getFreePage()
{
	int pageID = _freePagePTR;
	char * buffer = new char [PAGESIZE];
	//buffer = readPage(pageID);// Chinmay code here for reading the page
	int nextFreePage;
	memcpy(&nextFreePage,&buffer[NEXTPTR],sizeof(int));

	_freePagePTR = nextFreePage;


	if(extendFreeSpaceCheck() == 1)
		extendFreeSpace();

	delete buffer;
	return pageID;
}

int DB::addFreePageList(int pageID)
{
	char * buffer = new char [PAGESIZE];
	//buffer = readPage(pageID);// Chinmay code here for reading the page

	FreePage * newPage = new FreePage(pageID,_freePagePTR);

	newPage->writeFreePageBuffer(buffer);

	//writePageBuffer(pageID);// Chinmay code here for writing the page
	_freePagePTR = pageID;
	delete buffer;
	return 1;
}

int DB::createNewSysTablesEntry()
{

}

int DB::deleteSysTablesEntry()
{

}

int DB::createNewSysColumnsEntry()
{

}

int DB::deleteSysColumnsEntry()
{

}

int DB::createNewSysIndexEntry()
{

}

int DB::deleteSysIndexEntry()
{

}

int DB::createNewSysTablesPage()
{

}

int DB::createNewSysColumnsPage()
{

}

int DB::createNewSysIndexPage()
{

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


// DB Queries

int DB::createDB(int size,int pageSize,string dbName)
{
	// Add code to check whether database name already exists.......
	// Steps: In the database root get the list of the names of the files
	// Extract the file names....
	// Check by comparing the database name looping over the list of file names
	// If exists return error DBNAMEUNIQUEERROR -2001
	// Else continue
	// Create file of size*pageSize bytes... Call Chinmay's code here
	DBHeader * dbHeader = new DBHeader(size,pageSize,dbName);
	char * dbBuff = new char [pageSize];
	dbHeader->writeDBHeader(dbBuff);
	string fileName = DBROOT;
	fileName = fileName+dbName;
	fileName = fileName+".dat";
	fileCreate(fileName);
	pageWrite(fileName,1,dbBuff,pageSize);
	SysTables * sysTables = new SysTables(2,pageSize);
	char * tabBuff = new char [pageSize];
	//sysTables->setPageSize(pageSize);
	sysTables->writeSysTableBuffer(tabBuff);
	pageWrite(fileName,2,tabBuff,pageSize);
	SysColumns * sysColumns = new SysColumns(3,pageSize);
	char * colBuff = new char [pageSize];
	sysColumns->setPageSize(pageSize);
	sysColumns->writeSysColumnsBuffer(colBuff);
	pageWrite(fileName,3,colBuff,pageSize);
	char * indexBuff = new char [pageSize];
	SysIndex * sysIndex = new SysIndex(4,pageSize);
	sysIndex->writeSysIndexBuffer(indexBuff);
	pageWrite(fileName,4,indexBuff,pageSize);
	delete dbBuff;
	delete tabBuff;
	delete colBuff;
	delete indexBuff;
	delete sysIndex;
	delete sysColumns;
	delete sysTables;
	delete dbHeader;
	int totPages = dbHeader->getTotalPages();
	for(int i = 5;i<=totPages;i++)
	{
		if(i == totPages)
		{
			FreePage * freePage = new FreePage(i,-1,pageSize);
			char *idxbuffer = new char [pageSize];
			freePage->writeFreePageBuffer(idxbuffer);
			pageWrite(fileName,i,idxbuffer,pageSize);
			delete idxbuffer;
			delete freePage;
		}
		else
		{
			FreePage * freePage = new FreePage(i,i+1,pageSize);
			char *idxbuffer = new char [pageSize];
			freePage->writeFreePageBuffer(idxbuffer);
			pageWrite(fileName,i,idxbuffer,pageSize);
			delete idxbuffer;
			delete freePage;
		}
	}

	// Write all buffers to pages......
	// Call Chinmay's code for writing pages

	// delete sysIndex

	return 1;
}

int DB::useDB(string dbName)
{
	// For use database command
	// Read the database file..... (first 100 pages maybe)
	// Read the database header....
	// Read all the SysTables files.....
	// Read all the SysColumns files.....
	// Read all the SysIndex files......
}

int DB::dropDB(string dbName)
{
	// For drop database command
	// Delete the database file.....
}

int DB::createTable(/*Query Parameter Structure*/)
{
	// For create table command
	// Check if table name already exists, if so return TABLENAMEUNIQUEERROR
	// Create a SysTable Entry for the table
	// Create SysColumns entry for each of the columns of the table, if columnCount is not 0
	// Create a Directory and Data Page for the table, updating the corresponding pages
}

int DB::dropTable()
{
	// For drop table command
	// Delete all the data pages
	// Delete all the directory pages
	// Delete index if it exists
	// Delete all the entries related to SysColumns
	// Delete the SysTable entry for the table
}

int DB::createIndex()
{
	// For create index command
	// Check if index name already exists... If so return error....
	// Check if for the table, index already exists.... If so return error.....
	// Add sysIndex entry
	// If index is created before any insertions..... Then nothing to do from here on return....
	// Else
	// Call B+ tree code with data pages' data and its corresponding entry in the directory entry
	// Pass Data and RID(PageNo,SlotID pair)
}

int DB::dropIndex()
{
	// For drop index command
	// Recover all the pages related to the index
	// Delete the sysIndex entry
}

int DB::insertEntry()
{
	// For insert command
	// Index check required
	// Check whether entry has to be inserted and then the index updated or index will provide me the location to insert
	// If SysTables Entry exists, and SysColumns Entries are 0 for the table, then return error COLUMNSNOTSPECIFIEDERROR
}

int DB::deleteEntry()
{
	// For delete command
	// Index check required
}

int DB::updateEntry()
{
	// For update command
	// Index check required
	// If varchar field is getting updated - delete and insert.....
	// Call DataPage.getData giving the slot pointer and size, Compare the data here.
	// If it matches, create new data buffer, from the old data buffer but with modified content at position
	// Mark the slot as deleted and insert the data
	// Else, the field is fixed length, so, just call the modify data page data directly...
}

int DB::alterTable()
{
	// For alter table command
	// Index check required
}

int DB::selectEntry()
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
}

int DB::showTables()
{
	// For show tables command
}

