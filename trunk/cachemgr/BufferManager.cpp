#include "BufferManager.h"

BufferManager* BufferManager::buffer = NULL; //Global Buffer Variable (Singleton)

BufferManager::BufferManager(){
	for(int i=0;i<max_connection;i++){
		mdt[i].id = -1; 
		mdt[i].isopen = false;
		mdt[i].dbName = "";
	}
	bufferPool = NULL;
	sizeofheader = sizeof(MemBuffer) - sizeof(char[1]);
	pageSize = _PGSZ;
	numPages = _NUMPG;
	sizeofbuffer = sizeofheader + pageSize;	
	hitCnt = totalCnt = 0;
}

BufferManager::~BufferManager(){
	delete(bufferPool);
	bufferPool = NULL;
};


BufferManager* BufferManager::getBufferManager(){ //Method that returns Singleton buffer instance
	if(!buffer){
		buffer = new BufferManager();
	}
	return buffer;
}

void BufferManager::releaseBuffer(){
	if(buffer){
		(*buffer).~BufferManager();
	}
}


bool BufferManager::commitFile(int mdtID){
	int id;
	MemBuffer b;
	int offset;
	if(isBufferPool()){
		for(id = 0; id < numPages; id++){
			 offset=id*sizeofbuffer;
			 memcpy(&b, bufferPool+offset, sizeofheader);
			 if(b.dirty && b.mdtID == mdtID){
				mdt[mdtID].fd.seekp(b.pageNo * pageSize, ios::beg);
				mdt[mdtID].fd.write((char *)bufferPool+offset+sizeofheader, pageSize);
				b.dirty=false;
				b.pageNo = -1;
				b.p_priority = NOP;
				b.numHits=0;
				b.mdtID = -1;
				memcpy(bufferPool+offset, &b, sizeofheader);
			}
		}
		return true;
	}
	return false;
}


int BufferManager::writePageToBuffer(int fdID, int pgNo, PagePriority p, unsigned char *src){
	int bufId = -1;
	int offset;
	MemBuffer b;
	bufId = findPageInCache(fdID, pgNo);		
	if(bufId == -1){
		bufId = readPageInBuffer(fdID, pgNo, p);
	}
	if(bufId != -1){
		offset = bufId*sizeofbuffer;
		hitCnt++;
		totalCnt++;
		memcpy((char *)bufferPool+offset+sizeofheader, src, pageSize);
		memcpy(&b, bufferPool+offset, sizeofheader); 
		b.t_priority = time(NULL);
		b.dirty = true;
		b.numHits += 1; 
		memcpy(bufferPool+offset, &b, sizeofheader);
	}
	
	return bufId;
}

bool BufferManager::writeBufferPagetoDisk(int bufferId){
	int offset;
	MemBuffer b;
	offset =bufferId*sizeofbuffer;
	memcpy(&b, bufferPool+offset, sizeofheader);
	mdt[b.mdtID].fd.seekg(0, ios::end);
	long eof = mdt[b.mdtID].fd.tellg();
	mdt[b.mdtID].fd.seekp(b.pageNo*pageSize, ios::beg);
	if(eof >= (mdt[b.mdtID].fd.tellp()+(long)pageSize)){
		mdt[b.mdtID].fd.write(reinterpret_cast<const char *>(bufferPool+offset+sizeofheader), pageSize);
		b.dirty=false;
		b.pageNo = -1;
		b.p_priority = NOP;
		b.numHits=0;
		b.mdtID = -1;
		memcpy(bufferPool+offset, &b, sizeofheader);
		return true;
	}
	else {
		//"Write out of file";
		return false;
	}
	return false;
}



int BufferManager::readPageInBuffer(int fdID, int pgNo, PagePriority p){
	int bufId, offset;
	MemBuffer b;
	bufId = findPageInCache(fdID, pgNo);
	if(bufId == -1){
		bufId = findEmptyCachePage();
		if(bufId == -1){
			bufId = getReplacablePage(p);
			if(bufId == -1)
				return bufId;
		}
		if(bufId != -1){
			mdt[fdID].fd.seekp(0, ios::end);
			long eof = mdt[fdID].fd.tellp();
			mdt[fdID].fd.seekg(pgNo*pageSize, ios::beg);
			if(eof >= (mdt[fdID].fd.tellg()+(long)pageSize)){
				offset = bufId*sizeofbuffer;
				memcpy(&b, bufferPool+offset, sizeofheader); 
				b.pageNo = pgNo;
				b.p_priority = p;
				b.t_priority = time(NULL);
				b.dirty = false;
				b.bufferId = bufId;
				b.numHits = 1; 
				b.mdtID = fdID;
				memcpy(bufferPool+offset, &b, sizeofheader);
				mdt[fdID].fd.read((char *)bufferPool+offset+sizeofheader, pageSize); 
				totalCnt++;
			}
			else {
				//"Read out of file";
				return -1;
			}
			return bufId;
		}
	}
	else{
		hitCnt++;
		totalCnt++;
		offset = (bufId)*sizeofbuffer;
		memcpy(&b, bufferPool+offset, sizeofheader);
		b.t_priority = time(NULL);
		b.numHits += 1;
		memcpy(bufferPool+offset, &b, sizeofheader);
		return bufId;
	}
	return -1;
}

bool BufferManager::commitCache(){
	for(int i=0;i<max_connection;i++){
		if(mdt[i].id != -1){
			commitFile(i);
		}
	}
	return true;
}

int BufferManager::initializeCache(int numberOfPages){
	
	if(bufferPool != NULL){
			closeAll();
			delete(bufferPool);
			bufferPool = NULL;
	}
	
	if(numberOfPages < 0)
		numPages = _NUMPG;
	else
		numPages = numberOfPages;
	
	if(numPages == 0){
		if(bufferPool != NULL){
			delete(bufferPool);
			bufferPool = NULL;
		}
		return true;
	}
	else{
		bufferPool = new unsigned char[sizeofbuffer*numPages];
		
		time_t t = time(NULL);

		/*---------------- Buffer Initialization ---------*/			
		
		for(int i=0; i<numPages; i++){
			MemBuffer b;
			b.bufferId = i;
			b.dirty = true;
			b.p_priority = NOP;
			b.pageNo = -1;
			b.numHits = 0;
			b.t_priority = t;
			b.mdtID = -1;
			int offset = i*(sizeofbuffer);
			memcpy(bufferPool+offset, &b, sizeofheader);
		}
		return true;
	}
	return false;
}

int BufferManager::createDB(string filename, int sizeinBytes){
	fstream dbFile;
	dbFile.open(("../data/"+filename+".txt").c_str(), ios::in | ios::binary);
	if(dbFile){
		dbFile.close();
		return -1;
	}
	dbFile.open(("../data/"+filename+".txt").c_str(), ios::out | ios::binary);
	if(!dbFile)
		return -2;
	dbFile.seekp(sizeinBytes-1);
	if(!dbFile.write("",1))
		return -2;
	dbFile.close();
	return 1;
}

int BufferManager::openDB(string filename){
	int mdtptr;
	if((mdtptr = isFileExists(filename)) == -1){
		if((mdtptr = getEmptyMDT())!= -1){
			mdt[mdtptr].id = mdtptr;
			mdt[mdtptr].dbName = filename;
			mdt[mdtptr].fd.open(("../data/"+filename+".txt").c_str(), ios::in | ios::out | ios::binary);
			if(!mdt[mdtptr].fd){
				return -2;
			}
			mdt[mdtptr].isopen = true;
			return mdtptr;
		}
		else
			return -1;
	}
	else
	{
		if(mdt[mdtptr].isopen){
			return mdtptr;
		}
		else
		{
			mdt[mdtptr].fd.open(("../data/"+filename+".txt").c_str(), ios::in | ios::out | ios::binary);
			if(!mdt[mdtptr].fd){
				return -2;
			}
			mdt[mdtptr].isopen = true;
			return mdtptr;
		}
	}
	return -2;
}

bool BufferManager::readDB(int fdID, int pgNo, PagePriority p, unsigned char* dest){
	int bufId, offset;
	if(mdt[fdID].dbName == "")
		return false;
	if(!mdt[fdID].fd){
		return false;
	}
	if(isBufferPool())
	{
		bufId = readPageInBuffer(fdID, pgNo, p);
		if(bufId != -1)
		{
			offset = bufId*sizeofbuffer;
			memcpy((void*)dest, (bufferPool+offset+sizeofheader), pageSize);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
			if(mdt[fdID].dbName == "")
				return false;
			if(!mdt[fdID].fd){
				return false;
			}	
			mdt[fdID].fd.seekg(pgNo*pageSize, ios::beg);
			mdt[fdID].fd.read((char *)dest, pageSize);
			if(!mdt[fdID].fd){
				return false;
			}
			return true;
	}
	return false;
}

bool BufferManager::writeDB(int fdID, int pgNo, PagePriority p, unsigned char* src){
	if(mdt[fdID].dbName == "")
		return false;
	if(!mdt[fdID].fd)
		return false;
			
	if(isBufferPool()){
		writePageToBuffer(fdID, pgNo, p, src);
		return true;
	}	
	else if(mdt[fdID].isopen){
		mdt[fdID].fd.seekp(pgNo*pageSize, ios::beg);
		mdt[fdID].fd.write((char *)src, pageSize);
		return true;
	}
	return false;
}

int BufferManager::expandDB(int mdtID, int size){
	int dbStatus = closeDB(mdtID);
	if( dbStatus == 1 || dbStatus == -2){
		mdtID=openDB(mdt[mdtID].dbName);
		if(mdt[mdtID].isopen){
			mdt[mdtID].fd.seekp(1, ios::end);
			for(int i=0; i<(size-1); i++){
				mdt[mdtID].fd.write("", 1);
			}
			closeDB(mdtID);
			mdtID = openDB(mdt[mdtID].dbName);
		}
		return mdtID;
	}
	else
		return dbStatus;
}

int BufferManager::closeDB(int mdtID){
	if(mdt[mdtID].dbName == "")
			return -5;
	if(!mdt[mdtID].fd){
			return -5;
	}
	if(mdtID < max_connection){
		if(mdt[mdtID].id != -1){
			if(mdt[mdtID].isopen){
				commitFile(mdtID);
				mdt[mdtID].fd.close();
				mdt[mdtID].isopen = false;
				return 1;
			}
			else
				return -2;
		}
	}
	return -1;
}

void BufferManager::closeAll(){
	for(int i=0;i<max_connection;i++){
		if(mdt[i].id!=-1 && mdt[i].isopen){
			closeDB(i);
		}
	}
}
