#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cstring>
#include<math.h>
#include<vector>
#include<map>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include<fcntl.h>
#include<ctype.h>
#include<queue>

#define page_size _pageSize
/*#ifndef global1
#define PAGEID_PTR (page_size-sizeof(int))
#endif
#ifndef global2
#define PAGEPRIORITY_PTR (PAGEID_PTR-sizeof(short))
#endif
#ifndef global3
#define NEXT_PTR (PAGEPRIORITY_PTR-sizeof(int))
#endif*/
#ifndef global4
#define LEVEL (NEXTPTR-sizeof(int))
#endif
#ifndef global5
#define NUM_NODES (LEVEL-sizeof(int))
#endif
#ifndef global6
#define PAGE_NO (NUM_NODES-sizeof(int))
#endif
#ifndef global7
#define PARENT_NODE (PAGE_NO-sizeof(int))
#endif
#ifndef global8
#define PAGE_PRIORITY (PARENT_NODE-sizeof(int))
#endif
#ifndef global9
#define NEXT (PAGE_PRIORITY-sizeof(int))
#endif
#ifndef global10
#define PREV (NEXT-sizeof(int))
#endif
#ifndef global11
#define PAGEMETADATA (sizeof(int) + sizeof(short) + sizeof(int))
#endif
