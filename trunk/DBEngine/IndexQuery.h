#ifndef _INDEXQUERY_H
#define _INDEXQUERY_H
// For Drop Index
// The Index pages has to be returned to the free list
// Send the root page id
// Noopur has to return all the pages from the root page to the free page list
//enum indexOperators {EQL, NEQL, LET, LEQ, GRT, GEQ, LIKE, IN};

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
	short operatorType;

	int *rootPageID;
	char dataType[8];
	int charLength;

	int *fanOut;
	bool *fanOutChanged;
	int *newFanOut;

	//DB * curDB;

	bool *resultFlag;

	bool isPrimary; // Flag is set if the column is primary

	char *key;
	RecordID keyRecord;

	int limitKeys;
	int *returnedKeys;
	int *lastKeyID;
	RecordID *lastKeyRecordID;
	int *totalResults;
	RecordID *keyRecords;
	
	bool *rootPageIDUpdated;
	int *newRootPageID;

	char *oldKey;
	int oldKeyLength;
	char *newKey;
	int newKeyLength;
	RecordID oldKeyRecord;
	RecordID newKeyRecord;

	bool *errorFlag;
	int *errorNum;
}IndexQuery;
#endif
