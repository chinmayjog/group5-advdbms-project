#ifndef _MAIN_HEADER_
#define _MAIN_HEADER_
enum conjunction {EQL, NEQL, LET, LEQ, GRT, GEQ, ANDC, ORC, LIKEC, INC};
enum optype {INTEGER, FLOAT, STRING, DT, TM, DTTM, STRUCTPTR, ARRAY};

typedef struct dateStruct{
	char date[8];
	char time[6];
	int tz;
}dateStruct;

dateStruct* getDate(char []);

typedef struct columnStruct{
	char name[64];
	char type[9];
	char function[10];
	int sizeofField1;
	int sizeofField2; //only for decfloat
	dateStruct defaultdate;
	char defaultstring[257];
	short isPrimary;
	short isAutoIncrement;
	short isDefault;
	short isNullable;
}columnStruct;

typedef struct insertionStruct{
	char colname[64];
	char type[9];
	char str[64];
	dateStruct *d;
}insertStruct;

typedef struct conditionStruct{
	struct conditionStruct *lhs, *rhs;
	conjunction cond;
	char colname[64];
	char rightstr[50][100];
	dateStruct *rightdttm[50];
	optype lt, rt;
	int rightcnt;
	bool flag;
	
	
}condition;

condition* newConditionStruct();
columnStruct* newColumnStruct();
condition* pop();
void push(condition*);
void traverse(condition *node);

typedef struct {
	int error;
    char type[20];
	char dbname[64];
	char index[64];
    char table[64];
    columnStruct columns[20];
    int cntColumns;
    condition *root;
    insertStruct ins[20];
    int lowerLimit;
    int countResults;
    int dirPageNo;
    int slotID;
    int dataPageNo;
    int dataSlotID;
    int totalResultRows;
    char **results;
}querystruct;

typedef querystruct* query;



query load_query(const char *str);
void yyerror();
#endif

