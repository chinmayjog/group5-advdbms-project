#include "BufferManager.h"
#include<limits>
void process_choice(int);
int main(){
	int choice;
	do{
			cin.clear();
			cout<<"\n\n\t\t\tBuffer Manager Interface\n";
			cout<<"\n\t Welcome to Buffer Manager!";
			cout<<"\n\t 1. Initialize Cache";
			cout<<"\n\t 2. Create Database"; 
			cout<<"\n\t 3. Open Database"; 
			cout<<"\n\t 4. Read Database Page"; 
			cout<<"\n\t 5. Write Database Page"; 
			cout<<"\n\t 6. Expand Database File"; 
			cout<<"\n\t 7. Commit Database";
			cout<<"\n\t 8. Close Database";
			cout<<"\n\t 9. Commit Cache";
			cout<<"\n\t10. Release Cache";
			cout<<"\n\t11. Print Hex Dump"; 
			cout<<"\n\t12. Show Cache Page Header";
			cout<<"\n\t13. Show Hit Rate";
			cout<<"\n\t14. Exit Buffer Manager"; 
			cout<<"\n\t Choice [1:14]: ";
			while(!(cin>>choice) || choice > 14 || choice < 1){
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout<<endl<<"\tInvalid Choice. Try Again: ";
			};
			if(choice > 14)
				continue;
			else
				process_choice(choice);
	}while(choice!=14);
	return 0;
}
