// globals.h

typedef struct
{
	long _pno;
	int _tfs;
}DE_STRUCT, FAR* LP_DE_STRUCT;

typedef struct
{
	long _next;
	int _noe;
	DE_STRUCT _de[1];
}DP_STRUCT, FAR* LP_DP_STRUCT;
