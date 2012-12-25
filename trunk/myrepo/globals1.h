#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#include<math.h>

#define PAGESIZE _pageSize
#define DBROOT "/home/krishna/advdbms/group5-advdbms-project/kptestsample"
#define PAGEIDPTR PAGESIZE-sizeof(int)
#define PAGEPRIPTR PAGEIDPTR-sizeof(short)
#define NEXTPTR PAGEPRIPTR-sizeof(int)
#define CURSYSTABSIZEPTR NEXTPTR-sizeof(int)
#define REMSYSTABSIZEPTR CURSYSTABSIZEPTR-sizeof(int)
#define SYSTABLEENTRYPTR REMSYSTABSIZEPTR-sizeof(long)
#define NOEPTR SYSTABLEENTRYPTR-sizeof(int)
#define SYSTABLEMETADATASIZE (sizeof(int)+sizeof(short)+sizeof(int)+sizeof(int)+sizeof(int)+sizeof(long)+sizeof(int))
#define SYSTABLEENTRYSPACE PAGESIZE-(SYSTABLEMETADATASIZE)
#define SYSTABLEENTRYSIZE (65+65+sizeof(char)+sizeof(short)+sizeof(char)+sizeof(int)+sizeof(int)+257+sizeof(short)+sizeof(short)+sizeof(short)+sizeof(char)+sizeof(int)+sizeof(int))

#define TABLENAMEPTR 0
#define DBNAMEPTR TABLENAMEPTR+65
#define TABLETYPEPTR DBNAMEPTR+65
#define COLCOUNTPTR TABLETYPEPTR+sizeof(char)
#define ROWFORMATPTR COLCOUNTPTR+sizeof(short)
#define TABLEROWSPTR ROWFORMATPTR+sizeof(char)
#define NOPAGESPTR TABLEROWSPTR+sizeof(int)
#define REMARKSPTR NOPAGESPTR+sizeof(int)
#define KEYCOLSPTR REMARKSPTR+257
#define DATALENGTHPTR KEYCOLSPTR+sizeof(short)
#define RECLENGTHPTR DATALENGTHPTR+sizeof(short)
#define ENCODINGSCHEMEPTR RECLENGTHPTR+sizeof(short)
#define AVGROWLENGTHPTR ENCODINGSCHEMEPTR+sizeof(char)
#define DIRPAGEPTR AVGROWLENGTHPTR+sizeof(int)
