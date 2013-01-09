#ifndef globalA
#define PAGESIZE _pageSize
#endif
#ifndef globalB
#define DBROOT "../testDB/"
#endif
#ifndef globalC
#define PAGEIDPTR (PAGESIZE-sizeof(int))
#endif
#ifndef globalD
#define PAGEPRIPTR (PAGEIDPTR-sizeof(short))
#endif
#ifndef globalE
#define NEXTPTR (PAGEPRIPTR-sizeof(int))
#endif
#ifndef globalF
#define SYSDATABASEPAGESPRIORITY 50
#endif
#ifndef globalG
#define DIRECTORYPAGESPRIORITY 2
#endif
#ifndef globalH
#define DATAPAGESPRIORITY 1
#endif
#ifndef globalI
#define FREEPAGESPRIORITY 0
#endif
#ifndef globalF
#define DPSLOTSIZE (sizeof(long)+sizeof(int))
#include"errornumsDBEngine.h"
#define DEBUGFILENAME "../logs/DBEngine.log"
#endif
