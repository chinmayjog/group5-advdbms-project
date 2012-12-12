#include "BufferManager.h"

float BufferManager::getHitRate(){
	if(isBufferPool()){
		if(totalCnt > 0){
		 return hitCnt*1.0/totalCnt;
		}
	}
	return -1;
}

int BufferManager::getPageSize(){
	return pageSize;
}


bool BufferManager::isBufferPool(){
	if(bufferPool==NULL)
		return false;
	return true;
}

int BufferManager::isFileExists(string filename){
	for(int i=0;i<max_connection;i++){
		if(filename.compare(mdt[i].dbName)==0)
			return i;
	}
	return -1;
}

int BufferManager::getEmptyMDT(){
	for(int i=0;i<max_connection;i++){
		if(mdt[i].id == -1)
			return i;
	}
	for(int i=0;i<max_connection;i++){
		if(!mdt[i].isopen)
			return i;
	}
	return -1;
}

int BufferManager::getPageCount(int mdtID){
	if(mdt[mdtID].isopen){
		mdt[mdtID].fd.seekg(0,ios::end);
		int bytes = mdt[mdtID].fd.tellg();
		return(bytes/pageSize);
	}
	return -1;
}

int BufferManager::getReplacablePage(PagePriority p){
	MemBuffer b;
	time_t now = time(NULL);
	int bufferId = -1;
	int offset;
	bool dflag = false;
	for(int i=0; i<numPages; i++){
		offset = i*sizeofbuffer;
		memcpy(&b, bufferPool+offset, sizeofheader);
		if(b.p_priority <= p && b.t_priority < now ){
			now = b.t_priority;
			bufferId = b.bufferId;
			if(b.dirty){
				dflag = true;
			}
			else {
				dflag = false;
			}
		}
	}
	if(dflag == true){
		writeBufferPagetoDisk(bufferId);
	}
	return bufferId;
} 

int BufferManager::findEmptyCachePage(){
	MemBuffer b;
	int emptyCachePage = -1;
	for(int i=0; i<numPages; i++){
		int offset = i*sizeofbuffer;
		memcpy(&b, bufferPool+offset, sizeofheader);
		if(b.pageNo == -1){
			emptyCachePage = b.bufferId;
			break;
		}
	}
	return emptyCachePage;
} 

int BufferManager::findPageInCache(int fdID, int pgNo){
	MemBuffer b;
	int bufferId = -1;
	for(int i=0; i<numPages; i++){
		int offset = i*sizeofbuffer;
		memcpy(&b, bufferPool+offset, sizeofheader);
		if(b.pageNo == pgNo && b.mdtID == fdID){
			bufferId = b.bufferId;
			break;
		}
	}
	return bufferId;
} 

void BufferManager::setPageSize(int pageSizeInBytes){
	pageSize = pageSizeInBytes;
	sizeofbuffer = sizeofheader + pageSize;
}

void BufferManager::printCacheHeaders(){
	
	int freepoolcnt=0;
	typedef struct priority{
		int id;
		time_t tp;
	}priority; 
	
	priority *pl = new priority[numPages];
	for(int i=0; i<numPages; i++){
		if(((MemBuffer *)(bufferPool+i*sizeofbuffer))->pageNo == -1){
			freepoolcnt++;
			pl[i].id = -1;
		}
		else{
			pl[i].id = ((MemBuffer *)(bufferPool+i*sizeofbuffer))->bufferId;
			pl[i].tp = ((MemBuffer *)(bufferPool+i*sizeofbuffer))->t_priority;
		}
	}
	
	for(int i=0; i<numPages; i++){
		for(int j=0; j<numPages; j++){
			if(pl[i].tp < pl[j].tp){
				int id = pl[i].id;
				time_t t = pl[i].tp;
				pl[i].id = pl[j].id;
				pl[i].tp = pl[j].tp;
				pl[j].id = id;
				pl[j].tp = t;
				 
			}
		}
	}
	cout<<endl<<"Number of free cache pages: "<<freepoolcnt<<endl;
	for(int i=0; i<numPages; i+=10){
		cout<<endl;
		cout<<setw(4)<<"  ID "<<setw(6)<<"   PgNo  "<<setw(1)<<" Dirty  "<<setw(3)<<"Hits  ";
		cout<<setw(1)<<"PagePriority   "<<setw(1)<<"FD"<<endl;
		int max = (numPages - i)>10?10:(numPages-i);
		for(int j=i; j<i+max; j++){
			if(pl[j].id != -1){
				cout<<setw(4)<<((MemBuffer *)(bufferPool+(pl[j].id)*sizeofbuffer))->bufferId<<"  ";
				cout<<setw(6)<<((MemBuffer *)(bufferPool+(pl[j].id)*sizeofbuffer))->pageNo<<"       ";
				cout<<setw(1)<<((MemBuffer *)(bufferPool+(pl[j].id)*sizeofbuffer))->dirty<<"  ";
				cout<<setw(3)<<((MemBuffer *)(bufferPool+(pl[j].id)*sizeofbuffer))->numHits<<"   ";
				cout<<setw(1)<<((MemBuffer *)(bufferPool+(pl[j].id)*sizeofbuffer))->p_priority<<"              ";
				cout<<setw(1)<<((MemBuffer *)(bufferPool+(pl[j].id)*sizeofbuffer))->mdtID<<"  ";
				cout<<endl;
			}
		}
		cout<<endl;
		cin.clear();
		cout<<"Press n to interrupt, any other key to continue"<<endl;
		string c;
		if(i==0)
			getchar();
		c=getchar();
		if(c=="n")
			break;
	}
}


void BufferManager::printHex(int buffId, int size){
	int i, j;
	int offset = buffId*sizeofbuffer;
	const unsigned char *p = reinterpret_cast<const unsigned char *>(bufferPool+offset);
	cout<<"      ";
	for(j=0;j<16;j++){
		cout<<setfill('0')<<setw(2)<<setbase(16)<<(j)<<" ";
	}
	cout<<endl;
	for(i=0; i<size; i+=16){
		cout<<setfill('0')<<setw(4)<<setbase(16)<<i<<"  ";
		int max = (size - i)>16?16:(size-i);
		for(j=0;j<max;j++){
			cout<<setfill('0')<<setw(2)<<hex<<int(p[i+j]);
			cout<<" ";
		}
		cout<<setbase(10);
		cout<<endl;
	}
	
}


//void BufferManager::printCacheHeaders(int i){
	//cout<<"Sizeof each buffer ="<< sizeofbuffer <<"\n";
	//cout<<"Sizeof each header ="<< sizeofheader <<"\n";
	//for(int i=0; i<numPages; i++){
	//	cout<<setbase(16);
	//	cout<<"\nBufferId in Buffer:"<<((MemBuffer *)(bufferPool+i*sizeofbuffer))->bufferId;
	//	cout<<"\nPageNumber in Buffer:"<<((MemBuffer *)(bufferPool+i*sizeofbuffer))->pageNo;
	//	cout<<"\nDirtyBit in Buffer:"<<((MemBuffer *)(bufferPool+i*sizeofbuffer))->dirty;
	//	cout<<"\nNumber of Hits in Buffer:"<<((MemBuffer *)(bufferPool+i*sizeofbuffer))->numHits;
	//	cout<<"\nPagePriority in Buffer:"<<((MemBuffer *)(bufferPool+i*sizeofbuffer))->p_priority;
	//	cout<<"\nTimePriority in Buffer:"<<((MemBuffer *)(bufferPool+i*sizeofbuffer))->t_priority;
	//	cout<<"\nFileDescriptor in Buffer:"<<((MemBuffer *)(bufferPool+i*sizeofbuffer))->mdtID;
	//	cout<<setbase(10)<<"\n";
	//}
	//i = i+1;
	//printAllCache();
//}
