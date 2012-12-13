#ifndef globalA
#define PAGESIZE _pageSize
#endif
#ifndef globalB
#define DBROOT "/home/krishna/advdbms/group5-advdbms-project/kptestsample"
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
