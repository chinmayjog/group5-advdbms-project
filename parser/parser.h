#ifndef _MAIN_HEADER_
#define _MAIN_HEADER_
enum conjunction {EQL, NEQL, LET, LEQ, GRT, GEQ, ANDC, ORC, LIKEC, INC};
enum optype {INTEGER, FLOAT, STRING, DT, TM, DTTM, STRUCTPTR, ARRAY};

typedef struct columnStruct{
	char name[50];
	char type[20];
	char function[10];
	int sizeofField1;
	int sizeofField2; //only for decfloat
	//datetime struct
	//short shortdefault;
	// similarly other guys
}columnStruct;

typedef struct dateStruct{
	int dd;
	int mm;
	int yyyy;
	int hh;
	int mi;
	int ss;
	int tz;
}dateStruct;

dateStruct* getDate(char []);

typedef struct insertionStruct{
	char colname[50];
	char type[20];
	long i;
	double f;
	char str[40];
	dateStruct *d;
}insertStruct;

typedef struct conditionStruct{
	struct conditionStruct *lhs, *rhs;
	conjunction cond;
	char colname[100];
	char rightstr[50][100];
	int rightint[50];
	float rightflt[50];
	dateStruct *rightdttm[50];
	optype lt, rt;
	int rightcnt;
	
	
}condition;

condition* newConditionStruct();
columnStruct* newColumnStruct();
condition* pop();
void push(condition*);
void traverse(condition *node);

typedef struct {
	int error;
    char type[100];
	char dbname[100];
	char index[100];
    char table[100];
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

