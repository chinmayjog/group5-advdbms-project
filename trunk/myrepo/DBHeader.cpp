//Database header....

#include"DBHeader.h"

/*class DBHeader
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
			_filePath = NULL;
			_pageSize = DBHEADERPAGESIZE;
			_freePtr = 5;
			_dataBaseName = NULL;
			_priority = 50;
			_totalPages = _totalDBSize/_pageSize;
			_noFreePages = _totalPages - 4;
			//_curLTableID = 0;
		}

		DBHeader(char *buffer)
		{
			memcpy(&_noFreePages,&buffer[FREEPAGEPTR],sizeof(int));//memcpy operation here
			memcpy(&_sysTables,&buffer[SYSTABLEPTR],sizeof(int));//memcpy operation here
			memcpy(&_sysColumns,&buffer[SYSCOLUMNPTR],sizeof(int));//memcpy operation here
			memcpy(&_sysIndex,&buffer[SYSINDEXPTR],sizeof(int));//memcpy operation here
			//_directoryPage = NULL;//memcpy operation here
			memcpy(&_totalDBSize,&buffer[TOTDBSIZEPTR],sizeof(int));//_totalDBSize = 1024*1024*100;
			memcpy(&_filePath,&buffer[FILEPATHPTR],256);//memcpy operation here
			memcpy(&_pageSize,&buffer[PAGESIZEPTR],sizeof(int));//memcpy operation here
			memcpy(&_freePtr,&buffer[FREELISTPTR],sizeof(int));//memcpy operation here
			memcpy(&_dataBaseName,&buffer[DBNAMEPTR],64);//memcpy operation here
			memcpy(&_priority,&buffer[PAGEPRIORITYPTR],sizeof(short));//memcpy operation here
			_totalPages = _totalDBSize/_pageSize;
		}

		DBHeader(int pageSize,string dataBaseName)
		{
			_sysTables = 2;
			_sysColumns = 3;
			_sysIndex = 4;
			//_directoryPage = NULL;
			_totalDBSize = 1024*1024*100;
			_filePath = DBROOT+"/"+dataBaseName;
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
			_filePath = DBROOT+"/"+dataBaseName;
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

};
*/
int DBHeader::getNoFreePages()
{
	return _noFreePages;
}

void DBHeader::setNoFreePages(int _noFreePages)
{
	this->_noFreePages = _noFreePages;
}

int DBHeader::getSysTablesPointer()
{
	return _sysTables;
}

void DBHeader::setSysTablesPointer(int _sysTables)
{
	this->_sysTables = _sysTables;
}

int DBHeader::getSysColumnsPointer()
{
	return _sysColumns;
}

void DBHeader::setSysColumnsPointer(int _sysColumns)
{
	this->_sysColumns = _sysColumns;
}

int DBHeader::getSysIndexPointer()
{
	return _sysIndex;
}

void DBHeader::setSysIndexPointer(int _sysIndex)
{
	this->_sysIndex = _sysIndex;
}

/*DirectoryPage * DBHeader::getDirectoryPagePointer()
{
	return _directoryPage;
}

void DBHeader::setDirectoryPagePointer(DirectoryPage *_directoryPage)
{
	this->_directoryPage = _directoryPage;
}*/

int DBHeader::getTotalDBSize()
{
	return _totalDBSize;
}

void DBHeader::setTotalDBSize(int _totalDBSize)
{
	this->_totalDBSize = _totalDBSize;
}

string DBHeader::getFilePath()
{
	return _filePath;
}

void DBHeader::setFilePath(string _filePath)
{
	this->_filePath = _filePath;
}

int DBHeader::getPageSize()
{
	return _pageSize;
}

void DBHeader::setPageSize(int _pageSize)
{
	this->_pageSize = _pageSize;
}

int DBHeader::getFreePagePointer()
{
	return _freePtr;
}

void DBHeader::setFreePagePointer(int _freePtr)
{
	this->_freePtr = _freePtr;
}

string DBHeader::getDatabaseName()
{
	return _dataBaseName;
}

void DBHeader::setDatabaseName(string _dataBaseName)
{
	this->_dataBaseName = _dataBaseName;
}

int DBHeader::getTotalPages()
{
	return _totalPages;
}

void DBHeader::setTotalPages(int _totalPages)
{
	this->_totalPages = _totalPages;
}

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

void DBHeader::writeDBHeader(char *buffer)
{
	//char *buffer = new char[_pageSize];
	//buffer = ;// Chinmay get buffer
	char * filePathBuf = new char [256];
	char * dbNameBuf = new char [64];

	for(int i = 0;i<64;i++)
		dbNameBuf[i] = '$';
	for(int i = 0;i<256;i++)
		filePathBuf[i] = '$';

	int len = _dataBaseName.length();
	for(int i = 0;i<len;i++)
		dbNameBuf[i] = _dataBaseName[i];
	len = _filePath.length();
	for(int i = 0;i<len;i++)
		filePathBuf[i] = _filePath[i];

	memcpy(&buffer[FREEPAGEPTR],&_noFreePages,sizeof(int));//memcpy operation here
	memcpy(&buffer[SYSTABLEPTR],&_sysTables,sizeof(int));//memcpy operation here
	memcpy(&buffer[SYSCOLUMNPTR],&_sysColumns,sizeof(int));//memcpy operation here
	memcpy(&buffer[SYSINDEXPTR],&_sysIndex,sizeof(int));//memcpy operation here
	//_directoryPage = NULL;//memcpy operation here
	memcpy(&buffer[TOTDBSIZEPTR],&_totalDBSize,sizeof(int));//_totalDBSize = 1024*1024*100;
	memcpy(&buffer[FILEPATHPTR],filePathBuf,256);//memcpy operation here
	memcpy(&buffer[PAGESIZEPTR],&_pageSize,sizeof(int));//memcpy operation here
	memcpy(&buffer[FREELISTPTR],&_freePtr,sizeof(int));//memcpy operation here
	memcpy(&buffer[DBNAMEPTR],dbNameBuf,64);//memcpy operation here
	memcpy(&buffer[PAGEPRIORITYPTR],&_priority,sizeof(short));//memcpy operation here
	memcpy(&buffer[TOTPAGEPTR],&_totalPages,sizeof(int));

	// Call write code of Chinmay

	//delete buffer;// Freeing the buffer

	delete dbNameBuf;
	delete filePathBuf;
}

/*int DBHeader::pageWrite(int _pno,char *buffer)
{
	ofstream fd;
	string fileName = _dataBaseName+".dat";
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return;
	}

	fd.seekp((_pno-1)*_pageSize,ofstream::beg);
	fd.write(buffer,_pageSize);
}

char * DBHeader::pageRead(int _pno)
{
	ifstream fd;
	string fileName = _dataBaseName+".dat";
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return;
	}

	fd.seekg((_pno-1)*_pageSize,ifstream::beg);
	fd.read(buffer,_pageSize);
}*/
