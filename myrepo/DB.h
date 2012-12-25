// DB.h
// Main DB header file

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>

#include"globalDBEngine.h"

using namespace std;

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
		int getNoSysTablePages();
		void setNoSysTablePages(int noSysTablePages);
		int getNoSysColumnPages();
		void setNoSysColumnPages(int noSysColumnPages);
		int getNoSysIndexPages();
		void setNoSysIndexPages(int noSysIndexPages);

		// DB Queries

		int createDB(int size,int pageSize,string dbName);
		int useDB(string dbName);
		int dropDB();
		int createTable(/*Query Parameter Structure*/);
		int dropTable();
		int createIndex();
		int dropIndex();
		int insertEntry();
		int deleteEntry();
		int updateEntry();
		int alterTable();
		int selectEntry();
		int showTables();

		// DB Maintenance methods

		bool extendFreeSpaceCheck();
		int extendFreeSpace();
		int getFreePage();
		int addFreePageList(int pageID);
		int createNewSysTablesEntry();
		int deleteSysTablesEntry();
		int createNewSysColumnsEntry();
		int deleteSysColumnsEntry();
		int createNewSysIndexEntry();
		int deleteSysIndexEntry();
		int createNewSysTablesPage();
		int createNewSysColumnsPage();
		int createNewSysIndexPage();
		int deleteSysTablesPage(int pageID);
		int deleteSysColumnsPage(int pageID);
		int deleteSysIndexPage(int pageID);
};
