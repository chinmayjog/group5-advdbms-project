// Check strstr

#include<stdio.h>
#include<string.h>

int main()
{
	char ch1[] = "Testing strstr";
	char ch2[] = "str";
	char *f1,*f2;

	f1 = strstr(ch1,ch2);
	f2 = strstr(ch1,"Krishna");

	printf("\n Find result f1: %s",f1);
	if(f2 == 0)
	{
		printf("\n Inside if");
		printf("\n Find result f2: %s",f2);
	}
	else
	{
		printf("\n Inside else.");
		printf("\n Find result f2: %s",f2);
		printf("\n Find result in digit: %d",f2);
	}
	printf("\n");
	return 0;
}
