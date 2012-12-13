// Directory page for data page
#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#include<math.h>

#define DEPAGESIZE _pageSize

#define PAGEID_PTR (DEPAGESIZE-sizeof(int))
#define PAGEPRIORITY_PTR (PAGEID_PTR-sizeof(short))
#define NEXT_PTR (PAGEPRIORITY_PTR-sizeof(int))
#define NOE_PTR (NEXT_PTR-sizeof(int))
#define MAXTFS_PTR (NOE_PTR-sizeof(int))
#define REMSIZE_PTR (MAXTFS_PTR-sizeof(int))
#define NEXTDE_PTR (REMSIZE_PTR-sizeof(long))

#define DEPAGEMETADATA (sizeof(int)+sizeof(short)+sizeof(int)+sizeof(int)+sizeof(int)+sizeof(int)+sizeof(long))
#define DEENTRYSPACE (DEPAGESIZE-DEPAGEMETADATA)

#define DE_PTR 0
#define DESIZE (sizeof(int)+sizeof(int))
//#define DESLOTSIZE (sizeof(long)+sizeof(bool))
#define DESLOTSIZE (sizeof(char))
#define FIRSTDESLOTPTR (NEXTDE_PTR-sizeof(char))
//#define LASTSLOTPTR FIRSTSLOTPTR-(_noe-1)*sizeof(long)-(_noe-1)*sizeof(bool)
#define LASTDESLOTPTR FIRSTSLOTPTR-(_noe-1)*sizeof(bool)

#define DPSLOTSIZE sizeof(long)+sizeof(int)

using namespace std;

/*typedef struct{
int _pageNo;
long _tfs;
}DE_STRUCT, FAR * LP_DE_STRUCT;*/

class DirectoryPageEntry
{
	private: int _pageID;
		 int _tfs;

	public:	DirectoryPageEntry()
		{
			_pageID = -1;
			_tfs = -1;
		}

		DirectoryPageEntry(int pageID,int tfs)
		{
			_pageID = pageID;
			_tfs = tfs;
		}

		DirectoryPageEntry(char *buffer)
		{
			memcpy(&_pageID,&buffer[0],sizeof(int));
			memcpy(&_tfs,&buffer[4],sizeof(int));
		}

		~DirectoryPageEntry()
		{
			// Destructor for DirectoryPageEntry()
		}

		int getPageID();
		void setPageID(int pageID);
		int getTFS();
		void setTFS(int tfs);

		void fillBuffer(char *buffer);
		bool checkTFS(int dataSize);
};

class DirectoryPage
{
	private: int _nextDirectoryPage;
		 int _noe;
		 int _maxtfs;
		 int _pageSize;
		 int _remDirectoryPageSize;
		 int _pageID;
		 short _pagePriority;
		 long _dePTR;
		 //DE_STRUCT _de[1];

	public:	DirectoryPage()
		{
			_nextDirectoryPage = -1;
			_noe = 0;
			_maxtfs = 0;
			_dePTR = 0;
			_remDirectoryPageSize = DEENTRYSPACE;
		}

		DirectoryPage(int pageSize)
		{
			_pageSize = pageSize;
			_nextDirectoryPage = -1;
			_noe = 0;
			_maxtfs = 0;
			_dePTR = 0;
			_remDirectoryPageSize = DEENTRYSPACE;
		}

		DirectoryPage(char *buffer)
		{
			memcpy(&_nextDirectoryPage,&buffer[NEXT_PTR],sizeof(int));
			memcpy(&_noe,&buffer[NOE_PTR],sizeof(int));
			memcpy(&_maxtfs,&buffer[MAXTFS_PTR],sizeof(int));
			memcpy(&_remDirectoryPageSize,&buffer[REMSIZE_PTR],sizeof(int));
			memcpy(&_dePTR,&buffer[NEXTDE_PTR],sizeof(long));
			//memcpy(&_de[1],&buffer[DE_PTR],sizeof(DE_STRUCT));
		}

		DirectoryPage(char *buffer,int pageSize)
		{
			_pageSize = pageSize;
			memcpy(&_nextDirectoryPage,&buffer[NEXT_PTR],sizeof(int));
			memcpy(&_noe,&buffer[NOE_PTR],sizeof(int));
			memcpy(&_maxtfs,&buffer[MAXTFS_PTR],sizeof(int));
			memcpy(&_remDirectoryPageSize,&buffer[REMSIZE_PTR],sizeof(int));
			memcpy(&_dePTR,&buffer[NEXTDE_PTR],sizeof(long));
			//memcpy(&_de[1],&buffer[DE_PTR],sizeof(DE_STRUCT));
		}

		~DirectoryPage()
		{
			//Destructor for DirectoryPage object
		};

		/*static DirectoryPage * getDirectoryPage()
		{
			return new DirectoryPage();
		}

		static DirectoryPage * getDirectoryPage(char *buffer)
		{
			return new DirectoryPage(buffer);
		}*/

		int getNextDirectoryPagePointer();
		void setNextDirectoryPagePointer(int _nextDirectoryPage);
		int getNoE();
		void setNoE(int _noe);
		int getPageSize();
		void setPageSize(int _pageSize);
		int getMaxTFS();
		void setMaxTFS(int _maxtfs);
		int getRemDirectoryPageSize();
		void setRemDirectoryPageSize(int _remDirectoryPageSize);
		int getpageID();
		void setpageID(int pageID);
		short getpagePriority();
		void setpagePriority(short pagePriority);
		long getdePTR();
		void setdePTR(long dePTR);

		void fillBuffer(char *buffer);
		bool checkDirectoryEntryInsertion();
		void printSlotsInformation();
		//bool checkTFS(int dataSize);

		int insertDirectoryPageEntry(char *entryBuffer,char *directoryBuffer);
		//int insertDirectoryPageEntry(char *entryBuffer,char *directoryBuffer,int pageNo);
		int deleteDirectoryPageEntry(int dataPageNo,char *directoryBuffer);
		int updateDirectoryPageEntry(int dataPageNo,int tfsValue,char *directoryBuffer);
		int searchDirectoryPageEntry(int dataPageNo,char *directoryBuffer);
};

// Add directoryPage's print slots information function
