
#include"../cachemgr/src/BufferManager.h"
#include"globals_index.h"
#include"../DBEngine/IndexQuery.h"

int getFreePage(DB*);
int addFreePageList(DB * curDB,int pageID);

int SIZEOFSTR;
int FANOUT;
int key_size;
string data_type;
string primary;
int size_header;
int length;

int BufferManagerInterface();

template <class T>
class btree
{
	public:	
	struct internal_node
	{
		int level;
		int num_nodes;
		int page_no;	
		int parent_node;
		int page_priority;
		vector<T> key;//[FANOUT+1];
		vector<int> ptrs;//[FANOUT+2];
	};
	struct leaf_node
	{
		int level;
		int num_nodes;
		int page_no;
		int parent_node;
		int page_priority;
		int next;
		int prev;
		vector<T> key;//[FANOUT+1];
		vector<RecordID> rid;//[FANOUT+1];
	}leaf;
	int _pageSize;
	internal_node *root;	
	int found;
	string index_file;
	T step;
	btree(IndexQuery*);
	IndexQuery* start(IndexQuery*);
	IndexQuery* create(IndexQuery*);
	IndexQuery* insert_val(IndexQuery*);
	IndexQuery* insert_check(IndexQuery*);
	IndexQuery* search(IndexQuery*);
	IndexQuery* update_key(IndexQuery*);
	IndexQuery* update_record(IndexQuery*);
	IndexQuery* delete_key(IndexQuery*);
	IndexQuery* delete_record(IndexQuery*);
	IndexQuery* drop(IndexQuery*);
	IndexQuery* return_result(IndexQuery*,RecordID*);
	T find_limit(char[]);
	
	leaf_node* read_leaf_page(int,T);
	internal_node* read_internal_page(int,int,T);
	void write_leaf_page(leaf_node*,T);
	void write_internal_page(internal_node*,T);
	IndexQuery* deleting(T,RecordID,IndexQuery*);
	void delete_leaf(leaf_node*,T,internal_node*,RecordID);
	void combine_leaf(leaf_node*,internal_node*,T); 
	void combine_internal(internal_node*,internal_node*);
	IndexQuery* delete_dup(T,IndexQuery*,int*);
	IndexQuery* bulk_delete(T,T,IndexQuery*);

	IndexQuery* insert_key(T,RecordID,IndexQuery*);
	void insert_leaf(leaf_node*,T,internal_node*,RecordID,IndexQuery*);
	void insert_internal(internal_node*,T,internal_node*,internal_node*,IndexQuery*);
	void split_leaf(leaf_node*,internal_node*,RecordID,IndexQuery*);
	void split_internal(internal_node*,internal_node*,IndexQuery*);
	IndexQuery* bulk_insert(T,T,T,IndexQuery*);
	
	leaf_node* search_internal(internal_node *,T);
	leaf_node* insert_search_internal(internal_node *,T);
	leaf_node* search_leaf(leaf_node *,T,RecordID,int*,int*);
	leaf_node* search_insert_leaf(leaf_node *,T,RecordID,int*,int*);
	leaf_node* search_bulk_leaf(leaf_node *, T ,int *,int *);
	leaf_node* searching_neighbor(T,RecordID,int*,int *);
	leaf_node* searching_node(T,RecordID,int*);
	leaf_node* search_for_neighbor(leaf_node *,T,RecordID,int*,int *);
	leaf_node* compare(leaf_node*,T,RecordID,int*);
	
	int compare_data(T,T,string);
	int searching(T,RecordID);
	int searching_insert(T,RecordID);
	leaf_node* searching_key(T,int*,int*);
	RecordID* bulk_search(T,T,IndexQuery*);
	RecordID* search_dup(T,IndexQuery*,int*);
	
	IndexQuery* modify(T,RecordID,IndexQuery*);
	
	int compute_fanout(int);	
};

template <class T>
btree<T>::btree(IndexQuery *query)
{
	T val;
	BufferManager *bu = BufferManager::getBufferManager();
	_pageSize = (*bu).getPageSize();
	SIZEOFSTR = query->charLength;
	length = query->charLength;
	data_type = query->dataType;
	key_size = query->charLength;
	if(query->isPrimary==true)	
	primary = "y";
	else 
	primary = "n";

	if(query->rootPageID==-1)
	{
		FANOUT = compute_fanout(key_size);
		query->fanOutChanged = true;
		query->newFanOut = FANOUT;
		root = new internal_node;
		root->key.resize(FANOUT+1);
		root->ptrs.resize(FANOUT+2);
		root->page_no = getFreePage(query->curDB);
		leaf_node *l_temp = new leaf_node;
		l_temp->key.resize(FANOUT+1);
		l_temp->rid.resize(FANOUT+1);
		l_temp->level=0;
		l_temp->page_priority = 1;
		l_temp->num_nodes=0;
		l_temp->page_no = getFreePage(query->curDB); //Calling KP's get free page structure...
		l_temp->next=-1;
		l_temp->prev=-1;
	
	
		root->level=1;
		root->page_priority = 3;
		root->num_nodes=0;
		root->ptrs[0]=l_temp->page_no;
		root->parent_node = -1;
		l_temp->parent_node = root->page_no;
		//buffer = get_free_page();
	
		write_internal_page(root,val);
		write_leaf_page(l_temp,val);
	}
	else
	{
		root = read_internal_page(query->rootPageID,3,val);
		FANOUT = query->fanOut;
		query->fanOutChanged = false;
	}
}

template<class T>
IndexQuery* btree<T>::start(IndexQuery *query)
{
	int choice,num,i;
	T n,n1,s_dup;
	T val;
	IndexQuery *result;
	string index,data,index_file,filename;
	RecordID rid_temp,rid;	
	if(query->queryType==1)
	{
		result = create(query);
	}	
	else if(query->queryType==2)
	{ 
		result = insert_val(query);
	}
	else if(query->queryType==3)
	{
		result = insert_check(query);
	}
	else if(query->queryType==4)
	{
		result = search(query);
	}
	else if(query->queryType==5)
	{
		result = update_key(query);	
	}
	else if(query->queryType==6)
	{
		result = update_record(query);
	}
	else if(query->queryType==7)
	{
		result = delete_key(query); //deletion jst on d basis of key
	}
	else if(query->queryType==8)
	{
		result = delete_record(query); //deletion on d basis of key and rid
	}
	else if(query->queryType==9)
	{
		result = drop(query);	
	}
	return result;
	
}



template <class T>
IndexQuery* btree<T>::create(IndexQuery *query)
{
	data_type = query->dataType;
	key_size = query->charLength;
	FANOUT = compute_fanout(key_size);
	query->fanOut = FANOUT;
	query->rootPageID = root->page_no;
	return query;
}

template <class T>
IndexQuery* btree<T>::insert_val(IndexQuery *query)
{
	RecordID rid;
	int length;
	T num,val;
	root = read_internal_page(query->rootPageID,3,val);
	if(data_type=="int")
		length = sizeof(int);
	else if(data_type=="string")
	   length = query->charLength;
	else if(data_type=="float")
	   length = sizeof(float);
	else if(data_type=="short")
	   length = sizeof(short);
	else if(data_type=="double")
	   length = sizeof(double);
	else if(data_type=="long")
	   length = sizeof(long);
	else if(data_type=="char")
	   length = sizeof(char);
	else
	   length = 20;
	int j=0;
	memcpy(&num,&query->key[j],length);
	memcpy(&rid,&(query->keyRecord),sizeof(RecordID)); 
	query = insert_key(num,rid,query);
	return query;		
}

template <class T>
IndexQuery* btree<T>::insert_check(IndexQuery *query)
{
	T num,val;
	int r=0,p=-1;
	int *flag = &r,*pos = &p;
	struct leaf_node *l_temp = NULL;
	root = read_internal_page(query->rootPageID,3,val);
	if(data_type=="int")
		length = sizeof(int);
	else if(data_type=="string")
	   length = query->charLength;
	else if(data_type=="float")
	   length = sizeof(float);
	else if(data_type=="short")
	   length = sizeof(short);
	else if(data_type=="double")
	   length = sizeof(double);
	else if(data_type=="long")
	   length = sizeof(long);
	else if(data_type=="char")
	   length = sizeof(char);
	else
	   length = 20;
	int j=0;
	memcpy(&num,&query->key[j],length);
	l_temp = searching_key(num,flag,pos);
	if(*flag==1)
	{
		query->errorFlag = true;
		query->errorNum = 500; //Value already exists
	}
	return query;
}

template <class T>
IndexQuery* btree<T>::search(IndexQuery *query)
{
	int r=0,p=-1,l=0,j;
	int *flag = &r,*pos = &p;
	int *check = &l;
	T num;
	RecordID *record;
	j=0;
	struct leaf_node *l_temp = NULL;
	memcpy(&num,&query->key[j],query->charLength);
	if((string(query->operatorID)).compare("=")==0)
	{
		if(primary=="y")
		{
			l_temp = searching_key(num,flag,pos);
			if(*flag==1)
			{
				(*(query->keyRecords[0])).dataPageID = (l_temp->rid[*pos]).dataPageID; 
				(*(query->keyRecords[0])).slotID = (l_temp->rid[*pos]).slotID; 
				query->totalResults = 1;
				query->returnedKeys = 1;
			}
			else
			{
				query->errorFlag = true;
				query->errorNum = 501; //Value doesnt exists
			}
		}
		else if(primary=="n")
		{
			record = search_dup(num,query,check); //searched duplicates for a key and returns result set in record...
			if(*check==0)
			{
				query->errorFlag = true;
				query->errorNum = 501; //Value doesnt exists
				query->lastKeyID = 0;
			}
			else
			{
				query = return_result(query,record);
			}
		}
	}
	else if(string(query->operatorID).compare("<=")==0 || string(query->operatorID).compare("<")==0)
	{
		T v1 = find_limit(query->operatorID);
		record = bulk_search(v1,num,query);	
		query = return_result(query,record);
	}
	else if(string(query->operatorID).compare(">=")==0 || string(query->operatorID).compare(">")==0)
	{
		T v2 = find_limit(query->operatorID);
		record = bulk_search(num,v2,query);
		query = return_result(query,record);
	}	
	return query;	
}

template <class T>
IndexQuery* btree<T>::update_key(IndexQuery *query)
{
	T num;
	RecordID rid;
	int j=0;
	length = query->charLength;
	memcpy(&num,&query->oldKey,length);
	memcpy(&rid,&query->oldKeyRecord,sizeof(RecordID));
	query = modify(num,rid,query);
	return query;
}

template <class T>
IndexQuery* btree<T>::update_record(IndexQuery *query)
{
	T num;
	RecordID rid;
	int j=0;
	length = query->charLength;
	memcpy(&num,&query->oldKey,length);
	memcpy(&rid,&query->oldKeyRecord,sizeof(RecordID));
	query = modify(num,rid,query);
	return query;
}

template <class T>
IndexQuery* btree<T>::delete_key(IndexQuery *query)
{
	int r=0,p=-1,l=0,j;
	int *flag = &r,*pos = &p;
	int *check = &l;
	T num;
	RecordID rid;
	struct leaf_node* l_temp = NULL;
	j=0;
	memcpy(&num,&query->key[j],query->charLength);
	if((string(query->operatorID)).compare("=")==0)
	{
		if(primary=="y")
		{
			l_temp = searching_key(num,flag,pos);
			if(*flag==1)
			{
				rid.dataPageID = l_temp->rid[*pos].dataPageID;
				rid.slotID = l_temp->rid[*pos].slotID;
				query = deleting(num,rid,query);
			}
			else
			{
				query->errorFlag = true;
				query->errorNum = 501; //Value doesnt exists
			}
		}
		else if(primary=="n")
		{
			query = delete_dup(num,query,check); //searched duplicates for a key and returns result set in record...
			if(*check==0)
			{
				query->errorFlag = true;
				query->errorNum = 501; //Value doesnt exists
				query->lastKeyID = 0;
			}
		}
	}
	else if((string(query->operatorID)).compare("<=")==0)
	{
		T v1 = find_limit(query->operatorID);
		query = bulk_delete(v1,num,query);	
	}
	else if((string(query->operatorID)).compare(">=")==0)
	{
		T v2 = find_limit(query->operatorID);
		query = bulk_delete(num,v2,query);
	}	
	return query;	
}

template <class T>
IndexQuery* btree<T>::delete_record(IndexQuery *query)
{
	T num;
	RecordID rid;
	int j=0;
	length = query->charLength;
	memcpy(&num,&query->key[j],length);
	memcpy(&rid,&query->keyRecord,sizeof(RecordID));
	query = deleting(num,rid,query);
	return query;
}

template <class T>
IndexQuery* btree<T>::drop(IndexQuery *query)
{
	int i,count=0;
	T val;
	int page_num;
	internal_node *int_temp = new internal_node;
	internal_node *int_temp_1 = new internal_node;
	int_temp->key.resize(FANOUT+1);	
	int_temp->ptrs.resize(FANOUT+2);	
	leaf_node *l_temp = new leaf_node;
	leaf_node *temp_leaf = new leaf_node;
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);	
	queue<internal_node*> level_order;
	page_num = root->page_no;
	root = read_internal_page(page_num,root->page_priority,val);
	level_order.push(root);

	while(!level_order.empty())
	{
		int_temp = level_order.front();
		level_order.pop();
		if(int_temp->level==0)
		{
			l_temp = (leaf_node*)int_temp;
			addFreePageList(query->curDB,l_temp->page_no); //adding all pages in a queue and calling addFreePageList one by one,...
		}
		else
		{ 
			addFreePageList(query->curDB,int_temp->page_no);
			for(i=0;i<=int_temp->num_nodes && int_temp->level>1;i++)
			{
				page_num = int_temp->ptrs[i];
				int_temp_1 = read_internal_page(page_num,2,val);
				level_order.push(int_temp_1);
			}
			if(int_temp->level==1)
			{
				for(i=0;i<=int_temp->num_nodes;i++)
				{
					page_num = int_temp->ptrs[i];
					temp_leaf = read_leaf_page(page_num,val);
					int_temp_1 = (internal_node*)temp_leaf;
					level_order.push(int_temp_1);
				}
			}
		}
	}
	delete(l_temp);
	delete(int_temp);
	delete(int_temp_1);
	delete(temp_leaf);
}

template <class T>
IndexQuery* btree<T>::return_result(IndexQuery *query,RecordID *record)
{
	int j;
	if(query->lastKeyID !=1) //when not set
	{		
		if(query->limitKeys < query->totalResults)
		{	
			query->returnedKeys = query->limitKeys;
			query->lastKeyID = 1;
			j=1;
			while(j<=query->limitKeys)
			{
				(*(query->keyRecords[j])).dataPageID = (record[j]).dataPageID;
				(*(query->keyRecords[j])).slotID = (record[j]).slotID;
			}
			(query->lastRecordID).dataPageID = record[j-1].dataPageID;
			(query->lastRecordID).slotID = record[j-1].slotID;
		}
		else
		{
			query->lastKeyID = 0;
			query->returnedKeys = query->totalResults;
			j=1;
			while(j<=query->totalResults)
			{
				(*(query->keyRecords[j])).dataPageID = (record[j]).dataPageID;
				(*(query->keyRecords[j])).slotID = (record[j]).slotID;
				j++;
			}
			(query->lastRecordID).dataPageID = record[j-1].dataPageID;
			(query->lastRecordID).slotID = record[j-1].slotID;
		}
	}
	else
	{
		 //again search n send d remaining... searching again bcz d earlier set will be lost
		int count = (query->totalResults - query->returnedKeys);
		if(count <= query->limitKeys)
		{
			j=1;
			int i = count+1;
			while(j<=count)
			{
				(*(query->keyRecords[j])).dataPageID = (record[i]).dataPageID;
				(*(query->keyRecords[j])).slotID = (record[i]).slotID;
				j++;
				i++;
				query->lastKeyID = 0;
			}
			(query->lastRecordID).dataPageID = record[i-1].dataPageID;
			(query->lastRecordID).slotID = record[i-1].slotID;
			query->returnedKeys = query->returnedKeys + (j-1);
		}
		else
		{
			j=1;
			int i = count+1;
			while(j <= query->limitKeys)
			{
				(*(query->keyRecords[j])).dataPageID = (record[i]).dataPageID;
				(*(query->keyRecords[j])).slotID = (record[i]).slotID;
				j++;
				i++;
				query->lastKeyID = 1;
			}
			(query->lastRecordID).dataPageID = record[i-1].dataPageID;
			(query->lastRecordID).slotID = record[i-1].slotID;
			query->returnedKeys = query->returnedKeys + (j-1);	
		}
	}
	return query;
}


template <class T>
T btree<T>::find_limit(char* operatorID)
{
	T val,num;
	int page_num;
	if(string(operatorID).compare("<=")==0 || string(operatorID).compare("<")==0)
	{
		internal_node *int_temp = NULL;
		int_temp = read_internal_page(root->page_no,root->page_priority,val);
		while(int_temp->level >1)
		{
			page_num = int_temp->ptrs[0];
			int_temp = read_internal_page(page_num,2,val);
		}
		page_num = int_temp->ptrs[0];
		leaf_node *temp_leaf = read_leaf_page(page_num,val);
		num = temp_leaf->key[0];
		delete(int_temp);
		delete(temp_leaf);
	}
	else if(string(operatorID).compare(">=")==0 || string(operatorID).compare(">")==0)
	{
		internal_node *int_temp = NULL;
		int_temp = read_internal_page(root->page_no,root->page_priority,val);
		while(int_temp->level >1)
		{
			page_num = int_temp->ptrs[int_temp->num_nodes];
			int_temp = read_internal_page(page_num,2,val);
		}
		page_num = int_temp->ptrs[int_temp->num_nodes];
		leaf_node *temp_leaf = read_leaf_page(page_num,val);
		num = temp_leaf->key[int_temp->num_nodes-1];
		delete(int_temp);
		delete(temp_leaf);
	}
	return num;
} 

template <class T>
int btree<T>::compute_fanout(int key_size)
{
	int fan;
	fan = (_pageSize - (7*sizeof(int) + PAGEMETADATA))/ (key_size + sizeof(int)+ sizeof(int)) ;
	return fan;
}


template <class T>
typename btree<T>::leaf_node* btree<T>::compare(struct leaf_node *l_temp,T num,RecordID rid, int *pos)
{
	int i;
	int check=0;
	T val;
	if(primary=="y")
	{
		*pos = l_temp->num_nodes;
		for(i=0;i<l_temp->num_nodes;i++)
		{
			if(compare_data(num,l_temp->key[i],data_type)<0)
			{
				*pos = i;
				check=1;
				break;
			}
			if(compare_data(num,l_temp->key[i],data_type)==0)
			{
				*pos = i;
				check = 1;
				break;
			}
		}
		if(check == 0 && l_temp->next!=-1)
		{
			int page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			if(l_temp->page_no!=0)
			{
				if(l_temp->num_nodes!=0)
				{
					if(compare_data(l_temp->key[0],num,data_type)==0)
						l_temp = compare(l_temp,num,rid,pos);
					else
					{
						page_num = l_temp->prev;
						l_temp = read_leaf_page(page_num,val);
						*pos = l_temp->num_nodes;
					}
				}
			}
			else
			return NULL;
		}		
	}
	else if(primary=="n")
	{
		*pos = l_temp->num_nodes;
		for(i=0;i<l_temp->num_nodes;i++)
		{
			if(compare_data(num,l_temp->key[i],data_type)<=0)
			{
				if(compare_data(num,l_temp->key[i],data_type)==0)
				{
					if(rid.dataPageID < l_temp->rid[i].dataPageID)
					{
						*pos = i;
						check = 1;
						break;	
					}
					else if(rid.dataPageID == l_temp->rid[i].dataPageID)
					{
						if(rid.slotID <= l_temp->rid[i].slotID)
						{
							*pos = i;
							check = 1;
							break;
						}
					}
				}
				else
				{
					*pos = i;
					check = 1;
					break;
				}
			}
			else
			check = 0;
		}
		if(check == 0 && l_temp->next!=-1)
		{
			int page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			if(l_temp->page_no!=0)
			{
				if(l_temp->num_nodes!=0)
				{
					if(compare_data(l_temp->key[0],num,data_type)==0)
						l_temp = compare(l_temp,num,rid,pos);
					else
					{
						page_num = l_temp->prev;
						l_temp = read_leaf_page(page_num,val);
						*pos = l_temp->num_nodes;
					}
				}
			}
			else
			return NULL;
		}
	}
	return l_temp;
}


template <class T>
RecordID* btree<T>::search_dup(T num,IndexQuery *query,int *flag)
{
	int res1,count = 0;
	int *pos = &res1;
	int position = 0;
	T i,val;
	int check = 1;
	RecordID *record;
	*pos = 0;
	leaf_node *l_temp = new leaf_node;
	//leaf_node *leaf_arr;
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	l_temp = searching_key(num,flag,pos);
	//cout<<endl<<"l_temp->page_no: "<<l_temp->page_no<<endl;
	if(*flag==1)
	{
		i = l_temp->key[*pos];
		while(i==num)
		{
			count++;
			record[count] = l_temp->rid[*pos]; 
			*pos = *pos+1;
			//i = l_temp->key[*pos];
			if(*pos == l_temp->num_nodes)
			{
				//check1 = 0;
				int page_num = l_temp->next;
				if(page_num!=-1)
				{
					l_temp = read_leaf_page(page_num,val);
					*pos = 0;
				}
				else
				{
					*pos = -1;
					break;
				}
			}
			i = l_temp->key[*pos];
		}
	}
	else
	{
		*flag = 0;
	}
	query->totalResults = count;
	return record;
}


template <class T>
RecordID* btree<T>::bulk_search(T n1,T n2,IndexQuery *query)
{
	//int count;
	RecordID tr;
	RecordID *record;
	T i;
	T val;
	int res,res1;
	int page_num;
	//RecordID rid;
	//T num;
	int * flag = &res;
	*flag = 0;
	int *pos = &res1;
	*pos = 0;
	int count=1;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	l_temp = searching_key(n1,flag,pos);
	if(*flag ==1)
	{
		//l_temp = searching_node(n1,tr,pos);
		tr.slotID = (l_temp->rid[*pos]).slotID;
		tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
		i = l_temp->key[*pos];
	}
	else if(*flag!=1)
	{
		//l_temp = searching_neighbor(n1,tr,pos);
		if(*pos <= (l_temp->num_nodes-1))
		{
			//*pos = *pos +1;
			tr.slotID = (l_temp->rid[*pos]).slotID;
			tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
			i = l_temp->key[*pos];
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			*pos = 0;
			tr.slotID = (l_temp->rid[*pos]).slotID;
			tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
			i = l_temp->key[*pos];
		}
	}
	while(i<=n2 && l_temp!=NULL)
	{
		record[count].dataPageID = tr.dataPageID;
		record[count].slotID = tr.slotID;
		searching(i,tr);
		//read_leaf_page(l_temp);
		if(*pos < (l_temp->num_nodes-1))
		{
			*pos = *pos +1;
			i = l_temp->key[*pos];
			tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
			tr.slotID = (l_temp->rid[*pos]).slotID; 
			count++;
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			if(page_num!=0)
			{
				l_temp = read_leaf_page(page_num,val);
				*pos = 0;
				i = l_temp->key[*pos];
				tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
				tr.slotID = (l_temp->rid[*pos]).slotID; 
				count++;
			}
			else
				l_temp = NULL;
		}
		else
		{
			break;
		}	
	}	
	query->totalResults = count-1;
	return record;	
}


template <class T>
typename btree<T>::leaf_node* btree<T>::search_leaf(leaf_node *l_temp,T num,RecordID rid,int* flag,int *pos)
{
	int i,pos1,page_num;
	T val;
	leaf_node* temp_leaf = new leaf_node;	
	temp_leaf->key.resize(FANOUT+1);	
	temp_leaf->rid.resize(FANOUT+1);	
	temp_leaf = compare(l_temp,num,rid,pos);
	if(compare_data(l_temp->key[*pos],num,data_type)==0)
	{
		if(l_temp->rid[*pos].slotID==rid.slotID && l_temp->rid[*pos].dataPageID==rid.dataPageID)
		{
			*flag=1;
			cout<<endl<<"Value "<<l_temp->key[*pos]<<" found at level "<<l_temp->level<<" ,page_no "<<l_temp->page_no<<" and position "<<*pos;
		}
	}
	/*for(i=0;i<l_temp->num_nodes;i++)
	{
		if(compare_data(l_temp->key[i],num,data_type)==0)
		{
			
			if(l_temp->rid[i].slotID==rid.slotID && l_temp->rid[i].dataPageID==rid.dataPageID)
			{
				cout<<endl<<"Number "<<l_temp->key[i]<<" found at level "<<l_temp->level<<" ,page_no "<<l_temp->page_no<<" and position "<<i+1;
				pos1=2;
				*pos = i;
				*flag = 1;
				temp_leaf = l_temp;
				break;
			}
			else pos1=1;
		}
	}
	if(pos1==1 && l_temp->next!=-1)
	{
		page_num = l_temp->next;
		l_temp = read_leaf_page(page_num,val);
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes!=0)
			{
				if(compare_data(l_temp->key[0],num,data_type)==0)
					temp_leaf = search_leaf(l_temp,num,rid,flag,pos);
			}
		}
		else
		return NULL;
	}*/
	return temp_leaf;
}
	

template <class T>
typename btree<T>::leaf_node* btree<T>::search_insert_leaf(leaf_node *l_temp,T num,RecordID rid,int* flag,int *pos)
{
	int i,pos1,page_num;
	T val;
	leaf_node* temp_leaf = new leaf_node;	
	temp_leaf->key.resize(FANOUT+1);	
	temp_leaf->rid.resize(FANOUT+1);
	if(primary=="y")
	{	
		for(i=0;i<l_temp->num_nodes;i++)
		{
			if(compare_data(l_temp->key[i],num,data_type)==0)
			{		
				cout<<endl<<"Value "<<l_temp->key[i]<<" found at level "<<l_temp->level<<" ,page_no "<<l_temp->page_no<<" and position "<<i+1;
				pos1=2;
				*pos = i;
				*flag = 1;
				temp_leaf = l_temp;
				break;
			}
			else pos1=1;
		}
		if(pos1==1 && l_temp->next!=-1)
		{	
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			if(l_temp->page_no!=0)
			{
				if(l_temp->num_nodes!=0)
				{
					if(compare_data(l_temp->key[0],num,data_type)==0)
						temp_leaf = search_insert_leaf(l_temp,num,rid,flag,pos);
				}
			}
			else
			return NULL;
		}	
		return temp_leaf;
	}
	else if(primary=="n")
	{
		for(i=0;i<l_temp->num_nodes;i++)	
		{
			if(compare_data(l_temp->key[i],num,data_type)==0)
			{
				if(l_temp->rid[i].slotID==rid.slotID && l_temp->rid[i].dataPageID==rid.dataPageID)
				{
					cout<<endl<<"Value "<<l_temp->key[i]<<" with same RID found at level "<<l_temp->level<<" and position "<<i+1;
					pos1=2;
					*pos = i;
					*flag = 1;
					temp_leaf = l_temp;
					break;
				}
				else 
				pos1=1;				
			}
		}
		if(pos1==1 && l_temp->next!=-1)
		{
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			if(l_temp->page_no!=0)
			{
				if(l_temp->num_nodes!=0)
				{
					if(compare_data(l_temp->key[0],num,data_type)==0)
						temp_leaf = search_insert_leaf(l_temp,num,rid,flag,pos);
				}
			}
			else
			return NULL;
		}
		return temp_leaf;
	}
}


template <class T>
typename btree<T>::leaf_node* btree<T>::search_bulk_leaf(leaf_node *l_temp,T num,int* flag,int *pos)
{
	int i,pos1,page_num;
	T val;
	leaf_node* temp_leaf = new leaf_node;	
	temp_leaf->key.resize(FANOUT+1);	
	temp_leaf->rid.resize(FANOUT+1);	
	for(i=0;i<l_temp->num_nodes;i++)
	{
		if(compare_data(num,l_temp->key[i],data_type)==0)
		{
			//cout<<endl<<"Number found at level "<<l_temp->level<<" and position "<<i+1;
			pos1=2;
			*pos = i;
			*flag = 1;
			temp_leaf = l_temp;
			break;
		}
		else if(compare_data(num,l_temp->key[i],data_type)<0)
		{
			//cout<<endl<<"Number found at level "<<l_temp->level<<" and position "<<i+1;
			pos1=2;
			*pos = i;
			*flag = 0;
			temp_leaf = l_temp;
			break;
		}
		else pos1=1;
	}
	if(pos1==1 && l_temp->next!=-1)
	{
		page_num = l_temp->next;
		l_temp = read_leaf_page(page_num,val);
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes!=0)
			{
				if(compare_data(l_temp->key[0],num,data_type)==0)
					temp_leaf = search_bulk_leaf(l_temp,num,flag,pos);
			}
		}
		else
		return NULL;
	}
	return temp_leaf;
}



template <class T>
typename btree<T>::leaf_node* btree<T>::search_for_neighbor(leaf_node *l_temp,T num,RecordID rid,int* flag,int *pos)
{
	int i,pos1=0,page_num;
	T val;
	leaf_node* temp_leaf = new leaf_node;	
	temp_leaf->key.resize(FANOUT+1);	
	temp_leaf->rid.resize(FANOUT+1);	
	for(i=0;i<l_temp->num_nodes;i++)
	{
		if(compare_data(num,l_temp->key[i],data_type)<0)
		{
			//cout<<endl<<"Number found at level "<<l_temp->level<<" and position "<<i+1;
			*pos = i;
			*flag = 1;
			pos1 = 1;
			temp_leaf = l_temp;
			break;
		}
		if(compare_data(num,l_temp->key[i],data_type)==0)
		{
			//cout<<endl<<"Number found at level "<<l_temp->level<<" and position "<<i+1;
			*pos = i;
			*flag = 2;
			pos1 = 2;
			temp_leaf = l_temp;
			break;
		}
		else
		pos1=0;
		
	}
	if(pos1==0 && l_temp->next!=-1)
	{
		page_num = l_temp->next;
		l_temp = read_leaf_page(page_num,val);
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes!=0)
			{
				if(compare_data(num,l_temp->key[0],data_type)<=0)
					temp_leaf = search_for_neighbor(l_temp,num,rid,flag,pos);
			}
		}
		else 
		return NULL;
	}
	return temp_leaf;
}

template <class T>
typename btree<T>::leaf_node* btree<T>::search_internal(internal_node *int_temp,T num)
{
	int i,pos;
	int page_num;
	T val;
	leaf_node *temp_node_leaf = NULL;
	page_num = root->page_no;
	//cout<<endl<<"Reading root...."<<endl;
	root = read_internal_page(page_num,root->page_priority,val);
	//cout<<endl<<"Read root...."<<endl;
	pos=int_temp->num_nodes;
	if(root->num_nodes==0)
	{ 		
		page_num = root->ptrs[0];
		//cout<<endl<<"Reading r712...."<<endl;
		temp_node_leaf = read_leaf_page(page_num,val);
		//cout<<endl<<"Reading r714...."<<endl;
		return (temp_node_leaf);		
	}
	while(int_temp->level >= 1)
	{
		pos=int_temp->num_nodes;
		for(i=0;i<int_temp->num_nodes;i++)
		{
			if(compare_data(num,int_temp->key[i],data_type)<=0)
			{
				pos = i;			
				break;
			}
		}
		page_num = int_temp->ptrs[pos];
		if(int_temp->level > 1)
			int_temp = read_internal_page(page_num,2,val);
		else
		{
			temp_node_leaf = read_leaf_page(page_num,val);
			int_temp = (internal_node*)temp_node_leaf;
		}
		//}
		//int_temp = int_temp->ptrs[pos];
	}	
	return ((leaf_node*)int_temp);
}

template <class T>
typename btree<T>::leaf_node* btree<T>::insert_search_internal(internal_node *int_temp,T num)
{
	int i,pos,page_num;
	T val;
	leaf_node *temp_leaf = new leaf_node;
	pos=int_temp->num_nodes;
	//cout<<endl<<"Pos:"<<pos<<endl;
	while(int_temp->level>=1)
	{
		pos = int_temp->num_nodes;
		for(i=0;i<int_temp->num_nodes;i++)
		{
			//cout<<"i:"<<i<<endl;
			//cout<<"key:"<<int_temp->key[i]<<endl;
			if(compare_data(int_temp->key[i],num,data_type)==0)
				 found=1;
			if(compare_data(num,int_temp->key[i],data_type)<=0)
			{
				pos = i;			
				break;
			}
		}
		page_num = int_temp->ptrs[pos];
		if(int_temp->level > 1)
		{
			int_temp = read_internal_page(page_num,2,val);
		}
		else
		{
			temp_leaf = read_leaf_page(page_num,val);
			int_temp = (internal_node*)temp_leaf;	
			//int_temp = int_temp->ptrs[pos];
		}
	}
	return ((leaf_node*)int_temp);
}

template <class T>
int btree<T>::searching(T num,RecordID rid)
{
	int res,res1;
	int page_num;
	//RecordID rid;
	//T num;
	T val;
	int *flag = &res;
	*flag = 0;
	int *pos = &res1;
	*pos = 0;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	page_num = root->page_no;
	root = read_internal_page(page_num,root->page_priority,val);	
	if(root->num_nodes==0)
	{
		page_num = root->ptrs[0];
		l_temp = read_leaf_page(page_num,val);
		//l_temp=(leaf_node*)root->ptrs[0];
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes==0) 
				 cout<<endl<<"No values inserted yet";
			else
				l_temp = search_leaf(l_temp,num,rid,flag,pos);
		}
		else
		return -1;
	}
	else
	{
		l_temp = search_internal(root,num);
		l_temp = search_leaf(l_temp,num,rid,flag,pos);
	}
	return (*flag);
}

template <class T>
int btree<T>::searching_insert(T num,RecordID rid)
{
	int res,res1;
	int page_num;
	//RecordID rid;
	//T num;
	T val;
	int *flag = &res;
	*flag = 0;
	int *pos = &res1;
	*pos = 0;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	page_num = root->page_no;
	root = read_internal_page(page_num,root->page_priority,val);	
	if(root->num_nodes==0)
	{
		page_num = root->ptrs[0];
		l_temp = read_leaf_page(page_num,val);
		//l_temp=(leaf_node*)root->ptrs[0];
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes!=0) 
				l_temp = search_insert_leaf(l_temp,num,rid,flag,pos);
		}
		else
		return -1;
	}
	else
	{
		l_temp = search_internal(root,num);
		l_temp = search_insert_leaf(l_temp,num,rid,flag,pos);
	}
	//if(*flag==0)
	//	cout<<endl<<"Key not present";
	return (*flag);
}

template <class T>
typename btree<T>::leaf_node* btree<T>::searching_key(T num,int *flag,int *pos)
{
	int page_num;
	T val;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	page_num = root->page_no;
	root = read_internal_page(page_num,root->page_priority,val);	
	if(root->num_nodes==0)
	{	
		page_num = root->ptrs[0];
		l_temp = read_leaf_page(page_num,val);
		//l_temp=(leaf_node*)root->ptrs[0];
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes==0)  
				*flag = 0;
			else
				l_temp = search_bulk_leaf(l_temp,num,flag,pos);
		}
	}
	else
	{
		l_temp = search_internal(root,num);
		l_temp = search_bulk_leaf(l_temp,num,flag,pos);
	}
	/*if(*flag==0)
		cout<<endl<<"Key not present";
	return (*flag);*/
	return l_temp;
}

template <class T>
typename btree<T>::leaf_node* btree<T>::searching_node(T num,RecordID rid,int *pos)
{
	int i,res,page_num;
	//RecordID rid;
	//T num;
	T val;
	int * flag = &res;
	*flag = 0;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);	
	page_num = root->page_no;
	root = read_internal_page(page_num,root->page_priority,val);
	if(root->num_nodes==0)
	{
		page_num = root->ptrs[0];
		l_temp = read_leaf_page(page_num,val);
		//l_temp=(leaf_node*)root->ptrs[0];
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes==0)
				cout<<endl<<"No values inserted yet";
			else
				l_temp = search_leaf(l_temp,num,rid,flag,pos);
		}
		else
		return NULL;
	}
	else
	{
		l_temp = search_internal(root,num);
		l_temp = search_leaf(l_temp,num,rid,flag,pos);
	}
	//if(*flag==0)
	//	cout<<endl<<"Key not present";
	return l_temp;
}

template <class T>
typename btree<T>::leaf_node* btree<T>::searching_neighbor(T num,RecordID rid,int *flag,int *pos)
{
	int i,res,res1;
	//RecordID rid;
	//T num;
	T val;
	int page_num;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);	
	page_num = root->page_no;
	root = read_internal_page(page_num,root->page_priority,val);
	if(root->num_nodes==0)
	{
		page_num = root->ptrs[0];
		l_temp = read_leaf_page(page_num,val);
		//l_temp=(leaf_node*)root->ptrs[0];
		if(l_temp->page_no!=0)
		{
			if(l_temp->num_nodes==0) 
				 cout<<endl<<"No values inserted yet";
			else
				l_temp = search_for_neighbor(l_temp,num,rid,flag,pos);
		}
		else
		return NULL;
		
	}
	else
	{
		l_temp = search_internal(root,num);
		l_temp = search_for_neighbor(l_temp,num,rid,flag,pos);
	}
	//if(*flag==0)
	//	cout<<endl<<"Key not present";
	return l_temp;
}
	

template<class T>
int btree<T>::compare_data(T data1, T data2,string data_type)
{	
	if(data1<data2)
	return -1;
	else if(data1==data2)
	return 0;
	
	return 1;
}

template<>
int btree<string>::compare_data(string data1,string data2,string data_type)
{
	//cout<<"inside string compare data, before compare"<<endl;
	int val = data1.compare(data2);
	//cout<<"inside string compare data, after compare"<<endl;
	if(val==0)
	return 0;
	else if(val<0)
	return -1;
	//else if(val>0)
	return 1;
    
}

template<class T>
IndexQuery* btree<T>::modify(T data,RecordID rid,IndexQuery *query)
{
	int res,page_no,slot_no,j;
	T val,num;
	int *pos = &res;
	*pos = 0;
	RecordID tr;
	int flag = searching(data,rid);
	if(flag==0)
	{
		//cout<<"RID you are trying to modify doesnt exist ";
		query->errorFlag = true;
		query->errorNum = 501;
	}
	else
	{
		if(query->queryType==5)
		{
			query = deleting(data,rid,query);
			memcpy(&num,&query->newKey,query->newKeyLength);
			memcpy(&tr,&query->newKeyRecord,sizeof(RecordID));
			query = insert_key(num,tr,query);
		}
		else if(query->queryType==6)
		{
			if(query->isPrimary==true)
			{
				struct leaf_node *l_temp = searching_node(data,rid,pos);
				j=0;
				memcpy(&tr,&query->newKeyRecord,sizeof(RecordID));
				l_temp->rid[*pos].dataPageID = 	tr.dataPageID;
				l_temp->rid[*pos].slotID = tr.slotID;
				write_leaf_page(l_temp,val);
			}
			else
			{
				j=0;
				query = deleting(data,rid,query);
				memcpy(&tr,&query->newKeyRecord,sizeof(RecordID));
				query = insert_key(num,tr,query);
			}
		}
	}
	return query;	
}


template<class T>
typename btree<T>::leaf_node* btree<T>::read_leaf_page(int page_no,T val)
{
	int level, num_nodes,parent_node,next,prev,page_priority;
	//int i,j;
	T num;
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
	//cout<<endl<<"Leaf Page number to read:"<<page_no<<endl;
	char *buffer_page = new char[_pageSize];
	
	(*bu).readDB(0,page_no,PagePriority(1),buffer_page);
	RecordID rid;
	size_header = 8*sizeof(int) + sizeof(short);
	memcpy(&level,&buffer_page[LEVEL],sizeof(int));
	memcpy(&num_nodes,&buffer_page[NUM_NODES],sizeof(int));
	memcpy(&page_no,&buffer_page[PAGE_NO],sizeof(int));
	//cout<<endl<<"Leaf Page number after memcpy to read:"<<page_no<<endl;
	memcpy(&parent_node,&buffer_page[PARENT_NODE],sizeof(int));
	memcpy(&(page_priority),&(buffer_page[PAGE_PRIORITY]),sizeof(int));
	memcpy(&next,&buffer_page[NEXT],sizeof(int));
	memcpy(&prev,&buffer_page[PREV],sizeof(int));	
	if(level==0 && page_no!=0)
	{
		struct leaf_node *l_read = new leaf_node;
		l_read->level = level;
		l_read->num_nodes = num_nodes;
		l_read->page_no = page_no;
		l_read->page_priority = page_priority;
		l_read->parent_node = parent_node;
		l_read->next = next;
		l_read->prev = prev;
		int j = 0; 
		for(int i=0;i<l_read->num_nodes;i++)
		{
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
			//cout<<endl<<"Length:"<<length<<endl;
			///////////////////////
			
			if(data_type!="string")
			{
				memcpy(&num,&buffer_page[j],length);
				j = j + length;
				memcpy(&rid,&buffer_page[j],sizeof(RecordID)); //how to transfer
				j = j + sizeof(RecordID);
				(l_read->key).push_back(num); //value addition thru vector
				(l_read->rid).push_back(rid);
			} 
			/*else
			{
				//string num1;
				char str[256];
				memcpy(&str,&buffer_page[j],length);
				T num1(str);
				j = j + length;
				memcpy(&rid,&buffer_page[j],sizeof(RecordID)); 
				j = j + sizeof(RecordID);
				(l_read->key).push_back(num1); 
				(l_read->rid).push_back(rid);
			}

			if(data_type!="string")
			memcpy(&num,&buffer_page[j],length);
			else
			{
				char str[256];
				strcpy(str,num.c_str());
				memcpy(&str,&buffer_page[j],length); 
			}
			j = j + length;
			memcpy(&rid,&buffer_page[j],sizeof(RecordID)); //how to transfer
			j = j + sizeof(RecordID);
			(l_read->key).push_back(num); //value addition thru vector
			(l_read->rid).push_back(rid);*/
		}
		delete(buffer_page);
		return l_read;
	}
	//cout<<endl<<"Problem is not here but somewhere else..."<<endl;
	delete(buffer_page);
	return NULL;
}

template<>
typename btree<string>::leaf_node* btree<string>::read_leaf_page(int page_no, string val)
{
	int level, num_nodes,parent_node,next,prev,page_priority;
	//int i,j;
	string num;
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
	char *buffer_page = new char[_pageSize];
	
	//cout<<endl<<"Leaf Page number to read:"<<page_no<<endl;
	(*bu).readDB(0,page_no,PagePriority(1),buffer_page);
	RecordID rid;
	size_header = 8*sizeof(int) + sizeof(short);
	memcpy(&level,&buffer_page[LEVEL],sizeof(int));
	memcpy(&num_nodes,&buffer_page[NUM_NODES],sizeof(int));
	memcpy(&page_no,&buffer_page[PAGE_NO],sizeof(int));
	memcpy(&parent_node,&buffer_page[PARENT_NODE],sizeof(int));
	memcpy(&(page_priority),&(buffer_page[PAGE_PRIORITY]),sizeof(int));
	memcpy(&next,&buffer_page[NEXT],sizeof(int));
	memcpy(&prev,&buffer_page[PREV],sizeof(int));	
	if(level==0 && page_no!=0)
	{
		struct leaf_node *l_read = new leaf_node;
		l_read->level = level;
		l_read->num_nodes = num_nodes;
		l_read->page_no = page_no;
		l_read->page_priority = page_priority;
		l_read->parent_node = parent_node;
		l_read->next = next;
		l_read->prev = prev;
		int j = 0; 
		for(int i=0;i<l_read->num_nodes;i++)
		{
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
			//cout<<endl<<"Length:"<<length<<endl;
			///////////////////////
			
			if(data_type!="string")
			{
				memcpy(&num,&buffer_page[j],length);
				j = j + length;
				memcpy(&rid,&buffer_page[j],sizeof(RecordID)); //how to transfer
				j = j + sizeof(RecordID);
				(l_read->key).push_back(num); //value addition thru vector
				(l_read->rid).push_back(rid);
			} 
			else
			{
				//string num1;
				char str[256];
				memcpy(&str,&buffer_page[j],length);
				string num1(str);
				j = j + length;
				memcpy(&rid,&buffer_page[j],sizeof(RecordID)); 
				j = j + sizeof(RecordID);
				(l_read->key).push_back(num1); 
				(l_read->rid).push_back(rid);
			}
		}
		delete(buffer_page);
		return l_read;
	}
	delete(buffer_page);
	//cout<<endl<<"Problem is not here but somewhere else..."<<endl;
	return NULL;
}


template<class T>
typename btree<T>::internal_node* btree<T>::read_internal_page(int page_no,int page_priority,T val)
{
	int level, num_nodes,parent_node,next,prev,ptrs;
	int i,j;
	T num;
	//cout<<endl<<"Internal Page number to read:"<<page_no<<endl;
	//RecordID rid;
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
	char *buffer_page = new char[_pageSize];
	//cout<<endl<<"Allocated buffer for page number:"<<page_no<<endl;
	
	(*bu).readDB(0,page_no,PagePriority(page_priority),buffer_page);	
	//cout<<endl<<"Read page number:"<<page_no<<". Now copying values in local variables. "<<endl;
	size_header = 8*sizeof(int) + sizeof(short);
	memcpy(&level,&(buffer_page[LEVEL]),sizeof(int));
	memcpy(&num_nodes,&(buffer_page[NUM_NODES]),sizeof(int));
	memcpy(&page_no,&(buffer_page[PAGE_NO]),sizeof(int));
	memcpy(&parent_node,&(buffer_page[PARENT_NODE]),sizeof(int));
	memcpy(&(page_priority),&(buffer_page[PAGE_PRIORITY]),sizeof(int));
	memcpy(&next,&(buffer_page[NEXT]),sizeof(int));
	memcpy(&prev,&(buffer_page[PREV]),sizeof(int));	
	//cout<<endl<<" Now copied values in local variables. page number:"<<page_no<<". "<<endl;
	if(level!=0 && page_no!=0)
	{
		struct internal_node *int_read = new internal_node;
		int_read->level = level;
		int_read->num_nodes = num_nodes;
		int_read->page_no = page_no;
		int_read->page_priority = page_priority;
		int_read->parent_node = parent_node;
		//int_read->next = -1;
		//int_read->prev = -1;
		j = 0; 
		for(i=0;i<int_read->num_nodes;i++)
		{
			memcpy(&ptrs,&buffer_page[j],sizeof(int));
			j = j + sizeof(int);
			/////
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
			//cout<<endl<<"Length:"<<length<<endl;
			if(data_type!="string")
			{
				memcpy(&num,&buffer_page[j],length);
				j = j + length;
				(int_read->ptrs).push_back(ptrs);
				(int_read->key).push_back(num);
			} 
			/*else
			{
				//string num1;
				char str[256];
				memcpy(&str,&buffer_page[j],length);
				string num1(str);
				j = j + length;
				(int_read->ptrs).push_back(ptrs);
				(int_read->key).push_back(num1);
				//num = num1;
			}*/
			
		}
		memcpy(&ptrs,&buffer_page[j],sizeof(int));
		(int_read->ptrs).push_back(ptrs);
		delete(buffer_page);
		return int_read;	
	}
	delete(buffer_page);
	return NULL;	
}

template<>
typename btree<string>::internal_node* btree<string>::read_internal_page(int page_no,int page_priority, string val)
{
	int level, num_nodes,parent_node,next,prev,ptrs;
	int i,j;
	string num;
	//cout<<endl<<"Internal Page number to read:"<<page_no<<endl;
	//RecordID rid;
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
	char *buffer_page = new char[_pageSize];
	//cout<<endl<<"Allocated buffer for page number:"<<page_no<<endl;
	
	(*bu).readDB(0,page_no,PagePriority(page_priority),buffer_page);	
	//cout<<endl<<"Read page number:"<<page_no<<". Now copying values in local variables. "<<endl;
	size_header = 8*sizeof(int) + sizeof(short);
	memcpy(&level,&(buffer_page[LEVEL]),sizeof(int));
	memcpy(&num_nodes,&(buffer_page[NUM_NODES]),sizeof(int));
	memcpy(&page_no,&(buffer_page[PAGE_NO]),sizeof(int));
	memcpy(&parent_node,&(buffer_page[PARENT_NODE]),sizeof(int));
	memcpy(&(page_priority),&(buffer_page[PAGE_PRIORITY]),sizeof(int));
	memcpy(&next,&(buffer_page[NEXT]),sizeof(int));
	memcpy(&prev,&(buffer_page[PREV]),sizeof(int));	
	//cout<<endl<<" Now copied values in local variables. page number:"<<page_no<<". "<<endl;
	if(level!=0 && page_no!=0)
	{
		struct internal_node *int_read = new internal_node;
		int_read->level = level;
		int_read->num_nodes = num_nodes;
		int_read->page_no = page_no;
		int_read->page_priority = page_priority;
		int_read->parent_node = parent_node;
		//int_read->next = -1;
		//int_read->prev = -1;
		j = 0; 
		for(i=0;i<int_read->num_nodes;i++)
		{
			memcpy(&ptrs,&buffer_page[j],sizeof(int));
			j = j + sizeof(int);
			/////
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
			//cout<<endl<<"Length:"<<length<<endl;
			if(data_type!="string")
			{
				memcpy(&num,&buffer_page[j],length);
				j = j + length;
				(int_read->ptrs).push_back(ptrs);
				(int_read->key).push_back(num);
			} 
			else
			{
				//string num1;
				char str[256];
				memcpy(&str,&buffer_page[j],length);
				string num1(str);
				j = j + length;
				(int_read->ptrs).push_back(ptrs);
				(int_read->key).push_back(num1);
				//num = num1;
			}
			
		}
		memcpy(&ptrs,&buffer_page[j],sizeof(int));
		(int_read->ptrs).push_back(ptrs);
		delete(buffer_page);
		return int_read;	
	}
	delete(buffer_page);
	return NULL;	
}


template <class T>
void btree<T>::write_leaf_page(struct leaf_node *l_read, T val)
{
	char *buffer_page;
	T num;
	RecordID rid;
	//struct leaf_node *temp =NULL;
	int level,num_nodes,page_no,parent_node,next,prev,page_priority;
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
	buffer_page = new char[_pageSize];
	

	//page_num = l_read->page_no;
	//temp = read_leaf_page(page_num);
	
	if(l_read!=NULL && l_read->page_no!=0)
	{
		level = l_read->level;
		num_nodes = l_read->num_nodes;
		page_no = l_read->page_no;
		parent_node = l_read->parent_node;
		page_priority = l_read->page_priority;
		next = l_read->next;
		prev = l_read->prev;
		//cout<<endl<<"Leaf Page number to write:"<<page_no<<endl;
		memcpy(&(buffer_page[LEVEL]),&(level),sizeof(int));
		memcpy(&(buffer_page[NUM_NODES]),&(num_nodes),sizeof(int));
		memcpy(&(buffer_page[PAGE_NO]),&(page_no),sizeof(int));
		memcpy(&(buffer_page[PARENT_NODE]),&(parent_node),sizeof(int));
		memcpy(&(buffer_page[PAGE_PRIORITY]),&(page_priority),sizeof(int));
		memcpy(&(buffer_page[NEXT]),&(next),sizeof(int));
		memcpy(&(buffer_page[PREV]),&(prev),sizeof(int));
		int j = 0; 
		for(int i=0;i<l_read->num_nodes;i++)
		{
			num = l_read->key[i];
			rid = l_read->rid[i];
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
			//cout<<endl<<"Length:"<<length<<endl;
			if(data_type!="string")
			memcpy(&buffer_page[j],&num,length);
			/*else
			{
				char str[256];
				strcpy(str,num.c_str());
				memcpy(&buffer_page[j],&str,length); 
			}*/
			//cout<<"\n Size of data type:"<<length;
			j = j + length;
			memcpy(&buffer_page[j],&rid,sizeof(RecordID)); //how to transfer
			j = j + sizeof(RecordID);
			//cout<<"\n Size of RID:"<<sizeof(RecordID);
		}
		(*bu).writeDB(0,page_no,PagePriority(page_priority),buffer_page);
		//return buffer_page; (Call chinmay's function)
	}
	delete(buffer_page);
}

template <>
void btree<string>::write_leaf_page(struct leaf_node *l_read,string val)
{
	char *buffer_page;
	string num;
	RecordID rid;
	//struct leaf_node *temp =NULL;
	int level,num_nodes,page_no,parent_node,next,prev,page_priority;
	BufferManager *bu = BufferManager::getBufferManager();
	buffer_page = new char[_pageSize];
	/*call chinmay's function */

	//page_num = l_read->page_no;
	//temp = read_leaf_page(page_num);
	
	if(l_read!=NULL && l_read->page_no!=0)
	{
		level = l_read->level;
		num_nodes = l_read->num_nodes;
		page_no = l_read->page_no;
		parent_node = l_read->parent_node;
		page_priority = l_read->page_priority;
		next = l_read->next;
		prev = l_read->prev;
		//cout<<endl<<"Leaf Page number to write:"<<page_no<<endl;
		memcpy(&(buffer_page[LEVEL]),&(level),sizeof(int));
		memcpy(&(buffer_page[NUM_NODES]),&(num_nodes),sizeof(int));
		memcpy(&(buffer_page[PAGE_NO]),&(page_no),sizeof(int));
		memcpy(&(buffer_page[PARENT_NODE]),&(parent_node),sizeof(int));
		memcpy(&(buffer_page[PAGE_PRIORITY]),&(page_priority),sizeof(int));
		memcpy(&(buffer_page[NEXT]),&(next),sizeof(int));
		memcpy(&(buffer_page[PREV]),&(prev),sizeof(int));
		int j = 0; 
		for(int i=0;i<l_read->num_nodes;i++)
		{
			num = l_read->key[i];
			rid = l_read->rid[i];
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
			//cout<<endl<<"Length:"<<length<<endl;
			if(data_type!="string")
			memcpy(&buffer_page[j],&num,length);
			else
			{
				char str[256];
				strcpy(str,num.c_str());
				memcpy(&buffer_page[j],&str,length); 
			}
			//cout<<"\n Size of data type:"<<length;
			j = j + length;
			memcpy(&buffer_page[j],&rid,sizeof(RecordID)); //how to transfer
			j = j + sizeof(RecordID);
			//cout<<"\n Size of RID:"<<sizeof(RecordID);
		}
		(*bu).writeDB(0,page_no,PagePriority(page_priority),buffer_page);
		//return buffer_page; (Call chinmay's function)
	}
	delete(buffer_page);
}


template <class T>
void btree<T>::write_internal_page(struct internal_node *i_read,T val)
{
	char *buffer_page;
	T num;
	int ptrs;
	int i,j;
	int next = -1, prev = -1;
	int level,num_nodes,page_no,parent_node,page_priority;
	BufferManager *bu = BufferManager::getBufferManager();
	buffer_page = new char[_pageSize];
	if(i_read!=NULL && i_read->page_no!=0)
	{
		level = i_read->level;
		num_nodes = i_read->num_nodes;
		page_no = i_read->page_no;
		parent_node = i_read->parent_node;
		page_priority = i_read->page_priority;
		
		//cout<<endl<<"Internal Page number to write:"<<page_no<<endl;
		memcpy(&(buffer_page[LEVEL]),&(level),sizeof(int));
		memcpy(&(buffer_page[NUM_NODES]),&(num_nodes),sizeof(int));
		memcpy(&(buffer_page[PAGE_NO]),&(page_no),sizeof(int));
		memcpy(&(buffer_page[PARENT_NODE]),&(parent_node),sizeof(int));
		memcpy(&(buffer_page[PAGE_PRIORITY]),&(page_priority),sizeof(int));
		memcpy(&(buffer_page[NEXT]),&(next),sizeof(int));
		memcpy(&(buffer_page[PREV]),&(prev),sizeof(int));
		j = 0; 
		for(i=0;i<i_read->num_nodes;i++)
		{
			num = i_read->key[i];
			ptrs = i_read->ptrs[i];
			memcpy(&buffer_page[j],&ptrs,sizeof(int)); //how to transfer
			j = j + sizeof(int);
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
		//	cout<<endl<<"Length:"<<length<<endl;
			/////////////////////////////
			if(data_type!="string")
			memcpy(&buffer_page[j],&num,length);
			/*else
			{
				char str[256];
				strcpy(str,num.c_str());
				memcpy(&buffer_page[j],&str,length); 
			}*/
			//memcpy(&buffer_page[j],&num,length);
			j = j + length;
		}
		ptrs = i_read->ptrs[i];
		memcpy(&buffer_page[j],&ptrs,sizeof(int));
		(*bu).writeDB(0,page_no,PagePriority(page_priority),buffer_page);
		//return buffer_page; (Call chinmay's function)
	}
	delete(buffer_page);
}

template <>
void btree<string>::write_internal_page(struct internal_node *i_read,string val)
{
	char *buffer_page;
	string num;
	int ptrs;
	int i,j;
	int next = -1, prev = -1;
	int level,num_nodes,page_no,parent_node,page_priority;
	BufferManager *bu = BufferManager::getBufferManager();
	buffer_page = new char[_pageSize];
	if(i_read!=NULL && i_read->page_no!=0)
	{
		level = i_read->level;
		num_nodes = i_read->num_nodes;
		page_no = i_read->page_no;
		parent_node = i_read->parent_node;
		page_priority = i_read->page_priority;
		
		//cout<<endl<<"Internal Page number to write:"<<page_no<<endl;
		memcpy(&(buffer_page[LEVEL]),&(level),sizeof(int));
		memcpy(&(buffer_page[NUM_NODES]),&(num_nodes),sizeof(int));
		memcpy(&(buffer_page[PAGE_NO]),&(page_no),sizeof(int));
		memcpy(&(buffer_page[PARENT_NODE]),&(parent_node),sizeof(int));
		memcpy(&(buffer_page[PAGE_PRIORITY]),&(page_priority),sizeof(int));
		memcpy(&(buffer_page[NEXT]),&(next),sizeof(int));
		memcpy(&(buffer_page[PREV]),&(prev),sizeof(int));
		j = 0; 
		for(i=0;i<i_read->num_nodes;i++)
		{
			num = i_read->key[i];
			ptrs = i_read->ptrs[i];
			memcpy(&buffer_page[j],&ptrs,sizeof(int)); //how to transfer
			j = j + sizeof(int);
			int length =0;
			//cout<<"\nDataType:"<<data_type<<endl;
			if(data_type=="int")
			   length = sizeof(int);
			else if(data_type=="string")
			   length = SIZEOFSTR;
			else if(data_type=="float")
			   length = sizeof(float);
			else if(data_type=="short")
			   length = sizeof(short);
			else if(data_type=="double")
			   length = sizeof(double);
			else if(data_type=="long")
			   length = sizeof(long);
			else if(data_type=="char")
			   length = sizeof(char);
			else
			   length = 20;
		//	cout<<endl<<"Length:"<<length<<endl;
			/////////////////////////////
			if(data_type!="string")
			memcpy(&buffer_page[j],&num,length);
			else
			{
				char str[256];
				strcpy(str,num.c_str());
				memcpy(&buffer_page[j],&str,length); 
			}
			//memcpy(&buffer_page[j],&num,length);
			j = j + length;
		}
		ptrs = i_read->ptrs[i];
		memcpy(&buffer_page[j],&ptrs,sizeof(int));
		(*bu).writeDB(0,page_no,PagePriority(page_priority),buffer_page);
		//return buffer_page; (Call chinmay's function)
	}
	delete(buffer_page);
}

template <class T>
void btree<T>::combine_internal(internal_node* int_temp,internal_node* parent)
{
	int i,pos,val,j,len=0,k;
	T value;
	int page_num;
	T keys[FANOUT],temp1;
	int pointers[FANOUT+1];
	internal_node *temp_int = NULL;
	internal_node *child = NULL;
	internal_node *temp_node = NULL;
	leaf_node *temp_node_leaf = new leaf_node;
	internal_node *tmp_node = NULL;
//Root
	page_num = int_temp->ptrs[0];
	if(int_temp->level > 1)
		child = read_internal_page(page_num,2,value);
	else
	{
		temp_node_leaf = read_leaf_page(page_num,value);
		child = (internal_node*)temp_node_leaf;
	}
	page_num = root->page_no;
	root = read_internal_page(page_num,3,value);
	if(int_temp->page_no==root->page_no)
	{
		if(int_temp->num_nodes==0 && child->level!=0)
		{	
			child->page_priority = 3;
			root=child;
			write_internal_page(root,value);
		}
	}
	else
	{
		for(i=0;i<=parent->num_nodes;i++)
		{	
			if(parent->ptrs[i]==int_temp->page_no)
			{
				pos=i;
				break;
			}
		}	
//Redistributing nodes
		if(pos==0)
		{
			page_num = parent->ptrs[1];
			if(int_temp->level >= 1)
				temp_int = read_internal_page(page_num,2,value);
			else
			{
				temp_node_leaf = read_leaf_page(page_num,value);
				temp_int = (internal_node*)temp_node_leaf;
			}
		}
		else if(pos!=0 && pos<parent->num_nodes)
		{
			page_num = parent->ptrs[pos+1];
			if(int_temp->level >= 1)
				temp_int = read_internal_page(page_num,2,value);
			else
			{
				temp_node_leaf = read_leaf_page(page_num,value);
				temp_int = (internal_node*)temp_node_leaf;
			}
		}
		else if(pos==parent->num_nodes)
		{
			page_num = parent->ptrs[pos-1];
			if(int_temp->level >= 1)
				temp_int = read_internal_page(page_num,2,value);
			else
			{
				temp_node_leaf = read_leaf_page(page_num,value);
				temp_int = (internal_node*)temp_node_leaf;
			}
		}

		if(pos==0 && (temp_int->num_nodes + int_temp->num_nodes)>=FANOUT)//right node redistribution
		{
			if(FANOUT%2==0) 
				val=FANOUT/2;
			else
				 val=(FANOUT/2)+1;	
			j=0;
			len=temp_int->num_nodes;
			(int_temp->key).push_back(parent->key[0]);
			i = int_temp->num_nodes;
			int_temp->ptrs[i+1]=temp_int->ptrs[j];
			int_temp->num_nodes++;
			page_num = temp_int->ptrs[j];
			if(int_temp->level > 1)
				temp_node = read_internal_page(page_num,2,value);
			else
			{
				temp_node_leaf = read_leaf_page(page_num,value);
				temp_node = (internal_node*)temp_node_leaf;
			}
			if(temp_node->level==0)
			{
				temp_node_leaf->parent_node = int_temp->page_no;
				temp_node = (internal_node*)temp_node_leaf;
			}
			else
				temp_node->parent_node = int_temp->page_no;
			
			if(temp_node->level >=1)
				write_internal_page(temp_node,value);
			else
				write_leaf_page((leaf_node*)temp_node,value);

			j=0;

			for(i=int_temp->num_nodes;i<val-1;i++)
			{
				(int_temp->key).push_back(temp_int->key[j]);
				int_temp->ptrs[i+1]=temp_int->ptrs[j+1];
				page_num = temp_int->ptrs[j+1];
				if(int_temp->level > 1)
					temp_node = read_internal_page(page_num,2,value);
				else
				{
					temp_node_leaf = read_leaf_page(page_num,value);
					temp_node = (internal_node*)temp_node_leaf;
				}
				if(temp_node->level==0)
				{
					temp_node_leaf->parent_node = int_temp->page_no;
					temp_node = (internal_node*)temp_node_leaf;
				}
				else
					temp_node->parent_node = int_temp->page_no;

				if(temp_node->level >=1)
					write_internal_page(temp_node,value);
				else
					write_leaf_page((leaf_node*)temp_node,value);

				j++;
				temp_int->num_nodes--;
				int_temp->num_nodes++;
			}
			
			j++;
			parent->key[0] = temp_int->key[j-1];
			//temp_int->num_nodes--;
			i=0;		
			temp_int->num_nodes=0;
			for(;j<len;j++)
			{
				temp_int->key[i] = temp_int->key[j];
				temp_int->ptrs[i]=temp_int->ptrs[j];
				i++;
				temp_int->num_nodes++;
			}
			
			temp_int->ptrs[i]=temp_int->ptrs[j];
			
			write_internal_page(parent,value);
			if(int_temp->level >= 1)
				write_internal_page(temp_int,value);
			else
				write_leaf_page((leaf_node*)temp_int,value);

			if(int_temp->level >= 1)
				write_internal_page(int_temp,value);
			else
				write_leaf_page((leaf_node*)int_temp,value);
			
		}	
		else if(pos==parent->num_nodes && (temp_int->num_nodes + int_temp->num_nodes)>=FANOUT)//left
		{
			if(FANOUT%2==0)
				val=FANOUT/2;
			else 
				val=(FANOUT/2)+1;
			for(i=0;i<int_temp->num_nodes;i++)
			{
				keys[i] = int_temp->key[i];
				pointers[i]=int_temp->ptrs[i];
			}
			pointers[i]=int_temp->ptrs[i];
			len=i;
			k=temp_int->num_nodes;	
			i=(temp_int->num_nodes-(val-int_temp->num_nodes));
			temp1 = parent->key[pos-1];
			parent->key[pos-1] = temp_int->key[i];
			temp_int->num_nodes--;
			i++;
			int_temp->num_nodes=0;
			
			//(int_temp->key).push_back(parent)
			j=0;
			for(;i<k;i++)
			{
				//int_temp->key[j]=temp_int->key[i];
				(int_temp->key).push_back(temp_int->key[i]);
				int_temp->ptrs[j]= temp_int->ptrs[i]; //added +1 
				page_num = temp_int->ptrs[i];
				if(int_temp->level > 1)
					temp_node = read_internal_page(page_num,2,value);
				else
				{
					temp_node_leaf = read_leaf_page(page_num,value);
					temp_node = (internal_node*)temp_node_leaf;
				}
				if(temp_node->level==0)
				{
					temp_node_leaf->parent_node = int_temp->page_no;
					temp_node = (internal_node*)temp_node_leaf;
				}
				else
					temp_node->parent_node = int_temp->page_no;
				
				if(temp_node->level >=1)
					write_internal_page(temp_node,value);
				else
					write_leaf_page((leaf_node*)temp_node,value);
				j++;
				temp_int->num_nodes--;
				int_temp->num_nodes++;
			}
			int_temp->ptrs[j]= temp_int->ptrs[i];//check this and all similar
			page_num = temp_int->ptrs[j];
			if(int_temp->level > 1)
				temp_node = read_internal_page(page_num,2,value);
			else
			{
				temp_node_leaf = read_leaf_page(page_num,value);
				temp_node = (internal_node*)temp_node_leaf;
			}
			if(temp_node->level==0)
			{
				temp_node_leaf->parent_node = int_temp->page_no;
				temp_node = (internal_node*)temp_node_leaf;
			}
			else
				temp_node->parent_node = int_temp->page_no;
			
			if(temp_node->level >=1)
				write_internal_page(temp_node,value);
			else
				write_leaf_page((leaf_node*)temp_node,value);

			int_temp->key[j] = temp1;
			int_temp->num_nodes++;
			j++;
			for(i=0;i<len;i++)
			{
				int_temp->key[j] = keys[i];
				int_temp->ptrs[j]=pointers[i];
				j++;
				int_temp->num_nodes++;
			}
			int_temp->ptrs[j]=pointers[i];
				

			write_internal_page(parent,value);

			if(int_temp->level >= 1)
				write_internal_page(temp_int,value);
			else
				write_leaf_page((leaf_node*)temp_int,value);

			if(int_temp->level >= 1)
				write_internal_page(int_temp,value);
			else
				write_leaf_page((leaf_node*)int_temp,value);
		
		}	
		else if(pos!=parent->num_nodes && pos!=0 && (temp_int->num_nodes + int_temp->num_nodes)>=FANOUT)//right
		{
			if(FANOUT%2==0) 
				val=FANOUT/2;
			else
				val=(FANOUT/2)+1;	
			j=0;

			len=temp_int->num_nodes;
			(int_temp->key).push_back(parent->key[pos]);
			i = int_temp->num_nodes;
			int_temp->ptrs[i+1]=temp_int->ptrs[j];
			int_temp->num_nodes++;
			page_num = temp_int->ptrs[j];
			if(int_temp->level > 1)
				temp_node = read_internal_page(page_num,2,value);
			else
			{
				temp_node_leaf = read_leaf_page(page_num,value);
				temp_node = (internal_node*)temp_node_leaf;
			}
			if(temp_node->level==0)
			{
				temp_node_leaf->parent_node = int_temp->page_no;
				temp_node = (internal_node*)temp_node_leaf;
			}
			else
				temp_node->parent_node = int_temp->page_no;
			
			if(temp_node->level >=1)
				write_internal_page(temp_node,value);
			else
				write_leaf_page((leaf_node*)temp_node,value);

			j=0;

			for(i=int_temp->num_nodes;i<val-1;i++)
			{
				(int_temp->key).push_back(temp_int->key[j]);
				int_temp->ptrs[i+1]=temp_int->ptrs[j+1];
				page_num = temp_int->ptrs[j+1];
				if(int_temp->level > 1)
					temp_node = read_internal_page(page_num,2,value);
				else
				{
					temp_node_leaf = read_leaf_page(page_num,value);
					temp_node = (internal_node*)temp_node_leaf;
				}
				if(temp_node->level==0)
				{
					temp_node_leaf->parent_node = int_temp->page_no;
					temp_node = (internal_node*)temp_node_leaf;
				}
				else
					temp_node->parent_node = int_temp->page_no;

				if(temp_node->level >=1)
					write_internal_page(temp_node,value);
				else
					write_leaf_page((leaf_node*)temp_node,value);

				j++;
				temp_int->num_nodes--;
				int_temp->num_nodes++;
			}
			
			j++;
			parent->key[pos] = temp_int->key[j-1];
			i=0;		
			temp_int->num_nodes=0;
			for(;j<len;j++)
			{
				temp_int->key[i] = temp_int->key[j];
				temp_int->ptrs[i]=temp_int->ptrs[j];
				i++;
				temp_int->num_nodes++;
			}
			//parent->key[pos] = temp_int->key[0];
			temp_int->ptrs[i]=temp_int->ptrs[j];
			
			write_internal_page(parent,value);
			if(int_temp->level >= 1)
				write_internal_page(temp_int,value);
			else
				write_leaf_page((leaf_node*)temp_int,value);

			if(int_temp->level >= 1)
				write_internal_page(int_temp,value);
			else
				write_leaf_page((leaf_node*)int_temp,value);
		}
//Merging nodes	
		else
		{
			if(pos==0)//merge with right mode
			{
				page_num = parent->ptrs[pos+1];
				temp_int = read_internal_page(page_num,2,value);

				j=int_temp->num_nodes;
				//int_temp->key[j] = parent->key[pos];
				(int_temp->key).push_back(parent->key[pos]);
				int_temp->num_nodes++;
				j++;
				for(i=0;i<temp_int->num_nodes;i++)
				{
					(int_temp->key).push_back(temp_int->key[i]);
					int_temp->ptrs[j]= temp_int->ptrs[i];
					
					page_num = temp_int->ptrs[i];
					if(temp_int->level > 1)
						temp_node = read_internal_page(page_num,2,value);
					else
					{
						temp_node_leaf = read_leaf_page(page_num,value);
						temp_node = (internal_node*)temp_node_leaf;
					}
					if(temp_node->level==0)
					{
						temp_node_leaf->parent_node = int_temp->page_no;
						write_leaf_page(temp_node_leaf,value);
						temp_node = (internal_node*) temp_node_leaf;
					}
					else 
					{
						temp_node->parent_node = int_temp->page_no;
						write_internal_page(temp_node,value);
					}
					j++;
					int_temp->num_nodes++;
				}
				int_temp->ptrs[j] = temp_int->ptrs[i];
				temp_int->num_nodes = 0;
				page_num = temp_int->ptrs[i];
				if(parent->level > 2)
				{
					tmp_node = read_internal_page(page_num,2,value);
				}
				else
				{
					temp_node_leaf = read_leaf_page(page_num,value);
					tmp_node = (internal_node*)temp_node_leaf;
				}
				if(tmp_node->level==0)
				{
					tmp_node->parent_node = int_temp->page_no;
					write_leaf_page(temp_node_leaf,value);
				}
				else 
				{
					tmp_node->parent_node = int_temp->page_no;
					write_internal_page(tmp_node,value);
				}
				for(i=pos;i<parent->num_nodes-1;i++)
				{
					parent->key[i] = parent->key[i+1];
					parent->ptrs[i+1]=parent->ptrs[i+2];
				}
				parent->num_nodes--;	

				write_internal_page(parent,value);
				
				if(temp_int->level >= 1)
					write_internal_page(temp_int,value);
				else
					write_leaf_page((leaf_node*)temp_int,value);
				
	
				/*if(int_temp->level >= 1)
					write_internal_page(temp_node);
				else
					write_leaf_page((leaf_node*)temp_node); 
				
				if(tmp_node->level >= 1)
					write_internal_page(tmp_node);
				else
					write_leaf_page((leaf_node*)tmp_node); */
			}		
			else//merge with the left node
			{
				page_num = parent->ptrs[pos-1];
				if(parent->level > 1)
				temp_int = read_internal_page(page_num,2,value);
				else
				{
					temp_node_leaf = read_leaf_page(page_num,value);
					temp_int = (internal_node*)temp_node_leaf;	
				}
				j=temp_int->num_nodes;
				temp_int->key[j] = parent->key[pos-1];
				temp_int->num_nodes++;
				j++;
				for(i=0;i<int_temp->num_nodes;i++)
				{
					temp_int->key[j] = int_temp->key[i];
					temp_int->ptrs[j]=int_temp->ptrs[i];
					page_num = int_temp->ptrs[i];
					if(int_temp->level > 1)
						temp_node = read_internal_page(page_num,2,value);
					else
					{
						temp_node_leaf = read_leaf_page(page_num,value);
						temp_node = (internal_node*)temp_node_leaf;
					}
					if(temp_node->level==0)
					{
						temp_node_leaf->parent_node = temp_int->page_no;
						write_leaf_page(temp_node_leaf,value);
						temp_node = (internal_node*)temp_node_leaf;
					}
					else
					{
						temp_node->parent_node = temp_int->page_no;
						write_internal_page(temp_node,value);
					}
					j++;
					temp_node->num_nodes++;
				}
				temp_int->ptrs[j]=int_temp->ptrs[i];
				int_temp->num_nodes = 0;
				page_num = int_temp->ptrs[i];
				if(int_temp->level > 1)
					temp_node = read_internal_page(page_num,2,value);
				else
				{
					temp_node_leaf = read_leaf_page(page_num,value);
					temp_node = (internal_node*)temp_node_leaf;
				}
				if(temp_node->level==0)
				{
					temp_node_leaf->parent_node = parent->ptrs[pos-1];
					write_leaf_page(temp_node_leaf,value);
				}
				else
				{
					//temp_node->ptrs[j]=int_temp->ptrs[i];
					temp_node->parent_node = parent->ptrs[pos-1];
					write_internal_page(temp_node,value);
				}
				for(i=pos-1;i<parent->num_nodes-1;i++)
				{
					parent->key[i] = parent->key[i+1];
					parent->ptrs[i+1]=parent->ptrs[i+2];
				}
				parent->num_nodes--;	
				
				write_internal_page(parent,value);

				if(temp_int->level >= 1)
					write_internal_page(temp_int,value);
				else
					write_leaf_page((leaf_node*)temp_int,value);
	
				/*if(temp_node->level >= 1)
					write_internal_page(temp_node);
				else
					write_leaf_page((leaf_node*)temp_node); */
						
			}
			//int_temp = parent->parent_node;
			
			if(int_temp->level >= 1)
				write_internal_page(int_temp,value);
			else
				write_leaf_page((leaf_node*)int_temp,value);
			
			page_num = parent->page_no;
			parent = read_internal_page(page_num,parent->page_priority,value);
			page_num = parent->parent_node;
			if(page_num!=-1)
			{
				int_temp = read_internal_page(page_num,2,value);
				//write_internal_page(int_temp,value);
			}
			else
			{
				int_temp = NULL;
			}
	
			if(parent->num_nodes<(FANOUT/2)) 
				combine_internal(parent,int_temp);	
			
		}
	}
}

template <class T>
void btree<T>::combine_leaf(leaf_node* l_temp,internal_node* parent,T num)
{	
	int i,pos,value,j,len=0,k;
	int page_num,page_priority;
	T val;
	RecordID rids[FANOUT/2];
	T keys[FANOUT/2];
	leaf_node* ln_next=new leaf_node;
	leaf_node* ln_prev=new leaf_node;
	leaf_node *temp = new leaf_node;
	//ln->key.resize(FANOUT+1);	
	//ln->rid.resize(FANOUT+1);	
	internal_node* int_temp=new internal_node;
	int_temp->key.resize(FANOUT+1);	
	int_temp->ptrs.resize(FANOUT+2);	
	int_temp=(internal_node*)l_temp;

	for(i=0;i<=parent->num_nodes;i++) //finding position in the parent
	{
		if(parent->ptrs[i]==int_temp->page_no)
		{
			pos=i;
			break;
		}
	}

	page_num = l_temp->next;
	if(page_num!=-1)
		ln_next = read_leaf_page(page_num,val);
	else
		ln_next = NULL;

	page_num = l_temp->prev;
	if(page_num!=-1)
		ln_prev = read_leaf_page(page_num,val);
	else
		ln_prev = NULL;

	if(l_temp->prev==-1 && l_temp->next==-1)//only node in tree
	{
		if(l_temp->num_nodes==0) 
		{
			root->num_nodes=0;
			write_internal_page(root,val);
		}
	}
	else if(l_temp->prev==-1 && (ln_next->num_nodes + l_temp->num_nodes)>=FANOUT)//node 1 in level 0
	{		
		if(FANOUT%2==0) 
			value=FANOUT/2;
		else 
			value=(FANOUT/2)+1;	
		j=0;
		len=ln_next->num_nodes;
		for(i=l_temp->num_nodes;i<value;i++)
		{
			(l_temp->key).push_back(ln_next->key[j]);
			l_temp->rid[i].slotID=ln_next->rid[j].slotID;
			l_temp->rid[i].dataPageID=ln_next->rid[j].dataPageID;
			j++;
			ln_next->num_nodes--;
			l_temp->num_nodes++;
		}
		i=0;		
		ln_next->num_nodes=0;
		for(;j<len;j++)
		{
			ln_next->key[i] = ln_next->key[j];
			ln_next->rid[i].slotID=ln_next->rid[j].slotID;
			ln_next->rid[i].dataPageID=ln_next->rid[j].dataPageID;
			i++;
			ln_next->num_nodes++;
		}
		parent->key[0] = l_temp->key[l_temp->num_nodes-1];
		write_internal_page(parent,val);
		write_leaf_page(l_temp,val);
		//cout<<endl<<"Nodes redistributed in deletion process";
		write_leaf_page(ln_next,val);
	}
	else if(pos!=0 && (ln_prev->num_nodes + l_temp->num_nodes>=FANOUT))//redistribution with left node
	{
		if(FANOUT%2==0)
			 value=FANOUT/2;
		else
			 value=(FANOUT/2)+1;		
		for(i=0;i<l_temp->num_nodes;i++)
		{
			keys[i] = l_temp->key[i];
			rids[i].slotID=l_temp->rid[i].slotID;
			rids[i].dataPageID=l_temp->rid[i].dataPageID;
		}
		len=i;
		k=ln_prev->num_nodes;	
		i=(ln_prev->num_nodes-(value-l_temp->num_nodes)); //excess nodes....!!!
		l_temp->num_nodes=0;
		j=0;
		for(;i<k;i++)
		{
			l_temp->key[j] = ln_prev->key[i];
			l_temp->rid[j].slotID=ln_prev->rid[i].slotID;
			l_temp->rid[j].dataPageID=ln_prev->rid[i].dataPageID;
			j++;
			ln_prev->num_nodes--;
			l_temp->num_nodes++;
		}
		for(i=0;i<len;i++)
		{
			l_temp->key[j] = keys[i];
			l_temp->rid[j].slotID=rids[i].slotID;
			l_temp->rid[j].dataPageID=rids[i].dataPageID;
			j++;
			l_temp->num_nodes++;
		}
		parent->key[pos-1] = ln_prev->key[ln_prev->num_nodes-1];
		write_internal_page(parent,val);
		write_leaf_page(l_temp,val);
		write_leaf_page(ln_prev,val);
		//cout<<endl<<"Nodes redistributed in deletion process";
	}	
	else if(pos!=parent->num_nodes && (l_temp->num_nodes + ln_next->num_nodes>=FANOUT))//redistribution with right node
	{
		if(FANOUT%2==0)
			 value=FANOUT/2;
		else
			 value=(FANOUT/2)+1;	
		j=0;
		len=ln_next->num_nodes;
		for(i=l_temp->num_nodes;i<value;i++)
		{
			(l_temp->key).push_back(ln_next->key[j]);
			l_temp->rid[i].slotID=ln_next->rid[j].slotID;
			l_temp->rid[i].dataPageID=ln_next->rid[j].dataPageID;
			j++;
			ln_next->num_nodes--;
			l_temp->num_nodes++;
		}
		i=0;		
		ln_next->num_nodes=0;
		for(;j<len;j++)
		{
			ln_next->key[i] = ln_next->key[j];
			ln_next->rid[i].slotID=ln_next->rid[j].slotID;
			ln_next->rid[i].dataPageID=ln_next->rid[j].dataPageID;
			i++;
			ln_next->num_nodes++;
		}
		parent->key[pos] = l_temp->key[l_temp->num_nodes-1];
		write_internal_page(parent,val);
		write_leaf_page(l_temp,val);
		//write_leaf_page(ln_prev);
		write_leaf_page(ln_next,val);
		
		//cout<<endl<<"Nodes redistributed in deletion process";
	}
	else if(l_temp->num_nodes==0 && l_temp->next==-1)
	{
		ln_prev->next=-1;
		parent->num_nodes--;
		page_num = parent->parent_node;
		int_temp = read_internal_page(page_num,2,val);
		write_leaf_page(l_temp,val);
		write_leaf_page(ln_prev,val);
		write_internal_page(parent,val);
		//write_leaf_page(ln_next);
		if(parent->num_nodes<(FANOUT/2)) 
			combine_internal(parent,int_temp);
	}
	else//left and right nodes both not enough
	{
		if(pos==0)//merge with right mode
		{
			j=l_temp->num_nodes;
			for(i=0;i<ln_next->num_nodes;i++)
			{
				(l_temp->key).push_back(ln_next->key[i]);
				l_temp->rid[j].slotID=ln_next->rid[i].slotID;
				l_temp->rid[j].dataPageID=ln_next->rid[i].dataPageID;
				j++;
				l_temp->num_nodes++;
			}
			ln_next->num_nodes = 0;
			if(ln_next->next==-1)
			{
				l_temp->next = -1;
			}
			else
			{
				page_num = ln_next->next;
				temp = read_leaf_page(page_num,val);
				temp->prev = l_temp->page_no;
				l_temp->next = temp->page_no;
				//l_temp->next->next->prev=l_temp;
				//l_temp->next=l_temp->next->next;
			}
			for(i=0;i<parent->num_nodes-1;i++)
			{
				parent->key[i] = parent->key[i+1];
				parent->ptrs[i+1]=parent->ptrs[i+2];
			}
			parent->num_nodes--;		
			write_internal_page(parent,val);
		}		
		else//merge with the left node
		{
			j=ln_prev->num_nodes;
			for(i=0;i<l_temp->num_nodes;i++)
			{
				(ln_prev->key).push_back(l_temp->key[i]);
				ln_prev->rid[j].slotID=l_temp->rid[i].slotID;
				ln_prev->rid[j].dataPageID=l_temp->rid[i].dataPageID;
				j++;
				ln_prev->num_nodes++;
			}
			l_temp->num_nodes = 0;
			if(l_temp->next!=-1)
			{
				ln_prev->next = ln_next->page_no;
				ln_next->prev = ln_prev->page_no;
				//l_temp->prev->next=l_temp->next;
				//l_temp->next->prev=l_temp->prev;
			}
			else 
				ln_prev->next = -1;
			for(i=pos-1;i<parent->num_nodes-1;i++)
			{
				parent->key[i] = parent->key[i+1];
				parent->ptrs[i+1]=parent->ptrs[i+2];
			}
			parent->num_nodes--;	
			write_internal_page(parent,val);		
		}
		page_num = parent->page_no;
		parent = read_internal_page(page_num,parent->page_priority,val);
		page_num = parent->parent_node;
		if(page_num == root->page_no)
		page_priority = 3;
		else
		page_priority = 2;
		int_temp = read_internal_page(page_num,page_priority,val);
		write_leaf_page(l_temp,val);
		write_leaf_page(ln_next,val);
		write_leaf_page(ln_prev,val);
		if(parent->num_nodes<(FANOUT/2)) 
			combine_internal(parent,int_temp);
	}
	//write_internal_page(int_temp);
}

template <class T>
void btree<T>::delete_leaf(leaf_node* l_temp,T num,internal_node* parent,RecordID rid)
{
	int i,pos;
	T val;
	for(i=0;i<l_temp->num_nodes;i++)
	{
		if((compare_data(l_temp->key[i],num,data_type)==0) && l_temp->rid[i].slotID==rid.slotID && l_temp->rid[i].dataPageID==rid.dataPageID)
		{
			pos=i;
			break;
		}
	}
	
	for(i=pos;i<l_temp->num_nodes-1;i++)
	{
		l_temp->key[i] = l_temp->key[i+1]; 
		l_temp->rid[i].slotID=l_temp->rid[i+1].slotID; 
		l_temp->rid[i].dataPageID=l_temp->rid[i+1].dataPageID; 
	}
	if(pos==0 && l_temp->num_nodes==1)
	{
		l_temp->key.erase (l_temp->key.begin());
		l_temp->rid.erase (l_temp->rid.begin());
	}
	
	l_temp->num_nodes--;	
	write_leaf_page(l_temp,val);

	if(l_temp->num_nodes<(FANOUT/2)) 
	combine_leaf(l_temp,parent,num);	

}

template <class T>
IndexQuery* btree<T>::deleting(T num,RecordID rid,IndexQuery* query)
{
	int r=0,res1 = 0;
	int page_num;
	T val;
	int* res,*pos;	
	res = &r;
	pos = &res1;
	found=0;	
	*pos = -1;
	*res = 0;
	leaf_node* l_temp=new leaf_node;
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);	
	root = read_internal_page(root->page_no,3,val);
	if(primary=="y")
	{	
		l_temp = search_internal(root,num);
		l_temp = search_leaf(l_temp,num,rid,res,pos);
	
		if(*res==1)
		{
			internal_node * parent = new internal_node;
			parent->key.resize(FANOUT+1);	
			parent->ptrs.resize(FANOUT+2);	
			page_num = l_temp->parent_node;
			if(page_num == root->page_no)
			parent = read_internal_page(page_num,3,val);		
			else
			parent = read_internal_page(page_num,2,val);		
			delete_leaf(l_temp,num,parent,rid);
		}
		else
		{
			query->errorFlag = true;
			query->errorNum = 501;
		}
	}
	else
	{
		l_temp = search_internal(root,num);
		l_temp = searching_neighbor(num,rid,res,pos);
		
		if(*res==2)
		{
			internal_node * parent = new internal_node;
			parent->key.resize(FANOUT+1);	
			parent->ptrs.resize(FANOUT+2);	
			page_num = l_temp->parent_node;
			if(page_num == root->page_no)
			parent = read_internal_page(page_num,3,val);		
			else
			parent = read_internal_page(page_num,2,val);		
			delete_leaf(l_temp,num,parent,rid);
		}
		else if(*res==0)
		{
			query->errorFlag = true;
			query->errorNum = 501;
		}
	}
	if(query->rootPageID!=root->page_no)
	{
		query->newRootPageID = root->page_no;
		query->rootPageIDUpdated = true;
	}
	return query;
}


template <class T>
IndexQuery* btree<T>::bulk_delete(T n1,T n2,IndexQuery *query)
{
	int res1,res;
	int page_num;
	T val;
	int *pos = &res1;
	int *flag = &res;
	T i = n1;
	RecordID tr;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	*pos = 0;
	*flag = 0;
	l_temp = searching_key(i,flag,pos);

	if(*flag ==1)
	{
		//l_temp = searching_node(n1,tr,pos);
		tr.slotID = (l_temp->rid[*pos]).slotID;
		tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
		i = l_temp->key[*pos];
	}
	else if(*flag!=1)
	{
		//l_temp = searching_neighbor(n1,tr,pos);
		if(*pos <= (l_temp->num_nodes-1))
		{
			//*pos = *pos +1;
			tr.slotID = (l_temp->rid[*pos]).slotID;
			tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
			i = l_temp->key[*pos];
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			*pos = 0;
			tr.slotID = (l_temp->rid[*pos]).slotID;
			tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
			i = l_temp->key[*pos];
		}
	}
	while(i<=n2 && l_temp!=NULL)
	{
		//l_temp = searching_key(i,flag,pos);
		l_temp = searching_neighbor(i,tr,flag,pos);
		query = deleting(i,tr,query);
		
		
		if(*pos <= (l_temp->num_nodes-1))
		{
			//*pos = *pos +1;
			i = l_temp->key[*pos];
			tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
			tr.slotID = (l_temp->rid[*pos]).slotID; 
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			if(page_num!=0)
			{
				l_temp = read_leaf_page(page_num,val);
				*pos = 0;
				i = l_temp->key[*pos];
				tr.dataPageID = (l_temp->rid[*pos]).dataPageID;
				tr.slotID = (l_temp->rid[*pos]).slotID;
			}
			else
				l_temp = NULL; 
		}
		else
		{
			break;
		}
		
		//count++;
	}
	return query;		
}


template <class T>
IndexQuery* btree<T>::delete_dup(T num,IndexQuery *query,int *flag)
{	
	int res1;
	int *pos = &res1;
	int position = 0;
	int page_num,page_num_curr;
	T i,val;
	int check = 1;
	RecordID tr;
	RecordID *record;
	*pos = 0;
	int not_found;
	leaf_node *l_temp = new leaf_node;
	leaf_node *l_next = new leaf_node;
	internal_node *parent = new internal_node;
	parent->key.resize(FANOUT+1);	
	parent->ptrs.resize(FANOUT+2);
	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	l_temp = searching_key(num,flag,pos);
	
	if(*flag==1)
	{
		i = l_temp->key[*pos];
		tr.dataPageID = l_temp->rid[*pos].dataPageID;
		tr.slotID = l_temp->rid[*pos].slotID;
		while(i==num)
		{
			page_num_curr = l_temp->page_no;
			page_num = l_temp->next;
			query = deleting(num,tr,query);
			if(page_num_curr!=-1)
			l_temp = read_leaf_page(page_num_curr,val);
			not_found = 1;
			for(int j=0;j<l_temp->num_nodes;j++)
			{
				if(compare_data(num,l_temp->key[j],data_type)!=0)
				not_found = 1;
				else if(compare_data(num,l_temp->key[j],data_type)==0)
				{
					not_found = 0;
					*pos = j;
					break;
				}
			}
			if(page_num!=-1 && not_found==1)
			{
				page_num = l_temp->next;
				l_temp = read_leaf_page(page_num,val);
				*pos = 0;
			}
			if(*pos!=-1)
			{
				i = l_temp->key[*pos];
				tr.dataPageID = l_temp->rid[*pos].dataPageID;
				tr.slotID = l_temp->rid[*pos].slotID;
			}
		}
	}
	else
	{
		*flag = 0;
	}
	return query;
}

template <class T>
void btree<T>::split_internal(internal_node* child,internal_node* parent,IndexQuery *query)
{
	
	int new_num,old_num,i,j;
	int page_num;
	T val;
	internal_node *int_temp = new internal_node;	
	internal_node *temp = NULL;
	leaf_node *temp_leaf = NULL;
	//int_temp->key.resize(FANOUT+1);
	//int_temp->ptrs.resize(FANOUT+2);
	old_num = (FANOUT+1)/2;
	if((FANOUT+1)%2==1)
	{
	 old_num = old_num+1;
	} 
	new_num = (FANOUT+1)/2;
	child->num_nodes = old_num -1;
	int_temp->page_no = getFreePage(query->curDB);
	int_temp->level = child->level;
	int_temp->num_nodes = new_num;	
	int_temp->page_priority = 2;	

	j = old_num;
	for(i=0;i<new_num;i++,j++)	
	{
		(int_temp->key).push_back(child->key[j]);
	}
	j=0;
	for(i=old_num;i<FANOUT+2;i++,j++)
	{
		(int_temp->ptrs).push_back(child->ptrs[i]);
		child->ptrs[i] = -1;
	}
	for(i=0;i<=new_num;i++)
	{
		if(int_temp->level > 1)
		{
			page_num = int_temp->ptrs[i];
			temp = read_internal_page(page_num,2,val);
			if(temp!=NULL)
			{
				temp->parent_node = int_temp->page_no;
				write_internal_page(temp,val);
				
			}
		}
		else
		{
			page_num = int_temp->ptrs[i];
			
			temp_leaf = read_leaf_page(page_num,val);
			
			if(temp_leaf!=NULL)
			{
				temp_leaf->parent_node = int_temp->page_no;
					
				write_leaf_page(temp_leaf,val);
			}
		}
	}	

	for(i=0;i<=new_num;i++)
	{
		if(int_temp->level > 1)
		{
			page_num = int_temp->ptrs[i];
			temp = read_internal_page(page_num,2,val);
		}
		else
		{
			page_num = int_temp->ptrs[i];
			
			temp_leaf = read_leaf_page(page_num,val);
			
		}
	}	
			
	if(parent!=NULL)
	{
		int_temp->parent_node = parent->page_no;
	}
	else
	int_temp->parent_node = -1;

	write_internal_page(int_temp,val);
	write_internal_page(child,val);
	insert_internal(parent,child->key[old_num-1],child,int_temp,query);
}



template <class T>
void btree<T>::insert_internal(internal_node* node,T num,internal_node* child1,internal_node* child2,IndexQuery *query)
{
	int i,page_priority,r = -1;
	int *pos = &r;
	T temp1,temp2;
	T val;
	int temp1_child,temp2_child,page_num;
	
	if(node == NULL)
	{
		node = new internal_node;
		//node1->key.resize(FANOUT+1);	
		//node1->ptrs.resize(FANOUT+2);	
		node->level=child1->level + 1;
		node->num_nodes=1;
		node->page_no = getFreePage(query->curDB);
		node->page_priority = 3;
		(node->ptrs).push_back(child1->page_no);	
		(node->ptrs).push_back(child2->page_no);
		(node->key).push_back(num);
		page_num = root->page_no;
		root = read_internal_page(page_num,3,val);
		root->page_priority  = 2; //updating root
		write_internal_page(root,val); //writing the updated root
	
		node->parent_node = -1;
		root->page_no = node->page_no;
		root = node; //modify rootID
		query->rootPageIDUpdated = true;
		query->newRootPageID = root->page_no;
		query->rootPageID = root->page_no;
		if(child1->level==0)
		{
			((leaf_node*)child1)->parent_node = root->page_no;
			((leaf_node*)child2)->parent_node = root->page_no;
			write_leaf_page((leaf_node*)child1,val);
			write_leaf_page((leaf_node*)child2,val);			
		}
		else
		{
			child1->parent_node = root->page_no;
			child2->parent_node = root->page_no;	
			write_internal_page(child1,val);
			write_internal_page(child2,val);		
		}
		
		write_internal_page(root,val);
	}		
	else
	{
		if(node->level!=0)
		{
			*pos=node->num_nodes;	
			for(i=0;i<node->num_nodes;i++)
			{			
				if(compare_data(num,node->key[i],data_type)<0)	
				{
					*pos=i;
					break;
				}
				else if(compare_data(num,node->key[i],data_type)==0 && compare_data(num,node->key[i+1],data_type)<0)
				{
					*pos = i;
					break;
				}			
			}
		}
		temp2=num;
		temp2_child = child2->page_no;	
		(node->key).push_back(num);
		(node->ptrs).push_back(-1);
		for(i=*pos;i<=node->num_nodes;i++)
		{
			temp1 = node->key[i];
			node->key[i] = temp2;				
			temp2=temp1;		
			temp1_child = node->ptrs[i+1];
			node->ptrs[i+1] = temp2_child;
			temp2_child =temp1_child;
		}			

		if(node->num_nodes<FANOUT)
		{
			node->num_nodes++;
			write_internal_page(node,val);
		}
		else
		{
			internal_node *parent = NULL;
			//parent->key.resize(FANOUT+1);	
			//parent->ptrs.resize(FANOUT+2);	
			if(node->level==0)
			{
				page_num = ((leaf_node*)node)->parent_node;
				if(page_num!=-1)
				{   
					if(page_num== root->page_no)
						parent = read_internal_page(page_num,3,val);
					else
						parent = read_internal_page(page_num,2,val);
				}		
				else
					parent = NULL;
				
				split_internal(node,parent,query);
				if(parent!=NULL)
				write_internal_page(parent,val);
				write_leaf_page((leaf_node*)node,val);
			}
			else 
			{
				page_num = node->parent_node;

				if(page_num == root->page_no)
				page_priority = 3;
				else
				page_priority = 2;
				
				if(page_num!=-1)
				parent = read_internal_page(page_num,page_priority,val);
				else
				parent = NULL;	
			
				if(parent!=NULL)
				write_internal_page(parent,val);
				write_internal_page(node,val);

				split_internal(node,parent,query);

			}
		}
	}
}

template <class T>
void btree<T>::split_leaf(leaf_node* child,internal_node* parent,RecordID rid,IndexQuery *query)
{
	int new_num,old_num,i,j;
	T val;
	leaf_node *l_temp = new leaf_node;
	leaf_node *l_child = NULL;
	RecordID t1_rid;
	//l_temp->key.resize(FANOUT+1);	
	//l_temp->rid.resize(FANOUT+1);	
	old_num = (FANOUT+1)/2;
	if((FANOUT+1)%2==1) 
	{
		old_num = old_num+1;
	} 
	new_num = (FANOUT+1)/2;
	child->num_nodes = old_num;
	l_temp->level = 0;
	l_temp->page_no = getFreePage(query->curDB); //getFreePage
	l_temp->page_priority = 1;
	l_temp->num_nodes = new_num;
	l_temp->next = child->next;
	l_temp->prev = child->page_no;
	if(child->next!=-1)
	{
		l_child = new leaf_node;
		l_child = read_leaf_page(child->next,val);
		l_child->prev = l_temp->page_no;
		write_leaf_page(l_child,val);
	}
	child->next = l_temp->page_no;
	j = old_num;
	for(i=0;i<new_num;i++,j++)
	{	
		(l_temp->key).push_back(child->key[j]);
		t1_rid.slotID = child->rid[j].slotID;
		t1_rid.dataPageID = child->rid[j].dataPageID;
		(l_temp->rid).push_back(t1_rid);
	}
	if(parent!=NULL)
		l_temp->parent_node = parent->page_no;
	else
		l_temp->parent_node = -1;
	
	write_leaf_page(l_temp,val);
	write_leaf_page(child,val);

	insert_internal(parent,child->key[old_num-1],(internal_node*)child,(internal_node*)l_temp,query);
	
}

template <class T>
void btree<T>::insert_leaf(leaf_node* l_temp,T num,internal_node* parent,RecordID rid,IndexQuery *query)
{
	int i,r=-1;
	int *pos = &r;
	T val;
	RecordID t1_rid,t2_rid;
	T temp1,temp2;
	if(l_temp->num_nodes==0)	
	{
		l_temp->num_nodes=1;
		(l_temp->key).push_back(num);
		(l_temp->rid).push_back(rid);
		write_leaf_page(l_temp,val);
	}
	else if(l_temp->num_nodes<FANOUT)
	{	
		l_temp = compare(l_temp,num,rid,pos);	
		temp2=num;
		t2_rid.slotID=rid.slotID;
		t2_rid.dataPageID=rid.dataPageID;

		(l_temp->key).push_back(num);
		(l_temp->rid).push_back(t2_rid);
		
		//l_temp->num_nodes++;
		for(i=*pos;i<=l_temp->num_nodes;i++)
		{
			temp1 = l_temp->key[i];
			l_temp->key[i] = temp2;				
			temp2=temp1;

			t1_rid.slotID=l_temp->rid[i].slotID;
			t1_rid.dataPageID=l_temp->rid[i].dataPageID;
			l_temp->rid[i].slotID=t2_rid.slotID;
			l_temp->rid[i].dataPageID=t2_rid.dataPageID;
			t2_rid.slotID=t1_rid.slotID;
			t2_rid.dataPageID=t1_rid.dataPageID;
			
		}
		l_temp->num_nodes++;
		write_leaf_page(l_temp,val);

	}
	else
	{
		l_temp = compare(l_temp,num,rid,pos);
		temp2=num;
		t2_rid.slotID=rid.slotID;
		t2_rid.dataPageID=rid.dataPageID;
		(l_temp->key).push_back(num);
		(l_temp->rid).push_back(t2_rid);
		
		for(i=*pos;i<=l_temp->num_nodes;i++)
		{
			temp1 = l_temp->key[i];
			l_temp->key[i] = temp2;				
			temp2=temp1;

			t1_rid.slotID=l_temp->rid[i].slotID;
			t1_rid.dataPageID=l_temp->rid[i].dataPageID;
			l_temp->rid[i].slotID=t2_rid.slotID;
			l_temp->rid[i].dataPageID=t2_rid.dataPageID;
			t2_rid.slotID=t1_rid.slotID;
			t2_rid.dataPageID=t1_rid.dataPageID;
		}
		write_leaf_page(l_temp,val);
		
		split_leaf(l_temp,parent,rid,query);
	}
}

template <class T>
IndexQuery* btree<T>::insert_key(T num,RecordID rid,IndexQuery *query)
{
	leaf_node *l_temp = NULL;
	T val;
	int page_priority;
	int flag,page_num;
	if(root->num_nodes==0)
	{
		flag = searching_insert(num,rid);
		if(flag==1)
		{
			query->errorFlag = true;
			return query;
		}
		
		page_num = root->ptrs[0];
		l_temp = read_leaf_page(page_num,val);
		
		if(l_temp!=NULL)
		{
			insert_leaf(l_temp,num,root,rid,query);
			write_leaf_page(l_temp,val);
			write_internal_page(root,val);
		}
		delete(l_temp);
	}
	else
	{			
		flag = searching_insert(num,rid);
		if(flag==1)
		{
			query->errorFlag = true;
			return query;
		}
		else
		{
			page_num = root->page_no;
			page_priority = root->page_priority;
			root = read_internal_page(page_num,page_priority,val); //reading root
			l_temp = insert_search_internal(root,num);
			internal_node * parent = NULL;

			if(l_temp->parent_node != root->page_no)
			page_priority = 2;
			else
			page_priority = 3;

			page_num = l_temp->parent_node;
			if(page_num!=-1)
			{
				parent = read_internal_page(page_num,page_priority,val);
			}
			else
			{
				parent = NULL;
			}
			insert_leaf(l_temp,num,parent,rid,query);
			delete(l_temp);
			delete(parent);
				
		}
	}
	return query;
}


template <class T>
IndexQuery* btree<T>::bulk_insert(T n1,T n2,T step_size,IndexQuery* query)
{
	T i = n1;
	RecordID tr;
	int count_bulk = 1;
	while(i<=n2)
	{
		tr.dataPageID = getFreePage(query->curDB);
		tr.slotID = count_bulk; 
		query = insert_key(i,tr,query);
		i = i+step_size;
		count_bulk++;
	}
	return query;
}

void indexInterface(IndexQuery *query)
{
	string data;
	IndexQuery *result;
	data = (string)query->dataType;
	if(data.compare("string")==0 || data.compare("std::string")==0 || data.compare("varchar")==0 || data.compare("char")==0)
	{
		btree<string> bt = btree<string>(query);
		result = bt.start(query);		
	}
	else if(data.compare("int")==0 || data.compare("integer")==0)
	{
		btree<int> bt1 = btree<int>(query);
		result = bt1.start(query);		
	}
	else if(data.compare("float")==0)
	{
		btree<float> bt2 = btree<float>(query);
		result = bt2.start(query);		
	}
	else if(data.compare("double")==0)
	{
		btree<double> bt4 = btree<double>(query);
		result = bt4.start(query);		
	}
	else if(data.compare("long")==0)
	{
		btree<long> bt5 = btree<long>(query);
		result = bt5.start(query);		
	}
	else if(data.compare("short")==0)
	{
		btree<short> bt6 = btree<short>(query);
		result = bt6.start(query);		
	}
	//return result;
	
}


int main()
{
	cout<<"\n Instantiated before";
	cout<<"\n Size: "<<sizeof(IndexQuery);
	IndexQuery *query = (IndexQuery*)malloc(sizeof(IndexQuery));	
	cout<<"\n Instantiated";
	query->queryType = 1;
	strcpy(query->dataType,"varchar");
	query->charLength = 20;
	query->rootPageID = -1;
	query->errorFlag = false;
	IndexQuery *result = NULL;  //(IndexQuery*)malloc(sizeof(IndexQuery));
	result = indexInterface(query);
	if(result!=NULL)
	{
		cout<<endl<<"Query->queryType:"<<result->queryType<<endl;
		cout<<endl<<"Query->dataType:"<<result->dataType<<endl;
		cout<<endl<<"Query->root:"<<result->rootPageID<<endl;
	}
}
