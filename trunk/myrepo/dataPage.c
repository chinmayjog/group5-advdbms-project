// Code to create a data page and multiple entries

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<malloc.h>
#include<fcntl.h>
#include<ctype.h>

#define PAGESIZE 1024
#define FIRSTSLOTPTR PAGESIZE-sizeof(short)-sizeof(int)-sizeof(long)
#define LASTSLOTPTR FIRSTSLOTPTR-(pageDetails->_slotCounter-1)*sizeof(long)-(pageDetails->_slotCounter-1)*sizeof(int)
#define SLOTSIZE sizeof(long)+sizeof(int)
#define PAGESLOTCOUNTER PAGESIZE-sizeof(short)
#define PAGECFSSIZEPOINTER PAGESIZE-sizeof(short)-sizeof(int)
#define PAGECFSPOINTER PAGESIZE-sizeof(short)-sizeof(int)-sizeof(long)


struct dataPage
{
	int _cfs;
	long _cfsPointer;
	short _slotCounter;
};

struct slots
{
	long _slotPointer;
	int _slotSize; // 2^16 is a very large no. hence, limiting the size to short rather than int or long. Changed it to int
		// because, it involves negative nos as well
};

struct slotsDataPage
{
	short deletedSlots;
	struct slots *slotTable;
};

void flush_stdin( void )
{
	int ch;

	do
		ch = getchar();
	while ( ch != EOF && ch != '\n' );
}

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

ssize_t writePage(int fd,char *buffer,int pageSize)
{
	return write(fd,buffer,PAGESIZE);	
}

struct dataPage * castStructBuffer(char *buffer)
{
	short slotCounter;
	int cfs;
	long cfsPointer;
	struct dataPage *pageDetails = (struct dataPage *)malloc(sizeof(struct dataPage));

	memcpy(&slotCounter,&buffer[PAGESIZE-sizeof(short)],sizeof(short));
	memcpy(&cfs,&buffer[PAGESIZE-sizeof(short)-sizeof(int)],sizeof(int));
	memcpy(&cfsPointer,&buffer[PAGESIZE-sizeof(short)-sizeof(int)-sizeof(long)],sizeof(long));

	// Check using buffer[ADDRESS] directly........
	pageDetails->_cfs = cfs;
	pageDetails->_cfsPointer = cfsPointer;
	pageDetails->_slotCounter = slotCounter;

	return pageDetails;
}

void createEmptyPage(int fd)
{
	char *buffer = (char *)malloc(sizeof(char)*PAGESIZE);
	short slotCounter = 0;
	int cfs = PAGESIZE - sizeof(short) - sizeof(int) - sizeof(long) - SLOTSIZE;
	long cfsPointer = 0;
	off_t size = lseek(fd,0,SEEK_END);

	memcpy(&buffer[PAGESIZE - sizeof(short)],&slotCounter,sizeof(short));
	memcpy(&buffer[PAGESIZE - sizeof(short) - sizeof(int)],&cfs,sizeof(int));
	memcpy(&buffer[PAGESIZE - sizeof(short) - sizeof(int) - sizeof(long)],&cfsPointer,sizeof(long));

		
	if(writePage(fd,buffer,PAGESIZE))
		printf("\n Page has been written.");
	else
		printf("\n Page write failed!!!");
}

char * readPage(int fd,int pagenum)
{
	char *buffer = (char *)malloc(sizeof(char)*PAGESIZE);
	off_t offset = lseek(fd,(pagenum-1)*PAGESIZE,SEEK_SET);
	ssize_t size = read(fd,buffer,PAGESIZE);

	if(size > 0)
		return buffer;
	else
		return "\0";
}

void writePageData(int fd, char *data,int pagenum)
{
	char *buffer = readPage(fd,pagenum);
	int i;
	struct dataPage *pageDetails = castStructBuffer(buffer);
	long worstSlotPointer;
	int worstfitSize = 0,worstSize;
	int slotSize;
	long slotPointer;
	short deletedFlag = 0;
	short position;
	int dataSize = strlen(data);
	long newSlotPointer;
	int newCFSSize;
	short newSlotCounter;
	off_t size;

	
	if((dataSize+SLOTSIZE) > pageDetails->_cfs)
	{
		// Implement new page creation functionality......
		printf("\n This data page is full.....");
		printf("\n Writing to a new page......");
		printf("\n Functionality not implemented.....");
		return;
	}

	if(pageDetails->_slotCounter == 0)
	{
		pageDetails->_slotCounter+=1;
		memcpy(&buffer[pageDetails->_cfsPointer],data,strlen(data));
		pageDetails->_cfs -= (dataSize+SLOTSIZE);
		slotPointer = pageDetails->_cfsPointer;
		pageDetails->_cfsPointer += dataSize;
		memcpy(&buffer[FIRSTSLOTPTR-sizeof(long)],&slotPointer,sizeof(long));
		memcpy(&buffer[FIRSTSLOTPTR-sizeof(long)-sizeof(int)],&dataSize,sizeof(int));
		// Update the slot pointer for updating the CFS pointer
		newSlotPointer = slotPointer + dataSize;
		newCFSSize = pageDetails->_cfs;
		newSlotCounter = pageDetails->_slotCounter;
		memcpy(&buffer[PAGESLOTCOUNTER],&newSlotCounter,sizeof(short));
		memcpy(&buffer[PAGECFSPOINTER],&newSlotPointer,sizeof(long));
		memcpy(&buffer[PAGECFSSIZEPOINTER],&newCFSSize,sizeof(int));
	}
	else
	{
		// Check for deleted slots..... Use worst fit algorithm
		for(i=0;i<pageDetails->_slotCounter;i++)
		{
			memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-i*sizeof(int)],sizeof(long));
			memcpy(&worstSize,&buffer[FIRSTSLOTPTR-(i+1)*sizeof(long)-(i+1)*sizeof(int)],sizeof(int));
			if(worstSize<0 && -1*worstSize >= dataSize)
			{
				if(worstfitSize == 0)
				{
					deletedFlag = 1;
					worstfitSize = worstSize;
					worstSlotPointer = slotPointer;
					position = i;
				}
				else if(worstfitSize < worstSize)
					continue;
				else
				{
					worstfitSize = worstSize;
					worstSlotPointer = slotPointer;
					position = i;
				}
			}
		}

		// Inserting in the deleted slot
		if(deletedFlag == 1)
		{
			memcpy(&buffer[worstSlotPointer],data,dataSize);
			memcpy(&buffer[FIRSTSLOTPTR-(position+1)*sizeof(long)-(position+1)*sizeof(int)],&dataSize,sizeof(int));
		}
		else
		{
			//Inserting in the cfs slot
			pageDetails->_slotCounter+=1;
			memcpy(&buffer[pageDetails->_cfsPointer],data,strlen(data));
			pageDetails->_cfs -= (dataSize+SLOTSIZE);
			slotPointer = pageDetails->_cfsPointer;
			pageDetails->_cfsPointer += dataSize;

			memcpy(&buffer[LASTSLOTPTR-sizeof(long)],&slotPointer,sizeof(long));
			memcpy(&buffer[LASTSLOTPTR-sizeof(long)-sizeof(int)],&dataSize,sizeof(int));


			/*memcpy(&buffer[PAGESLOTCOUNTER],&(pageDetails._slotCounter),sizeof(short));
			memcpy(&buffer[PAGECFSPOINTER],&(pageDetails._cfsPointer),sizeof(long));
			memcpy(&buffer[PAGECFSSIZEPOINTER],&(pageDetails._cfs),sizeof(int));*/
			newSlotPointer = slotPointer + dataSize;
			newCFSSize = pageDetails->_cfs;
			newSlotCounter = pageDetails->_slotCounter;
			memcpy(&buffer[PAGESLOTCOUNTER],&newSlotCounter,sizeof(short));
			memcpy(&buffer[PAGECFSPOINTER],&newSlotPointer,sizeof(long));
			memcpy(&buffer[PAGECFSSIZEPOINTER],&newCFSSize,sizeof(int));
		}
	}
	size = lseek(fd,(pagenum-1)*PAGESIZE,SEEK_SET);
	if(writePage(fd,buffer,PAGESIZE))
	{
		printf("\n Page has been written.");
		return 1;
	}
	else
	{
		printf("\n Page write failed!!!");
		return -1;
	}
}


struct dataPage * getPageDetails(int fd,int pagenum)
{
	char *buffer;
	int cfs;
	long cfspointer;
	short slotCounter;

	struct dataPage *pageDetails = (struct dataPage *) malloc(sizeof(struct dataPage));

	buffer = readPage(fd,pagenum);
	memcpy(&slotCounter,&buffer[PAGESIZE-sizeof(short)],sizeof(short));
	memcpy(&cfs,&buffer[PAGESIZE-sizeof(short)-sizeof(int)],sizeof(int));
	memcpy(&cfspointer,&buffer[PAGESIZE-sizeof(short)-sizeof(int)-sizeof(long)],sizeof(long));

	pageDetails->_cfs = cfs;
	pageDetails->_cfsPointer = cfspointer;
	pageDetails->_slotCounter = slotCounter;

	return pageDetails;
}

int deleteSlot(int fd,char *data)
{
	// Implementing it as below because as of now dataPage is continuous Pages....
	// But after DEs are implemented, then, we will have to implement using DEs........

	off_t size = lseek(fd,0,SEEK_END);
	int no_of_pages = size/PAGESIZE;
	int i,j,k;
	short slotCounter;
	long slotPointer;
	int slotSize;
	char *buffer,*databuf;
	int resCount = 0;
	
	for(i=1;i<=no_of_pages;i++)
	{
		buffer = readPage(fd,i);

		memcpy(&slotCounter,&buffer[PAGESIZE-sizeof(short)],sizeof(short));
		for(j=0;j<slotCounter;j++)
		{
			memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
			memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));

			if(slotSize<0)
			{
				// Already deleted slot no need to check again.....
				continue;
			}

			databuf = (char *) malloc(sizeof(char)*slotSize);
			memcpy(databuf,&buffer[slotPointer],slotSize);
			if(strstr(databuf,data))
			{
				printf("\n Data found at slot: %d",j+1);
				slotSize = (-1)*slotSize;
				resCount++;
				memcpy(&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],&slotSize,sizeof(int));
			}
		}
		size = lseek(fd,(i-1)*PAGESIZE,SEEK_SET);
		if(writePage(fd,buffer,PAGESIZE))
			printf("\n Page has been written.");
		else
			printf("\n Page write failed!!!");
	}
	return resCount;
}

int updateSlot(int fd,char *data,char *modifyData)
{
	// Implementing it as below because as of now dataPage is continuous Pages....
	// But after DEs are implemented, then, we will have to implement using DEs........

	off_t size = lseek(fd,0,SEEK_END);
	int no_of_pages = size/PAGESIZE;
	int i,j,k;
	short slotCounter;
	long slotPointer;
	int slotSize;
	char *buffer,*databuf;
	int resCount = 0;
	
	for(i=1;i<=no_of_pages;i++)
	{
		buffer = readPage(fd,i);

		memcpy(&slotCounter,&buffer[PAGESIZE-sizeof(short)],sizeof(short));
		for(j=0;j<slotCounter;j++)
		{
			memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
			memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));

			if(slotSize<0)
			{
				// Already deleted slot no need to check again.....
				continue;
			}

			databuf = (char *) malloc(sizeof(char)*slotSize);
			memcpy(databuf,&buffer[slotPointer],slotSize);
			if(strstr(databuf,data))
			{
				printf("\n Data found at slot: %d",j+1);
				slotSize = (-1)*slotSize;
				resCount++;
				memcpy(&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],&slotSize,sizeof(int));
			}
		}
		size = lseek(fd,(i-1)*PAGESIZE,SEEK_SET);
		if(writePage(fd,buffer,PAGESIZE))
			printf("\n Page has been written.");
		else
			printf("\n Page write failed!!!");
	}
	return resCount;
}


void printAllSlots(int fd)
{
	off_t size = lseek(fd,0,SEEK_END);
	int no_of_pages = size/PAGESIZE;
	int i,j,k;
	short slotCounter;
	long slotPointer;
	int slotSize;
	char *buffer,*databuf;
	short deleted;

	for(i=1;i<=no_of_pages;i++)
	{
		buffer = readPage(fd,i);

		memcpy(&slotCounter,&buffer[PAGESIZE-sizeof(short)],sizeof(short));
		printf("\n Slot ID\tSlot Offset\tSlot Size\tDeleted\tData");
		for(j=0;j<slotCounter;j++)
		{
			deleted = 0;
			memcpy(&slotPointer,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-j*sizeof(int)],sizeof(long));
			memcpy(&slotSize,&buffer[FIRSTSLOTPTR-(j+1)*sizeof(long)-(j+1)*sizeof(int)],sizeof(int));
			if(slotSize<0)
			{
				slotSize = (-1)*slotSize;
				deleted = 1;
			}

			databuf = (char *) malloc(sizeof(char)*slotSize);
			memcpy(databuf,&buffer[slotPointer],slotSize);

			if(deleted == 1)
				printf("\n %d\t\t%ld\t\t%d\t\tTrue\t%s",j+1,slotPointer,slotSize,databuf);
			else
				printf("\n %d\t\t%ld\t\t%d\t\tFalse\t%s",j+1,slotPointer,slotSize,databuf);
		}
	}
	printf("\n");
}

void printAllPagesDetails(int fd)
{
	off_t size = lseek(fd,0,SEEK_END);
	int no_of_pages = size/PAGESIZE;
	int i;
	struct dataPage * pageDetails; 

	printf("\n The details of all the data pages: ");
	printf("\n ");
	for(i=1;i<=no_of_pages;i++)
	{
		printf("\n Reading page no. %d .....",i);
		pageDetails = getPageDetails(fd,i);
		//printf("\n Details of page no. %d: ",i);
		printf("\n Page number: %d",/*pageno*/i);
		printf("\n Number of slots: %u",pageDetails->_slotCounter);
		printf("\n Contiguous free space: %d",pageDetails->_cfs);
		printf("\n Pointer to the contiguous free space: %ld",pageDetails->_cfsPointer);
	}
}

int main()
{
	int fd,choice,no_of_pages,i,pageno;
	char *dpbuf;
	char fname[256],data[100],modifyData[100];
	off_t size;

	while(1)
	{
		printf("\n Creation of data pages......");
		printf("\n Enter 1 to create a data page.");
		printf("\n Enter <no.> to create multiple pages. <not implemented> <From scratch/With datapage file>");
		printf("\n Enter 2 to open a data page.");
		printf("\n Enter 3 to display the properties of the data page.");
		printf("\n Enter 4 to display the page as hex.");
		printf("\n Enter 5 to insert data to a Data Page.");
		printf("\n Enter 6 to delete data from a Data Page.");
		printf("\n Enter 7 to list all the Data Pages.");
		printf("\n Enter 8 to modify data in a Data Page.");
		printf("\n Enter 9 to print all the slots information.");
		printf("\n Enter -1 to exit.");
		printf("\n Enter your choice: ");
		scanf("%d",&choice);
		flush_stdin();

		switch(choice)
		{
			case 1: printf("\n Data Page Creation.....");
				printf("\n Enter the name of the data file to be created: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				fd = creat(fname, 0666);
				//printf("\n Enter the no. of data pages to be created: ");
				//scanf("%d",&no_of_pages);
				if(fd == -1)
					printf("\n Error in creating 1GB file.");
				else
				{
					printf("\n The file is created.");
					printf("\n The file is empty.");
					printf("\n Adding a new page....");
					createEmptyPage(fd);
				}
				printf("\n");
				close(fd);
				break;
			case 2: printf("\n Opening a data page.....");
				printf("\n Enter the file to be opened: ");
				fgets(fname,sizeof(fname),stdin);
				fname[strlen(fname)-1]='\0';
				fd = open(fname, O_RDWR, 0666);
				if(fd > 0)
				{
					printf("\n Successfully opened the file %s",fname);
					printf("\n The file descriptor of the opened file is: %d",fd);
					size = lseek(fd,0,SEEK_END);
					printf("\n The file size is: %ld bytes",size);
					close(fd);
				}
				else
				{
					printf("\n Unable to open the file %s",fname);
					printf("\n The error is: %d",fd);
				}
				printf("\n");
				break;
			case 3: printf("\n Displaying the page details......");
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
					printf("\n Enter the page no. for which details is required: ");
					scanf("%d",&pageno);
					size=lseek(fd,0,SEEK_END);
					if(size == 0)
					{
						printf("\n The size of the file is 0 bytes. We cannot access pages of an empty file.");
					}
					else
					{
						struct dataPage * pageDetails;
						no_of_pages = size/PAGESIZE;
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
							printf("\n Reading page no. %d .....",pageno);
							pageDetails = getPageDetails(fd,pageno);
							printf("\n Details of the page: ");
							printf("\n Page number: %d",pageno);
							printf("\n Number of slots: %u",pageDetails->_slotCounter);
							printf("\n Contiguous free space: %d",pageDetails->_cfs);
							printf("\n Pointer to the contiguous free space: %ld",pageDetails->_cfsPointer);
							//size = lseek(fd,0,SEEK_SET);
							//size = lseek(fd,(pageno-1)*PAGESIZE,SEEK_SET);
							//size = lseek(fd,PAGESIZE-2,SEEK_CUR);
							//struct dataPage *data_page = (struct dataPage *) malloc(sizeof(struct dataPage));
							//&(data_page->_slotCounter) = 
						}
					}
				}
				break;
			case 4: printf("\n Printing the hex dump of the datapage......");
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
					printf("\n Enter the page no. for which details is required: ");
					scanf("%d",&pageno);
					size=lseek(fd,0,SEEK_END);
					if(size == 0)
					{
						printf("\n The size of the file is 0 bytes. We cannot access pages of an empty file.");
					}
					else
					{
						no_of_pages = size/PAGESIZE;
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
							char *buffer = readPage(fd,pageno);
							printf("\n Reading the page to display its hex dump......");
							printf("\n The hex dump of the page is: ");
							printhex(buffer,PAGESIZE);
						}
					}
				}
				close(fd);
				break;
			case 5: printf("\n Inserting into the data page......");
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
					printf("\n Enter the page no. for which details is required: ");
					scanf("%d",&pageno);
					// Change code to loop over dataPages.... you must not ask for data page no. and figure out automatically
					flush_stdin();
					size=lseek(fd,0,SEEK_END);
					if(size == 0)
					{
						printf("\n The size of the file is 0 bytes. We cannot access pages of an empty file.");
					}
					else
					{
						no_of_pages = size/PAGESIZE;
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
							printf("\n Enter the data to be entered: (<100 bytes) ");
							fgets(data,sizeof(data),stdin);
							data[strlen(data)-1]='\0';
							writePageData(fd,data,pageno);
						}
					}
				}
				close(fd);
				break;
			case 6: printf("\n Deleting operation......");
				printf("\n The slot containing the data or substring will be deleted....");
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
					int result;
					printf("\n Successfully opened the file %s",fname);
					printf("\n The file descriptor of the opened file is: %d",fd);
					printf("\n Enter the data/substring to be deleted: (<100 bytes) ");
					fgets(data,sizeof(data),stdin);
					data[strlen(data)-1]='\0';
					result = deleteSlot(fd,data);

					if(result == 0)
						printf("\n There are no rows containing that data.");
					else
						printf("\n The no. of slots affected: %d",result);
				}
				break;
			case 7: printf("\n Printing the data page details for all Data Pages.....");
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
					printAllPagesDetails(fd);
				}
				break;
			case 8: printf("\n Modifying the data in the data page....");
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
					int result;
					printf("\n Successfully opened the file %s",fname);
					printf("\n The file descriptor of the opened file is: %d",fd);
					printf("\n Enter the data/substring to be modified: (<100 bytes) ");
					fgets(data,sizeof(data),stdin);
					data[strlen(data)-1]='\0';
					printf("\n Enter the data/substring to be written in place of it: (<100 bytes) ");
					fgets(modifyData,sizeof(modifyData),stdin);
					modifyData[strlen(modifyData)-1]='\0';
					result = updateSlot(fd,data,modifyData);

					if(result == 0)
						printf("\n No rows were modified.");
					else
						printf("\n The no. of rows modified: %d",result);
				}
				break;
			case 9: printf("\n Printing all the slots information......");
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
					printAllSlots(fd);
				}
				break;
			default:printf("\n Unimplemented operation. Exiting.....");
				printf("\n");
				exit(0);
				break;
		}
	}
}
