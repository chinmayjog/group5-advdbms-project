// File handling.....

#define FILEPAGESIZE 8192
#include<fcntl.h>

int pageWrite(int _pno,char *buffer)
{
	ofstream fd;
	string s1 = "_dataBaseName";
	string fileName = s1+".dat";
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	fd.seekp((_pno-1)*FILEPAGESIZE,ofstream::beg);
	fd.write(buffer,FILEPAGESIZE);
	return FILEPAGESIZE;
}

int pageWrite(string fileName,int _pno,char *buffer)
{
	ofstream fd;
	//string s1 = "_dataBaseName";
	//string fileName = s1+".dat";
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	fd.seekp((_pno-1)*FILEPAGESIZE,ofstream::beg);
	fd.write(buffer,FILEPAGESIZE);
	fd.close();
	return FILEPAGESIZE;
}

int pageWrite(ofstream &fd, int _pno,char *buffer)
{
	//ofstream fd;
	/*string s1 = "_dataBaseName";
	string fileName = s1+".dat";
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}*/

	fd.seekp((_pno-1)*FILEPAGESIZE,ofstream::beg);
	fd.write(buffer,FILEPAGESIZE);
	return FILEPAGESIZE;
}

int pageWrite(ofstream &fd,int _pno,char *buffer,int _pageSize)
{
	/*ifstream fd;
	string s1 = "_dataBaseName";
	string fileName = s1+".dat";
	//char buffer = new char [_pageSize];
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}*/

	fd.seekp((_pno-1)*_pageSize,ifstream::beg);
	fd.write(buffer,_pageSize);
	fd.close();
	return _pageSize;
}

int pageWrite(string fileName,int _pno,char *buffer,int _pageSize)
{
	/*ofstream fd;
	//char buffer = new char [_pageSize];
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	fd.seekp((_pno-1)*_pageSize,ifstream::beg);
	fd.write(buffer,_pageSize);
	fd.close();
	return _pageSize;*/

	int fd;
	size_t size;
	//char buffer = new char [_pageSize];
	//char *fName = fileName.c_str();
	fd = open(fileName.c_str(), O_CREAT|O_RDWR, 0666);
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


int pageRead(int _pno,char *buffer)
{
	ifstream fd;
	string s1 = "_dataBaseName";
	string fileName = s1+".dat";
	//char buffer = new char [_pageSize];
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	fd.seekg((_pno-1)*FILEPAGESIZE,ifstream::beg);
	fd.read(buffer,FILEPAGESIZE);
	return FILEPAGESIZE;
}

int pageRead(ifstream &fd,int _pno,char *buffer,int _pageSize)
{
	/*ifstream fd;
	string s1 = "_dataBaseName";
	string fileName = s1+".dat";
	//char buffer = new char [_pageSize];
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}*/

	fd.seekg((_pno-1)*_pageSize,ifstream::beg);
	fd.read(buffer,_pageSize);
	fd.close();
	return _pageSize;
}

int pageRead(string fileName,int _pno,char *buffer,int _pageSize)
{
	ifstream fd;
	fd.open(fileName.c_str(),ios::binary);
	if(!fd)
	{
		cerr<<"\n Error in creating the file";
		return -1;
	}

	fd.seekg((_pno-1)*_pageSize,ifstream::beg);
	fd.read(buffer,_pageSize);
	fd.close();
	return _pageSize;
}

int totalFileSize(string fileName)
{
	ifstream fd;
	fd.open(fileName.c_str(),ios::binary);

	fd.seekg(0,ifstream::end);

	int fileSize = fd.tellg();

	fd.close();

	return fileSize;
}

int fileCreate(string fName)
{
	int fd = creat(fName.c_str(), 0666);
	if(fd == -1)
		printf("\n Error in creating 100MB file.");
	else
	{
		printf("\n The file is created.");
		printf("\n The file is empty.");
	}
	close(fd);
}
