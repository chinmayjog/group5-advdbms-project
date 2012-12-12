#if ! defined _BUFFERMANAGER_H
#define _BUFFERMANAGER_H 1
#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#define _NUMPG 1024
#define _PGSZ 2048

using namespace std;

enum PagePriority {NOP, DATAPAGE, DIRECTORYPAGE, DATABASEHEADER};

typedef struct buf{
        bool dirty;
        PagePriority p_priority;
        int pageNo;
        int numHits;
        int bufferId;
        int mdtID;
        time_t t_priority;
        char data[1];
}MemBuffer;

typedef struct metadatatable{
	string dbName;
	int id; //id of metadatatable to which all others refer to get fd
	fstream fd;
	bool isopen;
}MDT;

class BufferManager {
         
    private:
		static const int max_connection = 3;
		MDT mdt[max_connection];
		unsigned char *bufferPool;
		int sizeofheader;
		int sizeofbuffer;
		int pageSize;
		int numPages;
		int hitCnt;
		int totalCnt;
		static BufferManager *buffer;
		BufferManager();
		~BufferManager();
		int  isFileExists(string filename);
		int  getEmptyMDT();
		bool isBufferPool();
		int  readPageInBuffer(int fdID, int pgNo, PagePriority p);
		int  findPageInCache(int fdID, int pgNo);
		int  findEmptyCachePage();
		int  getReplacablePage(PagePriority p);
		bool writeBufferPagetoDisk(int bufferId);
		int  writePageToBuffer(int fdID, int pgNo, PagePriority p, unsigned char *src);
		
		
	public:
		 static BufferManager* getBufferManager();
		 static void releaseBuffer();
		 void setPageSize(int pageSizeInBytes);
		 int  initializeCache(int numPages);
		 int  getPageSize();
		 int  createDB(string filename, int sizeinBytes);
		 int  openDB(string filename);
		 bool readDB(int fdID, int pgNo, PagePriority p, unsigned char* dest);
		 bool writeDB(int fdID, int pgNo, PagePriority p, unsigned char* src);
		 int  expandDB(int mdtID, int size);
		 int  closeDB(int fdID);
		 void closeAll();
		 int  getPageCount(int mdtID);
		 bool commitFile(int mdtID);
		 bool commitCache();
		 void printHex(int buffId, int size);
		 void printCacheHeaders();
		 float getHitRate();
};

#endif
