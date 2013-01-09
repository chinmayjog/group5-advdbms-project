// DB.h
// Main DB header file

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#include <vector>
#include<climits>
#include<cfloat>
#include<cerrno>

#include"globalDBEngine.h"
#include"../cachemgr/src/BufferManager.h"
#include"../parser/parser.h"
#include"SysTables.h"
#include"DBHeader.h"
#include"SysColumns.h"
#include"SysIndex.h"
#include"FreeList.h"
#include"dataDirectoryPage.h"
#include"DataPage.h"
#include"DataTypes.h"

using namespace std;

static int fdID;
static bool debugFlag = false;

class DB
{
	private: int _dbHeaderPTR;
		 int _sysTablesPTR;
		 int _noSysTablePages;
		 int _sysColumnsPTR;
		 int _noSysColumnPages;
		 int _sysIndexPTR;
		 int _noSysIndexPages;
		 int _freePagePTR;
		 int _lastFreePagePTR;
		 int _noFreePages;
		 int _pageSize;
		 int _totSize;

	public: DB()
		{
			_dbHeaderPTR = 0;
			_sysTablesPTR = 1;
			_sysColumnsPTR = 2;
			_sysIndexPTR = 3;
			_freePagePTR = 4;
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

		/*static DB * getDB()
		{
			return new DB();
		}*/

		int getDBHeaderPTR();
		void setDBHeaderPTR(int dbHeaderPTR);
		int getSysTablesPTR();
		void setSysTablesPTR(int sysTablesPTR);
		int getSysColumnsPTR();
		void setSysColumnsPTR(int sysColumnsPTR);
		int getSysIndexPTR();
		void setSysIndexPTR(int sysIndexPTR);
		int getFreePagePTR();
		void setFreePagePTR(int freePagePTR);
		int getLastFreePagePTR();
		void setLastFreePagePTR(int lastFreePagePTR);
		int getNoSysTablePages();
		void setNoSysTablePages(int noSysTablePages);
		int getNoSysColumnPages();
		void setNoSysColumnPages(int noSysColumnPages);
		int getNoSysIndexPages();
		void setNoSysIndexPages(int noSysIndexPages);

		// DB Queries

		int createDB(query q);
		int useDB(query q);
		int dropDB(query q);
		int showDB(query q);
		int createTable(/*Query Parameter Structure*/query q);
		int dropTable(query q);
		int createIndex(query q);
		int dropIndex(query q);
		int insertEntry(query q);
		int deleteEntry(query q);
		int updateEntry(query q);
		int alterTable(query q);
		int selectEntry(query q);
		int showTables(query q);

		// DB Maintenance methods

		bool extendFreeSpaceCheck();
		int extendFreeSpace();

		int createNewSysTablesEntry(SysTableEntry newSysTableEntry);
		int deleteSysTablesEntry(string tableName,string dbName);
		int createNewSysColumnsEntry(SysColumnsEntry newSysColumnsEntry);
		int deleteSysColumnsEntry(string _dbName,string _tableName,string _columnName,string _dataType);
		int createNewSysIndexEntry(SysIndexEntry newSysIndexEntry);
		int deleteSysIndexEntry(string indexName,string tableName);
		int createNewDirectoryPageEntry(int directoryPageID,int newDataPageID,int tfs,bool *noOfPagesChanged);
		int insertDataBaseEntry(int directoryPageID,char *dataBuffer,bool *noOfPagesChanged);

		bool queryEvaluate(query q,string * columnNames,string * dataTypes,int * ordinalPositions,short * scales,int * columnLengths);
		bool indexUse(condition * rootTree,string tableName,int * indexPageID,int * indexID);

		int createNewSysTablesPage();
		int createNewSysColumnsPage();
		int createNewSysIndexPage();
		int createNewDirectoryPage();
		int createNewDataPage();
		int deleteSysTablesPage(int pageID);
		int deleteSysColumnsPage(int pageID);
		int deleteSysIndexPage(int pageID);
		int deleteDirectoryPage(int pageID);
		int deleteDataPage(int pageID);

		// Log entries
		bool writeLog(string message)
		{
			ofstream myfile;
			myfile.open (DEBUGFILENAME);
			myfile << "From DB: "<<message<<endl;
			myfile.close();
		}

		// Making these methods friends as Indexing also needs these function calls to get the Free page and to add the Free page
		friend int getFreePage(DB * curDB);
		friend int addFreePageList(DB * curDB,int pageID);

		// This is the function which will call the main database queries. Parser will call this method. Check pageSize before execution of the query here itself And throw error if the two are different
		friend int mainDB(query q);
};
