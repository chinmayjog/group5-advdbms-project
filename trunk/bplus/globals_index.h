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

#define page_size 2048
#define PAGEID_PTR (page_size-sizeof(int))
#define PAGEPRIORITY_PTR (PAGEID_PTR-sizeof(short))
#define NEXT_PTR (PAGEPRIORITY_PTR-sizeof(int))
#define LEVEL (NEXT_PTR-sizeof(int))
#define NUM_NODES (LEVEL-sizeof(int))
#define PAGE_NO (NUM_NODES-sizeof(int))
#define PARENT_NODE (PAGE_NO-sizeof(int))
#define PAGE_PRIORITY (PARENT_NODE-sizeof(int))
#define NEXT (PAGE_PRIORITY-sizeof(int))
#define PREV (NEXT-sizeof(int))
#define PAGEMETADATA (sizeof(int) + sizeof(short) + sizeof(int))
