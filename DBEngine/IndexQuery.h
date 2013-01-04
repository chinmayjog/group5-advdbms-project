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
	bool insertFlag;
	char *key;
	RecordID keyRecord;
	int limitKeys;
	int returnedKeys;
	RecordID keyRecord[limitKeys];
	char *oldKey;
	int oldKeyLength;
	char *newKey;
	int newKeyLength;
	RecordID oldKeyRecord;
	RecordID newKeyRecord;
	bool errorFlag;
	int errorNum;
}IndexQuery;
