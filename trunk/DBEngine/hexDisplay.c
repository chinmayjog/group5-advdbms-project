// Hex dump creator.....

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<malloc.h>
#include<fcntl.h>
#include<ctype.h>

void printhex(void *buffer,int size)
{
	int i,j;
	unsigned char *buf = (unsigned char *) buffer;

	printf("\n");
	for(i=0;i<size;i+=16)
	{
		printf("%04x ",i);
		for(j=0;j<16;j++)
			printf("%02x ",*(buf+i+j));
		printf("  ");
		for(j=0;j<16;j++)
			printf("%c",isprint(*(buf+i+j))?*(buf+i+j):'.');
		printf("\n");
	}
}

char * readPage(int fd,int pagenum,int pagesize)
{
	char *buffer = (char *)malloc(sizeof(char)*pagesize);
	off_t offset = lseek(fd,(pagenum-1)*pagesize,SEEK_SET);
	ssize_t size = read(fd,buffer,pagesize);

	if(size > 0)
		return buffer;
	else
		return "\0";
}

int main()
{
	char fname[256];
	int fd,pageno,pagesize,size;
	printf("\n Enter the file to be opened: ");
	fgets(fname,sizeof(fname),stdin);
	fname[strlen(fname)-1]='\0';
	fd = open(fname, O_RDWR, 0666);
	if(fd<=0)
	{
		printf("\n Unable to open the file %s",fname);
		printf("\n The error is: %d",fd);
	}
	else
	{
		printf("\n Successfully opened the file %s",fname);
		printf("\n The file descriptor of the opened file is: %d",fd);
		printf("\n Enter the page size of the file: ");
		scanf("%d",&pagesize);
		printf("\n Enter the page no. for which details is required: ");
		scanf("%d",&pageno);
		size=lseek(fd,0,SEEK_END);
		if(size == 0)
		{
			printf("\n The size of the file is 0 bytes. We cannot access pages of an empty file.");
		}
		else
		{
			int no_of_pages = size/pagesize;
			if(pageno > no_of_pages)
			{
				printf("\n The page no you requested is not created.");
			}
			else if(pageno <= 0)
			{
				printf("\n Page no cannot be 0 or negative.");
			}
			else
			{
				char *buffer = readPage(fd,pageno,pagesize);
				printf("\n Reading the page to display its hex dump......");
				printf("\n The hex dump of the page is: ");
				printhex(buffer,pagesize);
			}
		}
	}
	close(fd);
}
