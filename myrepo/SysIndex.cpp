// SysIndex.cpp for SysIndex.h ....

#include "SysIndex.h"

string SysIndexEntry::getIndexName()
{
	return _indexName;	
}

void SysIndexEntry::setIndexName(string indexName)
{
	_indexName = indexName;
}

string SysIndexEntry::getDBName()
{
	return _dbName;
}

void SysIndexEntry::setDBName(string dbName)
{
	_dbName = dbName;
}

string SysIndexEntry::getTableName()
{
	return _tableName;
}

void SysIndexEntry::setTableName(string tableName)
{
	_tableName = tableName;
}

char SysIndexEntry::getUniqueRule()
{
	return _uniqueRule;
}

void SysIndexEntry::setUniqueRule(char uniqueRule)
{
	_uniqueRule = uniqueRule;
}

short SysIndexEntry::getColCount()
{
	return _colCount;
}

void SysIndexEntry::setColCount(short colCount)
{
	_colCount = colCount;
}

short SysIndexEntry::getFanOutNo()
{
	return _fanOutNo;
}

void SysIndexEntry::setFanOutNo(short fanOutNo)
{
	_fanOutNo = fanOutNo;
}

int SysIndexEntry::getIndexPageID()
{
	return _indexPageID;
}

void SysIndexEntry::setIndexPageID(int indexPageID)
{
	_indexPageID = indexPageID;
}

int SysIndexEntry::getPageSize()
{
	return _pageSize;
}

void SysIndexEntry::setPageSize(int pageSize)
{
	_pageSize = pageSize;
}

char SysIndexEntry::getEraseRule()
{
	return _eraseRule;
}

void SysIndexEntry::setEraseRule(char eraseRule)
{
	_eraseRule = eraseRule;
}

char SysIndexEntry::getCloseRule()
{
	return _closeRule;
}

void SysIndexEntry::setCloseRule(char closeRule)
{
	_closeRule = closeRule;
}

string SysIndexEntry::getRemarks()
{
	return _remarks;
}

void SysIndexEntry::setRemarks(string remarks)
{
	_remarks = remarks;
}

int SysIndexEntry::getAvgKeyLength()
{
	return _avgKeyLength;
}

void SysIndexEntry::setAvgKeyLength(int avgKeyLength)
{
	_avgKeyLength = avgKeyLength;
}

int SysIndexEntry::fillBuffer(char *buffer)
{
	char * tabNameBuff = new char [64];
	char * dbNameBuff = new char [64];
	char * indexNameBuff = new char [64];
	char * remarksBuff = new char [256];

	int i;

	for(i=0;i<64;i++)
	{
		tabNameBuff[i]='$';
		dbNameBuff[i]='$';
		indexNameBuff[i]='$';
	}
	for(i=0;i<256;i++)
		remarksBuff[i]='$';

	int len = _indexName.length();
	for(i=0;i<len;i++)
		indexNameBuff[i]=_indexName[i];
	len = _dbName.length();
	for(i=0;i<len;i++)
		dbNameBuff[i]=_dbName[i];
	len = _tableName.length();
	for(i=0;i<len;i++)
		tabNameBuff[i]=_tableName[i];
	len = _remarks.length();
	for(i=0;i<len;i++)
		remarksBuff[i]=_remarks[i];

	memcpy(&buffer[SYSINDINDEXNAMEPTR],indexNameBuff,64);
	memcpy(&buffer[SYSINDDBNAMEPTR],dbNameBuff,64);
	memcpy(&buffer[SYSINDTABLENAMEPTR],tabNameBuff,64);
	memcpy(&buffer[SYSINDUNIQRULEPTR],&_uniqueRule,sizeof(char));
	memcpy(&buffer[SYSINDCOLCOUNTPTR],&_colCount,sizeof(short));
	memcpy(&buffer[SYSINDFANOUTNOPTR],&_fanOutNo,sizeof(short));
	memcpy(&buffer[SYSINDINDEXPAGEIDPTR],&_indexPageID,sizeof(int));
	memcpy(&buffer[SYSINDPAGESIZEPTR],&_pageSize,sizeof(int));
	memcpy(&buffer[SYSINDERASERULEPTR],&_eraseRule,sizeof(char));
	memcpy(&buffer[SYSINDCLOSERULEPTR],&_closeRule,sizeof(char));
	memcpy(&buffer[SYSINDREMARKSPTR],remarksBuff,256);
	memcpy(&buffer[SYSINDAVGKEYLENGTHPTR],&_avgKeyLength,sizeof(int));

	delete tabNameBuff;
	delete dbNameBuff;
	delete indexNameBuff;
	delete remarksBuff;
}

int SysIndexEntry::getDataBuffer(char *buffer)
{
	char * tabNameBuff = new char [64];
	char * dbNameBuff = new char [64];
	char * indexNameBuff = new char [64];
	char * remarksBuff = new char [256];

	memcpy(indexNameBuff,&buffer[SYSINDINDEXNAMEPTR],64);
	memcpy(dbNameBuff,&buffer[SYSINDDBNAMEPTR],64);
	memcpy(tabNameBuff,&buffer[SYSINDTABLENAMEPTR],64);
	memcpy(&_uniqueRule,&buffer[SYSINDUNIQRULEPTR],sizeof(char));
	memcpy(&_colCount,&buffer[SYSINDCOLCOUNTPTR],sizeof(short));
	memcpy(&_fanOutNo,&buffer[SYSINDFANOUTNOPTR],sizeof(short));
	memcpy(&_indexPageID,&buffer[SYSINDINDEXPAGEIDPTR],sizeof(int));
	memcpy(&_pageSize,&buffer[SYSINDPAGESIZEPTR],sizeof(int));
	memcpy(&_eraseRule,&buffer[SYSINDERASERULEPTR],sizeof(char));
	memcpy(&_closeRule,&buffer[SYSINDCLOSERULEPTR],sizeof(char));
	memcpy(remarksBuff,&buffer[SYSINDREMARKSPTR],256);
	memcpy(&_avgKeyLength,&buffer[SYSINDAVGKEYLENGTHPTR],sizeof(int));

	_indexName = "";
	_dbName = "";
	_tableName = "";
	_remarks = "";

	int i;
	for(i=0;i<64;i++)
	{
		if(indexNameBuff[i]=='$')
			break;
		_indexName=_indexName+indexNameBuff[i];
	}
	for(i=0;i<64;i++)
	{
		if(dbNameBuff[i]=='$')
			break;
		_dbName=_dbName+dbNameBuff[i];
	}
	for(i=0;i<64;i++)
	{
		if(tabNameBuff[i]=='$')
			break;
		_tableName=_tableName+tabNameBuff[i];
	}
	for(i=0;i<256;i++)
	{
		if(remarksBuff[i]=='$')
			break;
		_remarks=_remarks+remarksBuff[i];
	}

	delete tabNameBuff;
	delete dbNameBuff;
	delete indexNameBuff;
	delete remarksBuff;
}

int SysIndex::getPageID()
{
	return _pageID;
}

void SysIndex::setPageID(int pageID)
{
	_pageID = pageID;
}

short SysIndex::getPagePriority()
{
	return _pagePriority;
}

void SysIndex::setPagePriority(short pagePriority)
{
	_pagePriority = pagePriority;
}

int SysIndex::getNextSysIndexPage()
{
	return _nextSysIndexPage;
}

void SysIndex::setNextSysIndexPage(int nextSysIndexPage)
{
	_nextSysIndexPage = nextSysIndexPage;
}

int SysIndex::getCurSysIndexSize()
{
	return _curSysIndexSize;
}

void SysIndex::setCurSysIndexSize(int curSysIndexSize)
{
	_curSysIndexSize = curSysIndexSize;
}

int SysIndex::getRemSysIndexSize()
{
	return _remSysIndexSize;
}

void SysIndex::setRemSysIndexSize(int remSysIndexSize)
{
	_remSysIndexSize = remSysIndexSize;
}

long SysIndex::getSysIndexEntryPointer()
{
	return _sysIndexEntryPointer;
}

void SysIndex::setSysIndexEntryPointer(long sysIndexEntryPointer)
{
	_sysIndexEntryPointer = sysIndexEntryPointer;
}

int SysIndex::getNoOfEntries()
{
	return _noOfEntries;
}

void SysIndex::setNoOfEntries(int noOfEntries)
{
	_noOfEntries = noOfEntries;
}

int SysIndex::getPageSize()
{
	return _pageSize;
}

void SysIndex::setPageSize(int pageSize)
{
	_pageSize = pageSize;
}

void SysIndex::writeSysIndexBuffer(char *buffer)
{
	memcpy(&buffer[PAGEIDPTR],&_pageID,sizeof(int));
	memcpy(&buffer[PAGEPRIPTR],&_pagePriority,sizeof(short));
	memcpy(&buffer[NEXTPTR],&_nextSysIndexPage,sizeof(int));
	memcpy(&buffer[CURSYSINDSIZEPTR],&_curSysIndexSize,sizeof(int));
	memcpy(&buffer[REMSYSINDSIZEPTR],&_remSysIndexSize,sizeof(int));
	memcpy(&buffer[SYSINDEXENTRYPTR],&_sysIndexEntryPointer,sizeof(long));
	memcpy(&buffer[SYSINDNOEPTR],&_noOfEntries,sizeof(int));
}

bool SysIndex::checkEntryInsertion()
{
	if(SYSINDEXENTRYSIZE + SYSINDSLOTSIZE > _remSysIndexSize)
		return 0;
	else
		return 1;
}

int SysIndex::createNewSysIndexEntry(char *entryBuff,char *sysIndexBuff)
{
	if(checkEntryInsertion()==0)
		return -1; // SysIndex Entry cannot be inserted.... Try inserting in a new SysIndex page
	else
	{
		// Memcpy to the next SysIndexEntry location
		memcpy(&sysIndexBuff[_sysIndexEntryPointer],entryBuff,SYSINDEXENTRYSIZE);

		// Updating the current size and remaining size
		_curSysIndexSize = _curSysIndexSize + SYSINDEXENTRYSIZE + SYSINDSLOTSIZE;
		_remSysIndexSize = _remSysIndexSize - SYSINDEXENTRYSIZE - SYSINDSLOTSIZE;

		_noOfEntries = _noOfEntries + 1;
		_sysIndexEntryPointer = _sysIndexEntryPointer + SYSINDEXENTRYSIZE;

		// Memcpy the updated values
		memcpy(&sysIndexBuff[CURSYSINDSIZEPTR],&_curSysIndexSize,sizeof(int));
		memcpy(&sysIndexBuff[REMSYSINDSIZEPTR],&_remSysIndexSize,sizeof(int));
		memcpy(&sysIndexBuff[SYSINDEXENTRYPTR],&_sysIndexEntryPointer,sizeof(long));
		memcpy(&sysIndexBuff[SYSINDNOEPTR],&_noOfEntries,sizeof(int));

		// Mark the inserted slot as 1
		char inserted = '1';
		if(_noOfEntries - 1 == 0)
			memcpy(&sysIndexBuff[FIRSTSYSINDSLOTPTR],&inserted,sizeof(char));
		else
			memcpy(&sysIndexBuff[FIRSTSYSINDSLOTPTR-((_noOfEntries-1)*sizeof(char))],&inserted,sizeof(char));
		return 1; // SysIndex Entry inserted....
	}
}

int SysIndex::deleteSysIndexEntry(string indexName,char *sysIndexBuff)
{
	bool found = 0;
	int entryID,i;
	char alreadyDeleted;

	for(i = 0;i<_noOfEntries;i++)
	{
		memcpy(&alreadyDeleted,&sysIndexBuff[FIRSTSYSINDSLOTPTR-(i*SYSINDSLOTSIZE)],SYSINDSLOTSIZE);

		if(alreadyDeleted == '0')
		{
			cout<<"The entry is deleted... Don't search there.....";
			continue;
		}

		char * newEntryBuff = new char [SYSINDEXENTRYSIZE];

		memcpy(newEntryBuff,&sysIndexBuff[0+(i*SYSINDEXENTRYSIZE)],SYSINDEXENTRYSIZE);

		char * idxName = new char [64];

		memcpy(idxName,&newEntryBuff[SYSINDINDEXNAMEPTR],64);

		string entryIndexName;
		for(int j=0;j<64;j++)
		{
			if(idxName[j] == '$')
				break;
			entryIndexName = entryIndexName+idxName[j];
		}

		if(indexName == entryIndexName)
		{
			found = 1;
			entryID = i;
			delete idxName;
			delete newEntryBuff;
			break;
		}
		delete idxName;
		delete newEntryBuff;
	}

	if(found == 0)
	{
		cout<<"Index not found... Continue searching..."<<endl;
		return -1;// Entry not found
	}

	alreadyDeleted = '0';
	memcpy(&sysIndexBuff[FIRSTSYSINDSLOTPTR-(i*SYSINDSLOTSIZE)],&alreadyDeleted,SYSINDSLOTSIZE);
	return i+1; // SysIndexEntry found Deleted at (i+1)th Slot
}

int SysIndex::searchSysIndexEntry(string indexName,char *sysIndexBuff)
{
	bool found = 0;
	int entryID,i;
	char alreadyDeleted;

	for(i = 0;i<_noOfEntries;i++)
	{
		memcpy(&alreadyDeleted,&sysIndexBuff[FIRSTSYSINDSLOTPTR-(i*SYSINDSLOTSIZE)],SYSINDSLOTSIZE);

		if(alreadyDeleted == '0')
		{
			cout<<"The entry is deleted... Don't search there.....";
			continue;
		}

		char * newEntryBuff = new char [SYSINDEXENTRYSIZE];

		memcpy(newEntryBuff,&sysIndexBuff[0+(i*SYSINDEXENTRYSIZE)],SYSINDEXENTRYSIZE);

		char * idxName = new char [64];

		memcpy(idxName,&newEntryBuff[SYSINDINDEXNAMEPTR],64);

		string entryIndexName;
		for(int j=0;j<64;j++)
		{
			if(idxName[j] == '$')
				break;
			entryIndexName = entryIndexName+idxName[j];
		}

		if(indexName == entryIndexName)
		{
			found = 1;
			entryID = i;
			delete idxName;
			delete newEntryBuff;
			break;
		}
		delete idxName;
		delete newEntryBuff;
	}

	if(found == 0)
	{
		cout<<"Index not found... Continue searching..."<<endl;
		return -1;// Entry not found
	}

	return i+1; // SysIndexEntry found at (i+1)th Slot
}
