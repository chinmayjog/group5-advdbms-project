#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<malloc.h>
#include<fcntl.h>
#include<ctype.h>

void flush_stdin( void )
{
	int ch;

	do
		ch = getchar();
	while ( ch != EOF && ch != '\n' );
}

void printhex(char *buffer,int size)
{
	int i,j;

	printf("\n");
	for(i=0;i<size;i+=16)
	{
		printf("%04x ",i);
		for(j=0;j<16;j++)
			printf("%02x ",*(buffer+i+j));
		printf("  ");
		for(j=0;j<16;j++)
			printf("%c",isprint(*(buffer+i+j))?*(buffer+i+j):'.');
		printf("\n");
	}
}

void main()
{
	int fd,choice,nob,whence,errno,pos;
	char fname[256];
	char *buffer;
	off_t size;

	while(1)
	{
		printf("\n Options to play around with files: ");
		printf("\n Enter 1 to create a file of 1GB.");
		printf("\n Enter 2 to open a file.");
		printf("\n Enter 3 to display size of the file.");
		printf("\n Enter 4 to print the page as binary.");
		printf("\n Enter 5 to print the page as hex.");
		printf("\n Enter 6 to read from file.");
		printf("\n Enter 7 to write to a file.");
		printf("\n Enter -1 to exit.");

		printf("\n Enter your choice: ");
		scanf("%d",&choice);
		flush_stdin();

		switch(choice)
		{
			case 1: printf("\n Creating 1GB file.");
				printf("\n Enter the name of the file to be created: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				fd = creat(fname, 0666);

				if(fd == -1)
					printf("\n Error in creating 1GB file.");
				else
				{
					printf("\n The file is created.");
					printf("\n The file is empty.");
				}
				printf("\n");
				close(fd);
				break;
			case 2: printf("\n Enter the name of the file to be opened: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				printf("\n The file you wanted to open is: %s",fname);
				printf("\n Trying to open the file.....");
				fd = open(fname, O_RDWR, 0666);
				if(fd > 0)
				{
					printf("\n Successfully opened the file %s",fname);
					printf("\n The file descriptor of the opened file is: %d",fd);
					close(fd);
				}
				else
				{
					printf("\n Unable to open the file %s",fname);
					printf("\n The error is: %d",fd);
				}
				printf("\n");
				break;
			case 3: printf("\n Enter the name of the file of which size is to be found: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				printf("\n The file you wanted to open is: %s",fname);
				printf("\n Trying to open the file.....");
				fd = open(fname, O_RDWR, 0666);
				if(fd > 0)
				{
					printf("\n Successfully opened the file %s",fname);
					printf("\n The file descriptor of the opened file is: %d",fd);
					size = lseek(fd,0,SEEK_END);
					printf("\n The size of the file is: %ld",size);
					close(fd);
				}
				else
				{
					printf("\n Unable to open the file %s, to display the size.",fname);
					printf("\n The error is: %d",fd);
				}
				printf("\n");
				break;
			case 4: printf("\n Enter the name of the file which has to be displayed in binary: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				printf("\n The file you wanted to open is: %s",fname);
				printf("\n Trying to open the file.....");
				fd = open(fname, O_RDWR, 0666);
				break;
			case 5: printf("\n Enter the name of the file from which which has to be displayed in hex: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				printf("\n The file you wanted to open is: %s",fname);
				printf("\n Trying to open the file.....");
				fd = open(fname, O_RDWR, 0666);
				break;
			case 6: printf("\n Enter the name of the file from which data is to be read: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				printf("\n The file you wanted to open is: %s",fname);
				printf("\n Trying to open the file.....");
				fd = open(fname, O_RDWR, 0666);
				if(fd > 0)
				{
					printf("\n Successfully opened the file %s",fname);
					printf("\n The file descriptor of the opened file is: %d",fd);
					printf("\n Enter the number of bytes to be read: ");
					scanf("%d",&nob);
					printf("\n Enter the offset of the file from where it is to be read: (0 for beginning, 1 for current location, 2 for end of file)");
					scanf("%d",&whence);
					printf("\n Enter the position from the offset where it is to be read: ");
					scanf("%d",&pos);
					size = lseek(fd,pos,whence);
					buffer = malloc(sizeof(char)*nob);
					size = read(fd,buffer,nob);
					printf("\n The read contents are: %s",buffer);
					printf("\n The size of the read contents is: %ld",size);
					close(fd);
				}
				else
				{
					printf("\n Unable to open the file %s, to read the file.",fname);
					printf("\n The error is: %d",fd);
				}
				printf("\n");
				break;
			case 7: printf("\n Enter the name of the file to which data is to be written: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				printf("\n The file you wanted to open is: %s",fname);
				printf("\n Trying to open the file.....");
				fd = open(fname, O_RDWR, 0666);
				if(fd > 0)
				{
					printf("\n Successfully opened the file %s",fname);
					printf("\n The file descriptor of the opened file is: %d",fd);
					printf("\n Enter the number of bytes to be written: ");
					scanf("%d",&nob);
					printf("\n Enter the offset of the file to where it is to be written (0 for beginning, 1 for current location, 2 for end of file): ");
					scanf("%d",&whence);
					printf("\n Enter the position from the offset where it is to be written: ");
					scanf("%d",&pos);
					flush_stdin();
					size = lseek(fd,pos,whence);
					buffer = malloc(sizeof(char)*(nob+1));
					printf("\n Enter %d bytes of data: ",nob);
					fgets(buffer,nob+1,stdin);
					//buffer[nob]='\0';
					size = write(fd,buffer,nob);
					size = lseek(fd,0,SEEK_END);
					printf("\n The written contents are: %s",buffer);
					printf("\n The size of the file after writing is: %ld",size);
					flush_stdin();
					close(fd);
				}
				else
				{
					printf("\n Unable to open the file %s, to write.",fname);
					printf("\n The error is: %d",fd);
				}
				printf("\n");
				break;
			default: printf("\n Exiting function.");
				printf("\n");
				exit(0);
				break;
		}
	}	
}
