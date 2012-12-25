// Test memcpy of strings.....

#include <iostream>
#include <string.h>

using namespace std;

int main()
{
	string data = "Krishna Prasad";
	char *databuf = new char [data.length()+1];
	const char *s = data.c_str();
	char *s1 = new char [data.length()+1];

	memcpy(databuf,s,data.length()+1);
	cout<<databuf<<endl;

	string data1;
	memcpy(s1,databuf,data.length()+1);
	data1 = string(s1);
	cout<<data1<<endl;

	cout<<"Data: "<<data<<endl;
	cout<<"Data1: "<<data1<<endl;

	if(data == data1)
		cout<<"Strings are equal."<<endl;
	else
		cout<<"Strings are not equal."<<endl;

	cout<<"Length of String: "<<data.length()<<endl;
	cout<<"Length of String1: "<<data1.length()<<endl;
}
