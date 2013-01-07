// For Drop Index
// The Index pages has to be returned to the free list
// Send the root page id
// Noopur has to return all the pages from the root page to the free page list

#include"DB.h"

#define INDEXCREATE 1
#define INDEXINSERT 2
#define INDEXINSERTCHECK 3
#define INDEXGET 4
#define INDEXUPDATEKEY 5
#define INDEXUPDATERECORD 6
#define INDEXDELETEKEY 7
#define INDEXDROP 8

typedef struct recordID
{
	int dataPageID;
	short slotID;
}RecordID;

typedef struct indexQuery
{
	short queryType;
	int rootPageID;
	char dataType[8];
	int charLength;

	DB * curDB;

	bool resultFlag;

	char *key;
	RecordID keyRecord;

	int limitKeys;
	int returnedKeys;
	int lastKeyID;
	int totalResults;
	RecordID **keyRecords;
	
	bool rootPageIDUpdated;
	int newRootPageID;

	char *oldKey;
	int oldKeyLength;
	char *newKey;
	int newKeyLength;
	RecordID oldKeyRecord;
	RecordID newKeyRecord;

	bool errorFlag;
	int errorNum;
}IndexQuery;
