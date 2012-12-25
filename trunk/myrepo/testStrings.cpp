// Test strings....

#include<iostream>
#include<cstring>

using namespace std;

int main()
{
	string s1 = "Krishna Prasad";

	for(int i = 0;i<s1.length();i++)
		cout<<s1[i]<<endl;

	string s2;

	for(int i=0;i<26;i++)
	{
		char *b = new char [1];
		*b = 'a'+i;
		s2 = s2+string(b);
		delete b;
	}

	s2=s2+"$";

	cout<<"String 2: "<<s2<<endl;

	cout<<"Printing until $ symbol: ";

	char *a = new char [1];
	*a = '$';

	s2[5] = *a;
	cout<<"String 2: "<<s2<<endl;

	for(int i=0;;i++)
	{
		if(s2[i]==*a)
			break;
		cout<<s2[i];
	}
	cout<<endl;
	delete a;

	cout<<"S2Length: "<<s2.length()<<endl;

	char *b2 = new char [64];
	char *b3 = new char [64];
	for(int i=0;i<64;i++)
		b2[i] = '$';
	b2[63]='\0';
	int len = s2.length();
	for(int i=0;i<len;i++)
		b2[i] = s2[i];
	b2[5] = 'f';
	cout<<"B2: "<<b2<<endl;
	cout<<"S2: "<<s2<<endl;
	memcpy(b3,b2,64);
	cout<<"B3: "<<b3<<endl;
	string s3;
	for(int i=0;b3[i]!='$';i++)
		s3=s3+b3[i];
	cout<<"S3: "<<s3<<endl;
	delete b2;
	delete b3;
	return 1;
}
