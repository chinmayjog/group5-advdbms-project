#ifndef globalA
#define PAGESIZE _pageSize
#endif
#ifndef globalB
#define DBROOT "./"
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
