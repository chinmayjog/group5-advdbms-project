// Testing passing of stream operators....

#include <iostream>
#include <fstream>

using namespace std;

void fRead(ifstream &fd)
{
	int c = 0;
	string line;
	while(!fd.eof())
	{
		getline (fd, line);
		cout<<line<<endl;
		c++;
	}
}

int main()
{
	ifstream fd;
	string fName = "testStreamPass.cpp";

	fd.open(fName.c_str());
	fRead(fd);
	fd.close();
}
