// C style file handling.....
#include<iostream>
#include<fcntl.h>
#include<cstring>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>

using namespace std;

int pageWrite(string fileName,int _pno,char *buffer,int _pageSize)
{
	int fd;
	size_t size;
	//char buffer = new char [_pageSize];
	//char *fName = fileName.c_str();
	fd = open(fileName.c_str(), O_RDWR, 0666);
	if(fd<0)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	//fd.seekp((_pno-1)*_pageSize,ifstream::beg);
	size = lseek(fd,(_pno-1)*_pageSize,SEEK_SET);
	write(fd,buffer,_pageSize);
	//fd.write(buffer,_pageSize);
	close(fd);
	return _pageSize;
}

int main()
{
	char a[10] = "aaaaaaaaa";
	string fileName = "a.txt";
	pageWrite(fileName,1,a,10);
	char b[10] = "bbbbbbbbb";
	pageWrite(fileName,2,b,10);
}
