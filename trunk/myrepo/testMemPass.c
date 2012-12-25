// Test buffer copy.....

#include<stdio.h>

void fill(char *s)
{
	int i;
	for(i=0;i<10;i++)
		s[i] = '0'+i;
}

// This code should work but how do we know we are not inserting in right location or not???? 
// Writing into a location in which we are not supposed to write

int main()
{
	char a[10];
	int i;

	fill(a);

	for(i=0;i<10;i++)
		printf("%c",a[i]);
	printf("\n");
}
