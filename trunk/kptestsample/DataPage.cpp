// CPP file for data page.......

#include"DataPage.h"

/*
class DataPage
{
	private: int _cfs;
		 long _cfsPointer;
		 short _slotCounter;
		 //char *buffer;

	public: DataPage()
		{
			//buffer = new char[PAGESIZE];
		 	_cfs = DATAPAGEMETADATA-SLOTSIZE;
			_cfsPointer = 0;
			_slotCounter = 0;
			//memcpy(&buffer[PAGESLOTCOUNTER],&_slotCounter,sizeof(short));
			//memcpy(&buffer[PAGECFSSIZEPOINTER],&_cfs,sizeof(int));
			//memcpy(&buffer[PAGECFSPOINTER],&_cfsPointer,sizeof(long));
		}

		DataPage(char *buffer)
		{
			//this->buffer = new char[PAGESIZE];
			//memcpy(&(this->buffer),&buffer,sizeof(buffer));
			memcpy(&_slotCounter,&buffer[PAGESLOTCOUNTER],sizeof(short));
			memcpy(&_cfs,&buffer[PAGECFSSIZEPOINTER],sizeof(int));
			memcpy(&_cfsPointer,&buffer[PAGECFSPOINTER],sizeof(long));
		}

		~DataPage()
		{
			//delete[] buffer;
		}

		static DataPage * getDataPage()
		{
			return new DataPage();
		}

		static DataPage * getDataPage(char *buffer)
		{
			return new DataPage(buffer);
		}

};*/

int DataPage::getcfs()
{
	return _cfs;
}

void DataPage::setcfs(int cfs)
{
	_cfs = cfs;
}

long DataPage::getcfsPointer()
{
	return _cfsPointer;
}

void DataPage::setcfsPointer(long cfsPointer)
{
	_cfsPointer = cfsPointer;
}

short DataPage::getslotCounter()
{
	return _slotCounter;
}

void DataPage::setslotCounter(short slotCounter)
{
	_slotCounter = slotCounter;
}

void DataPage::fillBuffer(char *buffer)
{

}

bool DataPage::checkCFS(int dataSize)
{
	if(_cfs > dataSize + SLOTSIZE)
		return 1;
	else
		return 0;
}

int DataPage::insertData(char *dataPageBuffer,char *insertData)
{
	int dataSize = sizeof(dataPageBuffer);
	if(checkCFS(dataSize) == 0)
		return -1;
	else
	{
		
	}
}

int DataPage::deleteData(char *dataPageBuffer,char *deleteData)
{

}

int DataPage::searchData(char *dataPageBuffer,char *searchData)
{

}

int DataPage::updateData(char *dataPageBuffer,char *oldData,char *newData)
{

}
