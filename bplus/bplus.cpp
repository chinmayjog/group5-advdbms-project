#include<iostream>
#include<cstring>
#include<string>
#include<string.h>
#include<math.h>
#include<vector>
#include<map>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include<fcntl.h>
#include<ctype.h>
#include<queue>
#include"../cachemgr/src/BufferManager.h"


#define page_size 2048
#define SIZEOFSTR 256
#define PAGEID_PTR (page_size-sizeof(int))
#define PAGEPRIORITY_PTR (PAGEID_PTR-sizeof(short))
#define NEXT_PTR (PAGEPRIORITY_PTR-sizeof(int))
#define LEVEL (NEXT_PTR-sizeof(int))
#define NUM_NODES (LEVEL-sizeof(int))
#define PAGE_NO (NUM_NODES-sizeof(int))
#define PARENT_NODE (PAGE_NO-sizeof(int))
#define PAGE_PRIORITY (PARENT_NODE-sizeof(int))
#define NEXT (PAGE_PRIORITY-sizeof(int))
#define PREV (NEXT-sizeof(int))

//#define FANOUT 3
#define PAGEMETADATA (sizeof(int) + sizeof(short) + sizeof(int))

using namespace std;

int FANOUT;
string data_type;
string primary;
int size_header;
int page_no;
int count_bulk=1;
//int fdID = 1;

struct RID{
	int page_no; 
	int slot_no;
};

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
		vector<struct RID> rid;//[FANOUT+1];
	}leaf;
	internal_node *root;	
	int found;
	string index_file;
	T step;
	btree();
	void start();
	leaf_node* read_leaf_page(int,T);
	internal_node* read_internal_page(int,int,T);
	void write_leaf_page(leaf_node*,T);
	void write_internal_page(internal_node*,T);
	//void write_internal_page(internal_node*,string);
	void delete_val();
	int deleting(T,struct RID);
	void delete_leaf(leaf_node*,T,internal_node*,struct RID);
	void combine_leaf(leaf_node*,internal_node*,T); 
	void combine_internal(internal_node*,internal_node*);
	void delete_dup(T);
	void bulk_delete(T,T,T);
	void insert_val();
	void insert_val(T,int slotID);
	void insert_key(T,struct RID);
	void insert_leaf(leaf_node*,T,internal_node*,struct RID);
	void insert_internal(internal_node*,T,internal_node*,internal_node*);
	void split_leaf(leaf_node*,internal_node*,struct RID);
	void split_internal(internal_node*,internal_node*);
	void bulk_insert(T,T,T);
	void search_val();
	leaf_node* search_internal(internal_node *,T);
	leaf_node* insert_search_internal(internal_node *,T);
	leaf_node* search_leaf(leaf_node *,T,struct RID,int*,int*);
	leaf_node* search_insert_leaf(leaf_node *,T,struct RID,int*,int*);
	leaf_node* search_bulk_leaf(leaf_node *, T ,int *,int *);
	leaf_node* searching_neighbor(T,struct RID,int*,int *);
	leaf_node* searching_node(T,struct RID,int*);
	leaf_node* search_for_neighbor(leaf_node *,T,struct RID,int*,int *);
	leaf_node* compare(leaf_node*,T,struct RID,int*);
	void traverse_val();
	int compare_data(T,T,string);
	int searching(T,struct RID);
	int searching_insert(T,struct RID);
	leaf_node* searching_key(T,int*,int*);
	void bulk_search(T,T,T);
	void search_dup(T);
	void traverse_step();
	void modify(T,struct RID);
	void open();
	int compute_fanout(int);
	int openfile(string);
	void create(string);
	void insert_file(string,T);
	void index_info();
	void drop(string);
	void commit(string);
	void close(string);	
};

template <class T>
btree<T>::btree()
{
	T val;
	root = new internal_node;
	root->key.resize(FANOUT+1);
	root->ptrs.resize(FANOUT+2);
	root->page_no = page_no++;
	leaf_node *l_temp = new leaf_node;
	l_temp->key.resize(FANOUT+1);
	l_temp->rid.resize(FANOUT+1);
	l_temp->level=0;
	l_temp->page_priority = 1;
	l_temp->num_nodes=0;
	l_temp->page_no = page_no++;
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

template<class T>
void btree<T>::start()
{
	int choice,num,i;
	T n,n1,s_dup;
	T val;
	string index,data,index_file,filename;
	struct RID rid_temp,rid;	
	while(1)
	{
		cout<<endl<<"                               ";
		cout<<endl<<"                               ";
		cout<<endl<<"Welcome to the Indexing module: ";	
		cout<<endl<<"	1.Create Index";
		cout<<endl<<"	2.Open Index";
		cout<<endl<<"	3.Insert from file";
		cout<<endl<<"	4.Insert individually";
		cout<<endl<<"	5.Bulk insert";
		cout<<endl<<"	6.Delete individually";
		cout<<endl<<"	7.Bulk delete";
		cout<<endl<<"	8.Delete Duplicates";
		cout<<endl<<"	9.Search individually";
		cout<<endl<<"	10.Bulk search";
		cout<<endl<<"	11.Search Duplicates";
		cout<<endl<<"	12.Traverse";
		cout<<endl<<"	13.Modify";	
		cout<<endl<<"	14.Drop index";
		cout<<endl<<"	15.Close index";
		cout<<endl<<"	16.Quit";
		cout<<endl;
		cin>>choice;
		if(choice==1)
		{
			char ans;
			cout<<"\n Selected Datatype: "<<data_type<<endl;
			cout<<"\n Want to enter FANOUT y/n?";
			cin>>ans;
			if(ans=='y')
			{
				cout<<"Enter FANOUT:";
				cin>>FANOUT;
			}
			else
			cout<<"\n Computed FANOUT: "<<FANOUT<<endl;
			cout<<endl<<"Is it a primary key y/n??";
			cin>>primary;
		}	
		else if(choice==2)
		{
			open();
		}
		else if(choice==3) 
		{	
			cout<<"\n Enter the filename: ";
			cin>>filename;
			insert_file(filename,val);
			//cout<<"File inserted....."<<endl;
		}
		else if(choice==4)
		{ 
			cout<<endl<<"How many values do you want to insert:";
			cin>>num;
			for(i=0;i<num;i++)
			{
				cout<<endl<<"Value: ";
				cin>>n;
				//cout<<endl<<"Enter rid page_no";
				//cin>>rid_temp.page_no;
				rid_temp.page_no = 999;
				cout<<endl<<"Enter rid slot_no";
				cin>>rid_temp.slot_no;
				//insert_val(n,rid_temp.slot_no);
				insert_key(n,rid_temp);
			}
		}
		else if(choice==5)
		{
			cout<<endl<<"Starting value:";
			cin>>n;
			cout<<endl<<"Ending value:";
			cin>>n1;
			cout<<endl<<"Step value:";
			cin>>step;
			bulk_insert(n,n1,step);
		}
		else if(choice==6)
		{
			cout<<endl<<"How many values do you want to delete:";
			cin>>num;
			for(i=0;i<num;i++)
			{
				cout<<endl<<"Value: ";
				cin>>n;
				cout<<endl<<"Enter rid page_no";
				cin>>rid_temp.page_no;
				//rid_temp.page_no = 1;
				cout<<endl<<"Enter rid slot_no";
				cin>>rid_temp.slot_no;
				int value = deleting(n,rid_temp);
				if(value==0)
				cout<<"Key Value doesnt exist";
			}
		}
		else if(choice==7)
		{
			cout<<endl<<"Starting value:";
			cin>>n;
			cout<<endl<<"Ending value:";
			cin>>n1;
			bulk_delete(n,n1,step);
		}
		else if(choice==8)
		{
			if(primary=="n")
			{
				cout<<"\n Enter the key whose duplicates are to be deleted:";
				cin>>s_dup;
				delete_dup(s_dup);
			}
			else
				cout<<endl<<"Index is on primary key... No duplicates exist"<<endl;
		}
		else if(choice==9)
		{
			cout<<endl<<"How many values do you want to search:";
			cin>>num;
			int flag;
			for(i=0;i<num;i++)
			{
				cout<<endl<<"Value: ";
				cin>>n;
				cout<<endl<<"Enter rid page_no";
				cin>>rid_temp.page_no;
				//rid_temp.page_no = 1;
				cout<<endl<<"Enter rid slot_no";
				cin>>rid_temp.slot_no;
				flag = searching(n,rid_temp);
				if(flag==0)
					cout<<"Key "<<n<<" with RID ["<<rid_temp.page_no<<","<<rid_temp.slot_no<<"] not present";
			}
		}
		else if(choice==10)
		{
			cout<<endl<<"Starting value:";
			cin>>n;
			cout<<endl<<"Ending value:";
			cin>>n1;
			bulk_search(n,n1,step);
		}
		else if(choice==11)
		{	
			if(primary=="n")
			{
				cout<<"\n Enter the key whose duplicate has to be searched:";
				cin>>s_dup;
				search_dup(s_dup);
			}
			else
				cout<<endl<<"Index is on primary key... No duplicates exist"<<endl;
		}
		else if(choice==12)
			traverse_val();
		else if(choice==13)
		{
			cout<<"\n Key value of the data to be updated:";
			cin>>val;
			cout<<"\n RID page_no:";
			cin>>rid.page_no;
			cout<<"\n RID slot_no:";
			cin>>rid.slot_no;
			modify(val,rid);
		}
		else if(choice==14)		
			drop(index);	
		else if(choice==15)		
			close(index);
		else if(choice==16)
		{
			return;
		}	
	}
}




template <class T>
int btree<T>::compute_fanout(int key_size)
{
	int fan;
	fan = (page_size - (7*sizeof(int) + PAGEMETADATA))/ (key_size + sizeof(int)+ sizeof(int)) ;
	return fan;
}

template <class T>
int btree<T>::openfile(string data)
{
	if(data.compare(index_file)==0)
	return 1;
	else 
	return 0;
}

template <class T>
typename btree<T>::leaf_node* btree<T>::compare(struct leaf_node *l_temp,T num,struct RID rid, int *pos)
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
					if(rid.page_no < l_temp->rid[i].page_no)
					{
						*pos = i;
						check = 1;
						break;	
					}
					else if(rid.page_no == l_temp->rid[i].page_no)
					{
						if(rid.slot_no <= l_temp->rid[i].slot_no)
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
void btree<T>::insert_file(string filename,T val)
{
	string full_line ="",line="",value="";
	int count = 0,s;
	//string[] li = "";
	T l;
	int col;
	struct RID tr;
	ifstream myReadFile;
 	myReadFile.open(filename.c_str());
 	//const char* output;
 	if (myReadFile.is_open())
	{
 		while (!myReadFile.eof())
		{
			getline(myReadFile,full_line);
			istringstream csv_stream(full_line);			
			col = 0;
			while( getline(csv_stream,value, ',') )
	                {
				if(col == 0)
				line = value;
				else if(col==1)
				{
					s = atoi(value.c_str());
					tr.page_no = s;
				}
				else if(col==2)
				{
					s = atoi(value.c_str());
					tr.slot_no = s;
				}
				col++;
            		}
			if(data_type.compare("int")==0)
			{
				s = atoi(line.c_str());
				l = s;
				//cout<<endl<<++i<<endl;
				if(tr.page_no!=-1)
				insert_key(l,tr);
				//cout<<endl<<l<<endl;
			}
			else if(data_type.compare("float")==0)
			{
				float ss = atof(line.c_str());
				l = ss;
				if(tr.page_no!=-1)
				insert_key(l,tr);
			}
			else if(data_type.compare("long")==0)
			{
				long ls = atol(line.c_str());
				l = ls;
				if(tr.page_no!=-1)
				insert_key(l,tr);
			}
			else if(data_type.compare("char")==0)
			{
				s = atoi(line.c_str());
				char c = (char)s;
				l = c;
				if(tr.page_no!=-1)
				insert_key(l,tr);
			}
			else if(data_type.compare("double")==0)
			{	
				double d = atof(line.c_str());		
				l = d;
				if(tr.page_no!=-1)
				insert_key(l,tr);
			}	
			tr.page_no = -1;
			tr.slot_no = -1;	
 		}
	}
	else
	cout<<endl<<"Cannot open file successfully.... Try again.."<<endl;
	myReadFile.close();
	//cout<<"Exit close"<<endl;	
}

template <>
void btree<string>::insert_file(string filename,string val)
{
	string full_line ="",line="",value="";
	int count = 0,s;
	//string[] li = "";
	string l;
	int col;
	struct RID tr;
	ifstream myReadFile;
 	myReadFile.open(filename.c_str());
 	//const char* output;
 	if (myReadFile.is_open())
	{
 		while (!myReadFile.eof())
		{
			getline(myReadFile,full_line);
			istringstream csv_stream(full_line);			
			col = 0;
			while( getline(csv_stream,value, ',') )
	                {
				if(col == 0)
				line = value;
				else if(col==1)
				{
					s = atoi(value.c_str());
					tr.page_no = s;
				}
				else if(col==2)
				{
					s = atoi(value.c_str());
					tr.slot_no = s;
				}
				col++;
            		}
			
			if(data_type.compare("string")==0 || data_type.compare("std::string")==0)
			{		
				//li = line;	
				l = string(line);
				if(tr.page_no!=-1)
				insert_key(l,tr);
			}
			tr.page_no = -1;
			tr.slot_no = -1;	
 		}
	}
	else
	cout<<endl<<"Cannot open file successfully.... Try again.."<<endl;
	myReadFile.close();
	//cout<<"Exit close"<<endl;	
}


template <class T>
void btree<T>::create(string data)
{
	
}

template <class T>
void btree<T>::open()
{
	ifstream myReadFile;
 	myReadFile.open("/data/index.txt");
	if (myReadFile.is_open())
	cout<<endl<<"Index File opened"<<endl;
	else
	cout<<endl<<"Could not open successfully"<<endl;	
}

template <class T>
void btree<T>::index_info()
{
	int page_num;
	T val;
	page_num = root->page_no;
	root = read_internal_page(page_num,root->page_priority,val);
	cout<<"\n Root node:"<<root->page_no;
	cout<<"\n Indexed datatype:"<<data_type;
	cout<<"\n Root page priority:"<<root->page_priority;
	cout<<"\n Total number of levels in the B+ tree structure:"<<root->level;
	cout<<"\n Total number of nodes in root node are:"<<root->num_nodes;
}

template <class T>
void btree<T>::drop(string)
{

}

template <class T>
void btree<T>::commit(string)
{

}

template <class T>
void btree<T>::close(string)
{

}

template <class T>
void btree<T>::search_dup(T num)
{
	int res,res1,count = 0;
	int *flag = &res;
	int *pos = &res1;
	int position = 0;
	T i,val;
	int check = 1;
	*flag = 0;
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
		cout<<"Key doesnt exists"<<endl;
		return;
	}
	if(count>1)
	{
		cout<<"Key "<<num<<" exists "<<count<<" times in the database";
	}
	else if(count==1)
	{
		cout<<"No duplicates exist for the key "<<num;
	}
}

template <class T>
void btree<T>::traverse_step()
{
	struct internal_node *i_temp = NULL;
	int page_num,pos,i;
	T val;
	page_num = root->page_no;
	root = read_internal_page(page_num,val);
	if(root!=NULL)
	{
		i_temp = root;
		while(i_temp->level > 1)
		{
			cout<<endl<<"Displaying contents of page_num:"<<i_temp->page_no<<endl;
			cout<<endl<<"Level of the node:"<<i_temp->level<<endl;
			cout<<endl<<"Number of nodes:"<<i_temp->num_nodes<<endl;
			cout<<endl<<"Parent node:"<<i_temp->parent_node<<endl;
			for(i=0;i<i_temp->num_nodes;i++)
			{
				cout<<endl<<"Value:"<<i_temp->key[i]<<" "<<endl;
			}
			 //apply queue concept
		}
	}
	else
	cout<<endl<<"No index on any column exists"<<endl;
}

template <class T>
void btree<T>::bulk_search(T n1,T n2,T step_size)
{
	//int count;
	struct RID tr;
	T i;
	T val;
	int res,res1;
	int page_num;
	//struct RID rid;
	//T num;
	int * flag = &res;
	*flag = 0;
	int *pos = &res1;
	*pos = 0;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	//cout<<"Before Searching"<<n1<<endl;
	l_temp = searching_key(n1,flag,pos);
	//cout<<"After Searching"<<n1<<endl;
	if(*flag ==1)
	{
		//l_temp = searching_node(n1,tr,pos);
		tr.slot_no = (l_temp->rid[*pos]).slot_no;
		tr.page_no = (l_temp->rid[*pos]).page_no;
		i = l_temp->key[*pos];
	}
	else if(*flag!=1)
	{
		//l_temp = searching_neighbor(n1,tr,pos);
		if(*pos <= (l_temp->num_nodes-1))
		{
			//*pos = *pos +1;
			tr.slot_no = (l_temp->rid[*pos]).slot_no;
			tr.page_no = (l_temp->rid[*pos]).page_no;
			i = l_temp->key[*pos];
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			*pos = 0;
			tr.slot_no = (l_temp->rid[*pos]).slot_no;
			tr.page_no = (l_temp->rid[*pos]).page_no;
			i = l_temp->key[*pos];
		}
	}
	while(i<=n2)
	{
		searching(i,tr);
		//read_leaf_page(l_temp);
		if(*pos < (l_temp->num_nodes-1))
		{
			*pos = *pos +1;
			i = l_temp->key[*pos];
			tr.page_no = (l_temp->rid[*pos]).page_no;
			tr.slot_no = (l_temp->rid[*pos]).slot_no; 
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			*pos = 0;
			i = l_temp->key[*pos];
			tr.page_no = (l_temp->rid[*pos]).page_no;
			tr.slot_no = (l_temp->rid[*pos]).slot_no; 
		}
		else
		{
			break;
		}
		
		//count++;
	}		
}


template <class T>
typename btree<T>::leaf_node* btree<T>::search_leaf(leaf_node *l_temp,T num,struct RID rid,int* flag,int *pos)
{
	int i,pos1,page_num;
	T val;
	leaf_node* temp_leaf = new leaf_node;	
	temp_leaf->key.resize(FANOUT+1);	
	temp_leaf->rid.resize(FANOUT+1);	
	temp_leaf = compare(l_temp,num,rid,pos);
	if(compare_data(l_temp->key[*pos],num,data_type)==0)
	{
		if(l_temp->rid[*pos].slot_no==rid.slot_no && l_temp->rid[*pos].page_no==rid.page_no)
		{
			*flag=1;
			cout<<endl<<"Value "<<l_temp->key[*pos]<<" found at level "<<l_temp->level<<" ,page_no "<<l_temp->page_no<<" and position "<<*pos;
		}
	}
	/*for(i=0;i<l_temp->num_nodes;i++)
	{
		if(compare_data(l_temp->key[i],num,data_type)==0)
		{
			
			if(l_temp->rid[i].slot_no==rid.slot_no && l_temp->rid[i].page_no==rid.page_no)
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
typename btree<T>::leaf_node* btree<T>::search_insert_leaf(leaf_node *l_temp,T num,struct RID rid,int* flag,int *pos)
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
				if(l_temp->rid[i].slot_no==rid.slot_no && l_temp->rid[i].page_no==rid.page_no)
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
		else if(compare_data(num,l_temp->key[i],data_type)<=0)
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
typename btree<T>::leaf_node* btree<T>::search_for_neighbor(leaf_node *l_temp,T num,struct RID rid,int* flag,int *pos)
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
int btree<T>::searching(T num,struct RID rid)
{
	int res,res1;
	int page_num;
	//struct RID rid;
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
int btree<T>::searching_insert(T num,struct RID rid)
{
	int res,res1;
	int page_num;
	//struct RID rid;
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
				cout<<endl<<"No values inserted yet";
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
typename btree<T>::leaf_node* btree<T>::searching_node(T num,struct RID rid,int *pos)
{
	int i,res,page_num;
	//struct RID rid;
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
typename btree<T>::leaf_node* btree<T>::searching_neighbor(T num,struct RID rid,int *flag,int *pos)
{
	int i,res,res1;
	//struct RID rid;
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

template <class T>
void btree<T>::search_val()
{
	T num;
	int flag;
	struct RID rid;
	cout<<endl<<"Enter the value to be searched:";
	cin>>num;
	cout<<endl<<"Enter the rid page no of the value";
	cin>>rid.page_no;
	cout<<endl<<"Enter the rid slot no of the value:";
	cin>>rid.slot_no;
	flag = searching(num,rid);
	if(flag==0)
		cout<<"Key "<<num<<" not present";
}

template <class T>
void btree<T>::traverse_val()
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
			cout<<endl<<"Level "<<l_temp->level<<" ;Leaf Node ;Node number : "<<++count<<" Page Number:"<<l_temp->page_no<<" Parent_node:"<<l_temp->parent_node<<" Prev: "<<l_temp->prev<<" Next:"<<l_temp->next<<endl;
			for(i=0;i<l_temp->num_nodes;i++)
			{
				cout<<"  "<<l_temp->key[i]<<"["<<l_temp->rid[i].page_no<<","<<l_temp->rid[i].slot_no<<"]";
			}
		}
		else
		{ 
			cout<<endl<<"Level "<<int_temp->level<<" ;Internal Node ;Node number : "<<++count<<" Page Number:"<<int_temp->page_no<<" Parent_node: "<<int_temp->parent_node<<endl;
			for(i=0;i<int_temp->num_nodes;i++)
				cout<<" "<<"["<<int_temp->ptrs[i]<<"]"<<" "<<int_temp->key[i];
				cout<<" "<<"["<<int_temp->ptrs[i]<<"]";
			for(i=0;i<=int_temp->num_nodes && int_temp->level>1;i++)
			{
				page_num = int_temp->ptrs[i];
				int_temp_1 = read_internal_page(page_num,2,val);
				level_order.push(int_temp_1);
				//cout<<endl<<"int_temp page_no:"<<int_temp->page_no<<endl;
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
void btree<T>::modify(T data,struct RID rid)
{
	int res,page_no,slot_no;
	T val,num;
	int *pos = &res;
	*pos = 0;
	struct RID tr;
	int flag = searching(data,rid);
	if(flag==0)
	cout<<"RID you are trying to modify doesnt exist ";
	else
	{
		string ans;
		struct leaf_node *l_temp = searching_node(data,rid,pos);
		if(primary=="n")
		{
			cout<<"\n Want to change key y/n ";
			cin>>ans;
			if(ans=="y")
			{
				cout<<"\n Enter new key ";
				cin>>num;
			}
		}		
		else
		num = data;
		cout<<"\n Want to change page_no y/n ";
		cin>>ans;
		if(ans=="y")
		{
			cout<<"\n Enter the new page no ";
			cin>>page_no;
			tr.page_no = page_no;
		}
		else
		tr.page_no = rid.page_no;
		cout<<"\n Want to change slot_no y/n ";
		cin>>ans;
		if(ans=="y")
		{
			cout<<"\n Enter the new slot no ";
			cin>>slot_no;
			tr.slot_no = slot_no;
		}
		else
		tr.slot_no = rid.slot_no;
		deleting(data,rid);
		insert_key(num,tr);
	}
		
}

/*char* get_free_page()
{
	char*buffer;
	buffer = new char[page_size];
	return buffer;
}*/

template<class T>
typename btree<T>::leaf_node* btree<T>::read_leaf_page(int page_no,T val)
{
	int level, num_nodes,parent_node,next,prev,page_priority;
	//int i,j;
	T num;
	char*buffer_page = new char[page_size];
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
	//cout<<endl<<"Leaf Page number to read:"<<page_no<<endl;
	(*bu).readDB(0,page_no,PagePriority(1),buffer_page);
	struct RID rid;
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
				memcpy(&rid,&buffer_page[j],sizeof(struct RID)); //how to transfer
				j = j + sizeof(struct RID);
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
				memcpy(&rid,&buffer_page[j],sizeof(struct RID)); 
				j = j + sizeof(struct RID);
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
			memcpy(&rid,&buffer_page[j],sizeof(struct RID)); //how to transfer
			j = j + sizeof(struct RID);
			(l_read->key).push_back(num); //value addition thru vector
			(l_read->rid).push_back(rid);*/
		}
		return l_read;
	}
	//cout<<endl<<"Problem is not here but somewhere else..."<<endl;
	return NULL;
}

template<>
typename btree<string>::leaf_node* btree<string>::read_leaf_page(int page_no, string val)
{
	int level, num_nodes,parent_node,next,prev,page_priority;
	//int i,j;
	string num;
	char*buffer_page = new char[page_size];
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
	//cout<<endl<<"Leaf Page number to read:"<<page_no<<endl;
	(*bu).readDB(0,page_no,PagePriority(1),buffer_page);
	struct RID rid;
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
				memcpy(&rid,&buffer_page[j],sizeof(struct RID)); //how to transfer
				j = j + sizeof(struct RID);
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
				memcpy(&rid,&buffer_page[j],sizeof(struct RID)); 
				j = j + sizeof(struct RID);
				(l_read->key).push_back(num1); 
				(l_read->rid).push_back(rid);
			}

			/*if(data_type!="string")
			memcpy(&num,&buffer_page[j],length);
			else
			{
				char str[256];
				strcpy(str,num.c_str());
				memcpy(&str,&buffer_page[j],length); 
			}
			j = j + length;
			memcpy(&rid,&buffer_page[j],sizeof(struct RID)); //how to transfer
			j = j + sizeof(struct RID);
			(l_read->key).push_back(num); //value addition thru vector
			(l_read->rid).push_back(rid);*/
		}
		return l_read;
	}
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
	//struct RID rid;
	char*buffer_page = new char[page_size];
	//cout<<endl<<"Allocated buffer for page number:"<<page_no<<endl;
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
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
		return int_read;	
	}
	return NULL;	
}

template<>
typename btree<string>::internal_node* btree<string>::read_internal_page(int page_no,int page_priority, string val)
{
	int level, num_nodes,parent_node,next,prev,ptrs;
	int i,j;
	string num;
	//cout<<endl<<"Internal Page number to read:"<<page_no<<endl;
	//struct RID rid;
	char*buffer_page = new char[page_size];
	//cout<<endl<<"Allocated buffer for page number:"<<page_no<<endl;
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */
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
		return int_read;	
	}
	return NULL;	
}


template <class T>
void btree<T>::write_leaf_page(struct leaf_node *l_read, T val)
{
	char *buffer_page;
	T num;
	struct RID rid;
	//struct leaf_node *temp =NULL;
	int level,num_nodes,page_no,parent_node,next,prev,page_priority;
	buffer_page = new char[page_size];
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */

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
			memcpy(&buffer_page[j],&rid,sizeof(struct RID)); //how to transfer
			j = j + sizeof(struct RID);
			//cout<<"\n Size of RID:"<<sizeof(struct RID);
		}
		(*bu).writeDB(0,page_no,PagePriority(page_priority),buffer_page);
		//return buffer_page; (Call chinmay's function)
	}
}

template <>
void btree<string>::write_leaf_page(struct leaf_node *l_read,string val)
{
	char *buffer_page;
	string num;
	struct RID rid;
	//struct leaf_node *temp =NULL;
	int level,num_nodes,page_no,parent_node,next,prev,page_priority;
	buffer_page = new char[page_size];
	BufferManager *bu = BufferManager::getBufferManager();/*call chinmay's function */

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
			memcpy(&buffer_page[j],&rid,sizeof(struct RID)); //how to transfer
			j = j + sizeof(struct RID);
			//cout<<"\n Size of RID:"<<sizeof(struct RID);
		}
		(*bu).writeDB(0,page_no,PagePriority(page_priority),buffer_page);
		//return buffer_page; (Call chinmay's function)
	}
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
	buffer_page = new char[page_size];
	if(i_read!=NULL && i_read->page_no!=0)
	{
		level = i_read->level;
		num_nodes = i_read->num_nodes;
		page_no = i_read->page_no;
		parent_node = i_read->parent_node;
		page_priority = i_read->page_priority;
		BufferManager *bu = BufferManager::getBufferManager();
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
		bool flag = (*bu).writeDB(0,page_no,PagePriority(page_priority),buffer_page);
		//return buffer_page; (Call chinmay's function)
	}
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
	buffer_page = new char[page_size];
	if(i_read!=NULL && i_read->page_no!=0)
	{
		level = i_read->level;
		num_nodes = i_read->num_nodes;
		page_no = i_read->page_no;
		parent_node = i_read->parent_node;
		page_priority = i_read->page_priority;
		BufferManager *bu = BufferManager::getBufferManager();
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
				j++;
				temp_int->num_nodes--;
				int_temp->num_nodes++;
			}
			
	
			i=0;		
			temp_int->num_nodes=0;
			for(;j<len;j++)
			{
				temp_int->key[i] = temp_int->key[j];
				temp_int->ptrs[i]=temp_int->ptrs[j];
				i++;
				temp_int->num_nodes++;
			}
			parent->key[0] = temp_int->key[0];
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
				j++;
				temp_int->num_nodes--;
				int_temp->num_nodes++;
			}
			int_temp->ptrs[j]= temp_int->ptrs[i];//check this and all similar
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
				j++;
				temp_int->num_nodes--;
				int_temp->num_nodes++;
			}
			
	
			i=0;		
			temp_int->num_nodes=0;
			for(;j<len;j++)
			{
				temp_int->key[i] = temp_int->key[j];
				temp_int->ptrs[i]=temp_int->ptrs[j];
				i++;
				temp_int->num_nodes++;
			}
			parent->key[pos] = temp_int->key[0];
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
					temp_node_leaf->parent_node = int_temp->page_no;
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
						temp_node = (internal_node*)temp_node_leaf;
					}
					else
						temp_node->parent_node = temp_int->page_no;
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
	struct RID rids[FANOUT/2];
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
			cout<<endl<<"Tree is empty";
		}
		else 
		cout<<endl<<"Deleted but just one node remaining in the tree";
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
			l_temp->rid[i].slot_no=ln_next->rid[j].slot_no;
			l_temp->rid[i].page_no=ln_next->rid[j].page_no;
			j++;
			ln_next->num_nodes--;
			l_temp->num_nodes++;
		}
		i=0;		
		ln_next->num_nodes=0;
		for(;j<len;j++)
		{
			ln_next->key[i] = ln_next->key[j];
			ln_next->rid[i].slot_no=ln_next->rid[j].slot_no;
			ln_next->rid[i].page_no=ln_next->rid[j].page_no;
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
			rids[i].slot_no=l_temp->rid[i].slot_no;
			rids[i].page_no=l_temp->rid[i].page_no;
		}
		len=i;
		k=ln_prev->num_nodes;	
		i=(ln_prev->num_nodes-(value-l_temp->num_nodes)); //excess nodes....!!!
		l_temp->num_nodes=0;
		j=0;
		for(;i<k;i++)
		{
			l_temp->key[j] = ln_prev->key[i];
			l_temp->rid[j].slot_no=ln_prev->rid[i].slot_no;
			l_temp->rid[j].page_no=ln_prev->rid[i].page_no;
			j++;
			ln_prev->num_nodes--;
			l_temp->num_nodes++;
		}
		for(i=0;i<len;i++)
		{
			l_temp->key[j] = keys[i];
			l_temp->rid[j].slot_no=rids[i].slot_no;
			l_temp->rid[j].page_no=rids[i].page_no;
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
			l_temp->rid[i].slot_no=ln_next->rid[j].slot_no;
			l_temp->rid[i].page_no=ln_next->rid[j].page_no;
			j++;
			ln_next->num_nodes--;
			l_temp->num_nodes++;
		}
		i=0;		
		ln_next->num_nodes=0;
		for(;j<len;j++)
		{
			ln_next->key[i] = ln_next->key[j];
			ln_next->rid[i].slot_no=ln_next->rid[j].slot_no;
			ln_next->rid[i].page_no=ln_next->rid[j].page_no;
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
				l_temp->rid[j].slot_no=ln_next->rid[i].slot_no;
				l_temp->rid[j].page_no=ln_next->rid[i].page_no;
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
				ln_prev->rid[j].slot_no=l_temp->rid[i].slot_no;
				ln_prev->rid[j].page_no=l_temp->rid[i].page_no;
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
void btree<T>::delete_leaf(leaf_node* l_temp,T num,internal_node* parent,struct RID rid)
{
	int i,pos;
	T val;
	for(i=0;i<l_temp->num_nodes;i++)
	{
		if((compare_data(l_temp->key[i],num,data_type)==0) && l_temp->rid[i].slot_no==rid.slot_no && l_temp->rid[i].page_no==rid.page_no)
		{
			pos=i;
			break;
		}
	}
	
	for(i=pos;i<l_temp->num_nodes-1;i++)
	{
		l_temp->key[i] = l_temp->key[i+1]; 
		l_temp->rid[i].slot_no=l_temp->rid[i+1].slot_no; 
		l_temp->rid[i].page_no=l_temp->rid[i+1].page_no; 
	}
	if(pos==0 && l_temp->num_nodes==1)
	{
		l_temp->key.erase (l_temp->key.begin());
		l_temp->rid.erase (l_temp->rid.begin());
	}
	
	l_temp->num_nodes--;
	cout<<endl<<"Deleted: key-"<<num<<" rid-"<<rid.page_no<<","<<rid.slot_no<<" at level 0";	
	write_leaf_page(l_temp,val);

	if(l_temp->num_nodes<(FANOUT/2)) 
	combine_leaf(l_temp,parent,num);	

}

template <class T>
int btree<T>::deleting(T num,struct RID rid)
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
			return 0;
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
	}
	return 1;
}


template <class T>
void btree<T>::delete_val()
{	
	T num;
	struct RID rid;
	cout<<endl<<"Please enter the data to be deleted: ";
	cin>>num;
	//rid.page_no = 1;
	cout<<endl<<"Please enter the rid page_no";
	cin>>rid.page_no;
	cout<<endl<<"Please enter the rid slot_no";
	cin>>rid.slot_no;
	deleting(num,rid);
}



template <class T>
void btree<T>::bulk_delete(T n1,T n2, T step_size)
{
	int res1,res;
	int page_num;
	T val;
	int *pos = &res1;
	int *flag = &res;
	T i = n1;
	struct RID tr;
	leaf_node *l_temp = new leaf_node;	
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	*pos = 0;
	*flag = 0;
	l_temp = searching_key(i,flag,pos);

	if(*flag ==1)
	{
		//l_temp = searching_node(n1,tr,pos);
		tr.slot_no = (l_temp->rid[*pos]).slot_no;
		tr.page_no = (l_temp->rid[*pos]).page_no;
		i = l_temp->key[*pos];
	}
	else if(*flag!=1)
	{
		//l_temp = searching_neighbor(n1,tr,pos);
		if(*pos <= (l_temp->num_nodes-1))
		{
			//*pos = *pos +1;
			tr.slot_no = (l_temp->rid[*pos]).slot_no;
			tr.page_no = (l_temp->rid[*pos]).page_no;
			i = l_temp->key[*pos];
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			*pos = 0;
			tr.slot_no = (l_temp->rid[*pos]).slot_no;
			tr.page_no = (l_temp->rid[*pos]).page_no;
			i = l_temp->key[*pos];
		}
	}
	while(i<=n2)
	{
		//l_temp = searching_key(i,flag,pos);
		deleting(i,tr);
		l_temp = searching_neighbor(i,tr,flag,pos);
		//page_num = l_temp->page_no;
		//l_temp = read_leaf_page(page_num,val);
		
		if(*pos <= (l_temp->num_nodes-1))
		{
			//*pos = *pos +1;
			i = l_temp->key[*pos];
			tr.page_no = (l_temp->rid[*pos]).page_no;
		tr.slot_no = (l_temp->rid[*pos]).slot_no; 
		}
		else if(l_temp->next!=-1)
		{
			page_num = l_temp->next;
			l_temp = read_leaf_page(page_num,val);
			*pos = 0;
			i = l_temp->key[*pos];
			tr.page_no = (l_temp->rid[*pos]).page_no;
		tr.slot_no = (l_temp->rid[*pos]).slot_no; 
		}
		else
		{
			break;
		}
		//count++;
	}		
}




template <class T>
void btree<T>::delete_dup(T num)
{	
	int res,res1,count = 0;
	int *flag = &res;
	int *pos = &res1;
	int position = 0;
	int page_num,page_num_curr;
	T i,val;
	int check = 1;
	struct RID tr;
	*flag = 0;
	*pos = 0;
	int not_found;
	leaf_node *l_temp = new leaf_node;
	leaf_node *l_next = new leaf_node;
	internal_node *parent = new internal_node;
	parent->key.resize(FANOUT+1);	
	parent->ptrs.resize(FANOUT+2);
	//leaf_node *leaf_arr;
	l_temp->key.resize(FANOUT+1);	
	l_temp->rid.resize(FANOUT+1);
	l_temp = searching_key(num,flag,pos);
	//cout<<endl<<"l_temp->page_no: "<<l_temp->page_no<<endl;
	if(*flag==1)
	{
		i = l_temp->key[*pos];
		tr.page_no = l_temp->rid[*pos].page_no;
		tr.slot_no = l_temp->rid[*pos].slot_no;
		while(i==num)
		{
			page_num_curr = l_temp->page_no;
			page_num = l_temp->next;
			deleting(num,tr);
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
				tr.page_no = l_temp->rid[*pos].page_no;
				tr.slot_no = l_temp->rid[*pos].slot_no;
			}
		}
	}
	else
	{
		cout<<"Key doesnt exists"<<endl;
		return;
	}
	if(count>1)
	{
		cout<<"Key "<<num<<" exists "<<count<<" times in the database";
	}
	else if(count==1)
	{
		cout<<"No duplicates exist for the key "<<num;
	}
}

template <class T>
void btree<T>::split_internal(internal_node* child,internal_node* parent)
{
	//cout<<"\n Inside Split Internal";
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
	int_temp->page_no = page_no++;
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
				//cout<<"\n Temp writing 2103";
			}
		}
		else
		{
			page_num = int_temp->ptrs[i];
			//cout<<"\n Temp writing 2109";
			temp_leaf = read_leaf_page(page_num,val);
			//cout<<"\n Temp reading 2110";
			if(temp_leaf!=NULL)
			{
				temp_leaf->parent_node = int_temp->page_no;
					//cout<<"\n Temp_leaf writing 2114";
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
			//cout<<"\n Temp writing 2109";
			temp_leaf = read_leaf_page(page_num,val);
			//cout<<"\n Temp reading 2110";
		}
	}	
			/*else
		{ 
			((leaf_node*)(int_temp->ptrs[j]))->parent_node = int_temp;
		}*/
	if(parent!=NULL)
	{
		int_temp->parent_node = parent->page_no;
	}
	else
	int_temp->parent_node = -1;

	write_internal_page(int_temp,val);

	write_internal_page(child,val);
	//cout<<"\n Inside split internal....before calling insert internal";
	insert_internal(parent,child->key[old_num-1],child,int_temp);
	//cout<<"Inside split internal... after insert internal";
	//	cout<<"\n Temp writing 2131";
}



template <class T>
void btree<T>::insert_internal(internal_node* node,T num,internal_node* child1,internal_node* child2)
{
	//cout<<"\n Insert internal in progress";
	int i,page_priority,r = -1;
	int *pos = &r;
	T temp1,temp2;
	T val;
	int temp1_child,temp2_child,page_num;

	if(node == NULL)
	{
		//cout<<"\n creating new internal node";
		node = new internal_node;
		//node1->key.resize(FANOUT+1);	
		//node1->ptrs.resize(FANOUT+2);	
		node->level=child1->level + 1;
		node->num_nodes=1;
		node->page_no = page_no++;
		node->page_priority = 2;
		(node->ptrs).push_back(child1->page_no);	
		(node->ptrs).push_back(child2->page_no);
		(node->key).push_back(num);
		//root->page_no = node1->page_no;
		page_num = root->page_no;
			//cout<<"\n Temp writing 2168";
		root = read_internal_page(page_num,3,val);
			//cout<<"\n Temp writing 2170";
		root->page_priority  = 2; //updating root
			//cout<<"\n Temp writing 2172";
		write_internal_page(root,val);
				//cout<<"\n Temp writing 2174";
	
		node->parent_node = -1;
		root = node;
		root->page_priority = 3;
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
		//cout<<"\n Inside insert_internal,,,, created internal_node";
		//	cout<<"\n Temp writing 2190";
		write_internal_page(node,val);
	}		
	else
	{
		//int *pos;
		//cout<<"\n Inside insert internal when node is not null";
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
		/*else if(node->level==0)	
		{
			node = (leaf_node*)node;
			node = compare(node,num,node->rid,pos);
			node = (internal_node*)node;
		}*/
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
			//cout<<"\n Inside insert internal....fanout is gone... before calling split_internal ";
			internal_node *parent = NULL;
			//parent->key.resize(FANOUT+1);	
			//parent->ptrs.resize(FANOUT+2);	
			if(node->level==0)
			{
					//cout<<"\n Temp writing 2229";
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
				
				split_internal(node,parent);
				//cout<<"Inside insert internal....after calling split_internal";
					//cout<<"\n Temp writing 2247";
				if(parent!=NULL)
				write_internal_page(parent,val);
					//cout<<"\n Temp writing 2249";
				write_leaf_page((leaf_node*)node,val);
			}
			else 
			{
					//cout<<"\n Temp writing 2238";
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

				split_internal(node,parent);

			}
		}
	}
}

template <class T>
void btree<T>::split_leaf(leaf_node* child,internal_node* parent,struct RID rid)
{
	//cout<<"\n Inside Split Leaf";
	int new_num,old_num,i,j;
	T val;
	leaf_node *l_temp = new leaf_node;
	leaf_node *l_child = NULL;
	struct RID t1_rid;
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
	l_temp->page_no = page_no++;
	l_temp->page_priority = 1;
	l_temp->num_nodes = new_num;
	//cout<<endl<<"Child: next = "<<child->next<<endl;
	//cout<<endl<<"Child: prev = "<<child->prev<<endl;
	l_temp->next = child->next;
	l_temp->prev = child->page_no;
	if(child->next!=-1)
	{
		l_child = new leaf_node;
		l_child = read_leaf_page(child->next,val);
		//cout<<"\n Temp writing 2303 \n";
		l_child->prev = l_temp->page_no;
		//cout<<endl<<"Writing l_child"<<endl;
		//	cout<<"\n Temp writing 2286";
		write_leaf_page(l_child,val);
	}
	child->next = l_temp->page_no;
	j = old_num;
	for(i=0;i<new_num;i++,j++)
	{	
		(l_temp->key).push_back(child->key[j]);
		t1_rid.slot_no = child->rid[j].slot_no;
		t1_rid.page_no = child->rid[j].page_no;
		(l_temp->rid).push_back(t1_rid);
	}
	if(parent!=NULL)
		l_temp->parent_node = parent->page_no;
	else
		l_temp->parent_node = -1;
	
	write_leaf_page(l_temp,val);
	write_leaf_page(child,val);

	//cout<<"\n Inside split leaf before calling insert internal";
	insert_internal(parent,child->key[old_num-1],(internal_node*)child,(internal_node*)l_temp);
	
}

template <class T>
void btree<T>::insert_leaf(leaf_node* l_temp,T num,internal_node* parent,struct RID rid)
{
	int i,r=-1;
	int *pos = &r;
	T val;
	//const char *c,*d;
	struct RID t1_rid,t2_rid;
	T temp1,temp2;
	//l_temp->key.resize(FANOUT+1);	
	//l_temp->rid.resize(FANOUT+2);
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
		/*pos=l_temp->num_nodes;	
		for(i=0;i<l_temp->num_nodes;i++)
		{
			//cout<<i<<endl;
			if(data_type.compare("string")==0)
			{
				c = num.c_str();
				cout<<c;
				d = (l_temp->key[i]).c_str();
				cout<<d;
				if(strcmp(c,d)<0)
				{
					pos = i;
					break;
				}	
				strcmp(c,d);			
			}
			if(compare_data(num,l_temp->key[i],data_type)<0)	
			{
				pos=i;
				break;
			}				
		}*/
		//cout<<"inside insert leaf, done compare"<<endl;
		temp2=num;
		t2_rid.slot_no=rid.slot_no;
		t2_rid.page_no=rid.page_no;

		(l_temp->key).push_back(num);
		(l_temp->rid).push_back(t2_rid);
		
		//l_temp->num_nodes++;
		for(i=*pos;i<=l_temp->num_nodes;i++)
		{
			//cout<<"inside insert leaf for, i: "<<i<<endl;
			temp1 = l_temp->key[i];
			l_temp->key[i] = temp2;				
			temp2=temp1;

			t1_rid.slot_no=l_temp->rid[i].slot_no;
			t1_rid.page_no=l_temp->rid[i].page_no;
			l_temp->rid[i].slot_no=t2_rid.slot_no;
			l_temp->rid[i].page_no=t2_rid.page_no;
			t2_rid.slot_no=t1_rid.slot_no;
			t2_rid.page_no=t1_rid.page_no;
			
		}
		//cout<<"inside insert leaf, done for"<<endl;
		//cout<<"Inserted in the leaf node";
		l_temp->num_nodes++;
		write_leaf_page(l_temp,val);

	}
	else
	{
		//int val;
		//cout<<"\n Split Leaf_node in progress...";
		/*pos=l_temp->num_nodes;	
		for(i=0;i<l_temp->num_nodes;i++)
		{
			val = compare_data(num,l_temp->key[i],data_type);
			if(val<=0)	
			{
				pos=i;
				break;
			}				
		}*/
		l_temp = compare(l_temp,num,rid,pos);
		temp2=num;
		t2_rid.slot_no=rid.slot_no;
		t2_rid.page_no=rid.page_no;
		(l_temp->key).push_back(num);
		(l_temp->rid).push_back(t2_rid);
		
		for(i=*pos;i<=l_temp->num_nodes;i++)
		{
			//cout<<"inside insert leaf for, i: "<<i<<endl;
			temp1 = l_temp->key[i];
			l_temp->key[i] = temp2;				
			temp2=temp1;

			t1_rid.slot_no=l_temp->rid[i].slot_no;
			t1_rid.page_no=l_temp->rid[i].page_no;
			l_temp->rid[i].slot_no=t2_rid.slot_no;
			l_temp->rid[i].page_no=t2_rid.page_no;
			t2_rid.slot_no=t1_rid.slot_no;
			t2_rid.page_no=t1_rid.page_no;
		}
		write_leaf_page(l_temp,val);
		
		split_leaf(l_temp,parent,rid);
		//cout<<"Splitted leaf node and inserted";
	}
}

template <class T>
void btree<T>::insert_key(T num,struct RID rid)
{
	leaf_node *l_temp = NULL;
	T val;
	int page_priority;
	int flag,page_num;
	//struct RID rid;
	//l_temp->num_nodes = 0;
	if(root->num_nodes==0)
	{
		flag = searching_insert(num,rid);
		if(flag==1)
		{
			cout<<"\n Error: Data already exists with same RID. Cannot insert this value "<<endl;
			return;
		}
		
		page_num = root->ptrs[0];
		//cout<<endl<<"Page no:"<<page_num;
		l_temp = read_leaf_page(page_num,val);
		
		//rid.page_no = lpage_no;
		//rid.slot_no = slotID; 
		if(l_temp!=NULL)
		{
			insert_leaf(l_temp,num,root,rid);
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
			cout<<"\n Error: Data already exists with same RID. Cannot insert this value..."<<endl;
			return;
		}
		else
		{
			page_num = root->page_no;
			page_priority = root->page_priority;
			//cout<<"\n Temp writing 2457";
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
			insert_leaf(l_temp,num,parent,rid);
			
			//cout<<"\n Leaf page no written:"<<l_temp->page_no<<endl;
			//cout<<"\n Temp writing 2478";
			//if(parent!=NULL)
			//write_internal_page(parent);
			delete(parent);
				
		}
	}
	//delete(l_temp);
	//delete(parent);
}

template <class T>
void btree<T>::insert_val()
{
	struct RID rid;
	T num;
	cout<<endl<<"Enter value to be inserted: ";
	cin>>num;

	rid.page_no = 999;
	cout<<endl<<"Enter slot_no: ";
	cin>>rid.slot_no;
//for(int i=0;i<19;i++){
	//cout<<endl<<"insert_key "<<nums[i]<<" with rid "<<rids[i];
	//num = nums[i];
	//rid = rids[i];
	insert_key(num,rid);
//}
}

template <class T>
void btree<T>::insert_val(T num,int slotID)
{
	struct RID rid;

	rid.page_no = 999;
	rid.slot_no = slotID;
	insert_key(num,rid);
//}
}

template <class T>
void btree<T>::bulk_insert(T n1,T n2,T step_size)
{
	T i = n1;
	struct RID tr;
	while(i<=n2)
	{
		tr.page_no = 999;
		tr.slot_no = count_bulk; 
		insert_key(i,tr);
		i = i+step_size;
		count_bulk++;
	}
}

int main()
{
	string filepath = "../testDB/";
	BufferManager *bu = BufferManager::getBufferManager();
	(*bu).setPageSize(page_size);
	(*bu).initializeCache(1000);
	(*bu).createDB("../testDB/","index.txt",2048000);
	int flag = (*bu).openDB("../testDB/","index.txt");
	cout<<"Flag:"<<flag<<endl<<"PageCount:"<<(*bu).getPageCount(flag)<<endl;
	string data;
	cout<<endl<<"Data type to be indexed:";
	cin>>data;
	int key_size;
	page_no = 1;
	//cout<<"Size of int:"<<sizeof(int)<<endl;
	//cout<<"Size of short:"<<sizeof(short)<<endl;
	//cout<<"Size of long:"<<sizeof(long)<<endl;
	if(data.compare("string")==0 || data.compare("std::string")==0)
	{
		btree<string> bt = btree<string>();
		data_type = data;
		key_size = SIZEOFSTR + sizeof(int) + sizeof(int);
		FANOUT = bt.compute_fanout(key_size);
		bt.start();		
	}
	else if(data.compare("int")==0 || data.compare("integer")==0)
	{
		btree<int> bt1 = btree<int>();
		data_type = data;
		key_size = sizeof(int) + sizeof(int) + sizeof(int);
		FANOUT = bt1.compute_fanout(key_size);
		bt1.start();		
	}
	else if(data.compare("float")==0)
	{
		btree<float> bt2 = btree<float>();
		data_type = data;
		key_size = sizeof(float) + sizeof(int) + sizeof(int);
		FANOUT = bt2.compute_fanout(key_size);
		bt2.start();		
	}
	else if(data.compare("char")==0 || data.compare("character")==0)
	{
		btree<char> bt3 = btree<char>();
		data_type = data;
		key_size = sizeof(char) + sizeof(int) + sizeof(int);
		FANOUT = bt3.compute_fanout(key_size);
		bt3.start();		
	}
	else if(data.compare("double")==0)
	{
		btree<double> bt4 = btree<double>();
		data_type = data;
		key_size = sizeof(double) + sizeof(int) + sizeof(int);
		FANOUT = bt4.compute_fanout(key_size);
		bt4.start();		
	}
	else if(data.compare("long")==0)
	{
		btree<long> bt5 = btree<long>();
		data_type = data;
		key_size = sizeof(long) + sizeof(int) + sizeof(int);
		FANOUT = bt5.compute_fanout(key_size);
		bt5.start();		
	}
	else if(data.compare("short")==0)
	{
		btree<short> bt6 = btree<short>();
		data_type = data;
		key_size = sizeof(short) + sizeof(int) + sizeof(int);
		FANOUT = bt6.compute_fanout(key_size);
		bt6.start();		
	}
	(*bu).closeDB(0);
	BufferManager::releaseBuffer();
}
