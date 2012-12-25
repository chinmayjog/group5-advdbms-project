// This is the code for the directory page

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<malloc.h>
#include<fcntl.h>
#include<ctype.h>

#define PAGESIZE 1024

struct directoryPage
{
	long _nextPointer;
	long _prevPointer;
	int _maxTFS;
	char _data[1];
};

struct recordSlot
{
	int _pageNo;
	int _tfs;
};
