%{
#include <iostream>
#include <cstring>
#include "parser.h"
#include "flexparser.h"
using namespace std;

query q;
condition *a[2];
condition *stack[100];
int top = -1;
int count = 0;
int colcount = 0;
//-- Lexer prototype required by bison, aka getNextToken()
int yylex(); 
int yyerror(const char *p) { cout<<p<< endl; q->error = 1;}
%}

%union{
	char id[100];
    int integer;
    float flt;
    char datetm[28];
    
};

%token SELECT INSERT CREATE DELETE UPDATE USE DROP SHOW ADD MODIFY ALTER PRIMARY KEY AUTOINCREMENT DEFAULT
%token <id> ID
%token DATABASE DATABASES TABLE TABLES DISTINCT FROM WHERE INTO VALUES INDEX ON COLUMN
%token LE LT GE GT EQ NE OR AND LIKE GROUP HAVING ORDER ASC DESC IN END QUOTE LIMIT SET
%token CHAR VARCHAR 
%token <datetm>DATETIME
%token <integer> INTNUM
%token <flt> DBLNUM
%right  LPAREN RPAREN  
%left AND OR '+' '/' '*' '-'
%left LT GT LE GE EQ NE

%%

	S:	  		 STA1 END {return 0; }
				|STA2 END {return 0; }
				|STA3 END {return 0; } 
				|STA4 END {return 0; }
				|STA5 END {return 0; }
				;

	STA1:		 USE DATABASE ID {strcpy(q->type, "USEDB"); strcpy(q->dbname, $3);} 
				|CREATE DATABASE ID {strcpy(q->type, "CREATEDB"); strcpy(q->dbname, $3);} 	
				|DROP DATABASE ID {strcpy(q->type, "DROPDB"); strcpy(q->dbname, $3);} 	
				|SHOW DATABASES {strcpy(q->type, "SHOWDB");}
				|SHOW TABLES {strcpy(q->type, "SHOWTBL");}
				|CREATE TABLE ID LPAREN columnlist RPAREN {strcpy(q->type, "CREATETBL"); strcpy(q->table ,$3);}
				|CREATE INDEX ID ON ID LPAREN indexlist RPAREN {strcpy(q->type, "CREATEIND"); strcpy(q->table ,$5); strcpy(q->index ,$3);}
				|DROP INDEX ID {strcpy(q->type, "DROPIND"); strcpy(q->index, $3);} 	
				|ALTER TABLE ID ADD COLUMN ID ID {
													strcpy(q->type, "ALTERADD"); 
													strcpy(q->table, $3); 
													strcpy((q->columns[q->cntColumns]).name, $6); 
													strcpy((q->columns[q->cntColumns]).type, $7);
												  } 	
				|ALTER TABLE ID ADD COLUMN ID VARCHAR LPAREN INTNUM RPAREN {
													strcpy(q->type, "ALTERADD"); 
													strcpy(q->table, $3); 
													strcpy((q->columns[q->cntColumns]).name, $6); 
													strcpy((q->columns[q->cntColumns]).type, "varchar");
													(q->columns[q->cntColumns]).sizeofField1 = $9;
												  } 	
				|ALTER TABLE ID ADD COLUMN ID CHAR LPAREN INTNUM RPAREN {
													strcpy(q->type, "ALTERADD"); 
													strcpy(q->table, $3); 
													strcpy((q->columns[q->cntColumns]).name, $6); 
													strcpy((q->columns[q->cntColumns]).type, "char");
													(q->columns[q->cntColumns]).sizeofField1 = $9;
												  } 	
				|ALTER TABLE ID DROP COLUMN ID {
													strcpy(q->type, "ALTERDROP"); 
													strcpy(q->table, $3); 
													strcpy((q->columns[q->cntColumns]).name, $6); 
												  } 	
				|ALTER TABLE ID MODIFY COLUMN ID ID {
													strcpy(q->type, "ALTERMODIFY"); 
													strcpy(q->table, $3); 
													strcpy((q->columns[q->cntColumns]).name, $6); 
													strcpy((q->columns[q->cntColumns]).type, $7);
												  } 	
				|ALTER TABLE ID MODIFY COLUMN ID VARCHAR LPAREN INTNUM RPAREN{
													strcpy(q->type, "ALTERMODIFY"); 
													strcpy(q->table, $3); 
													strcpy((q->columns[q->cntColumns]).name, $6); 
													strcpy((q->columns[q->cntColumns]).type, "varchar");
													(q->columns[q->cntColumns]).sizeofField1 = $9;
												  } 	
				|ALTER TABLE ID MODIFY COLUMN ID CHAR LPAREN INTNUM RPAREN{
													strcpy(q->type, "ALTERMODIFY"); 
													strcpy(q->table, $3); 
													strcpy((q->columns[q->cntColumns]).name, $6); 
													strcpy((q->columns[q->cntColumns]).type, "char");
													(q->columns[q->cntColumns]).sizeofField1 = $9;
												  } 	
				;
				
	indexlist:  ID',' indexlist {strcpy((q->columns[q->cntColumns++]).name, $1); }
				|ID {strcpy((q->columns[q->cntColumns++]).name, $1); }
				;
	columnlist:	ID ID',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										q->cntColumns = q->cntColumns + 1;
									}
				| ID ID PRIMARY KEY',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										(q->columns[q->cntColumns]).isPrimary = 1;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID ID AUTOINCREMENT',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										(q->columns[q->cntColumns]).isAutoIncrement = 1;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID ID PRIMARY KEY AUTOINCREMENT',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										(q->columns[q->cntColumns]).isPrimary = 1;
										(q->columns[q->cntColumns]).isAutoIncrement = 1;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID ID DEFAULT INTNUM',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										if(strcmp((q->columns[q->cntColumns]).type, "short") == 0){
											(q->columns[q->cntColumns]).defaultshort = $4;
										}
										if(strcmp((q->columns[q->cntColumns]).type, "int") == 0){
											(q->columns[q->cntColumns]).defaultint = $4;
										}if(strcmp((q->columns[q->cntColumns]).type, "long") == 0){
											(q->columns[q->cntColumns]).defaultlong = $4;
										}
										(q->columns[q->cntColumns]).isPrimary = 0;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID ID DEFAULT DBLNUM',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										if(strcmp((q->columns[q->cntColumns]).type, "float") == 0){
											(q->columns[q->cntColumns]).defaultfloat = $4;
										}if(strcmp((q->columns[q->cntColumns]).type, "double") == 0){
											(q->columns[q->cntColumns]).defaultdouble = $4;
										}
										(q->columns[q->cntColumns]).isPrimary = 0;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID VARCHAR LPAREN INTNUM RPAREN',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "varchar"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										q->cntColumns = q->cntColumns + 1;
									}
				| ID VARCHAR LPAREN INTNUM RPAREN PRIMARY KEY',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "varchar"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										(q->columns[q->cntColumns]).isPrimary = 1;
										q->cntColumns = q->cntColumns + 1;
									}
				| ID VARCHAR LPAREN INTNUM RPAREN DEFAULT ID',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "varchar"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										strcpy((q->columns[q->cntColumns]).defaultstring, $7);
										q->cntColumns = q->cntColumns + 1;
									}
				| ID CHAR LPAREN INTNUM RPAREN',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "char"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										q->cntColumns = q->cntColumns + 1;
									}
				| ID CHAR LPAREN INTNUM RPAREN PRIMARY KEY',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "char"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										(q->columns[q->cntColumns]).isPrimary = 1;
										q->cntColumns = q->cntColumns + 1;
									}
				| ID CHAR LPAREN INTNUM RPAREN DEFAULT ID',' columnlist {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "char"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										strcpy((q->columns[q->cntColumns]).defaultstring, $7);
										q->cntColumns = q->cntColumns + 1;
									}
				| ID ID {
							strcpy((q->columns[q->cntColumns]).name, $1); 
							strcpy((q->columns[q->cntColumns]).type, $2); 
							q->cntColumns = q->cntColumns + 1;
						}
				| ID ID PRIMARY KEY{
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										(q->columns[q->cntColumns]).isPrimary = 1;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID ID AUTOINCREMENT{
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										(q->columns[q->cntColumns]).isAutoIncrement = 1;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID ID PRIMARY KEY AUTOINCREMENT {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										(q->columns[q->cntColumns]).isPrimary = 1;
										(q->columns[q->cntColumns]).isAutoIncrement = 1;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID ID DEFAULT INTNUM{
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										if(strcmp((q->columns[q->cntColumns]).type, "short") == 0){
											(q->columns[q->cntColumns]).defaultshort = $4;
										}
										if(strcmp((q->columns[q->cntColumns]).type, "int") == 0){
											(q->columns[q->cntColumns]).defaultint = $4;
										}if(strcmp((q->columns[q->cntColumns]).type, "long") == 0){
											(q->columns[q->cntColumns]).defaultlong = $4;
										}	
										(q->columns[q->cntColumns]).isPrimary = 0;
										q->cntColumns = q->cntColumns + 1;
									}
				| ID ID DEFAULT DBLNUM{
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, $2); 
										if(strcmp((q->columns[q->cntColumns]).type, "float") == 0){
											(q->columns[q->cntColumns]).defaultfloat = $4;
										}if(strcmp((q->columns[q->cntColumns]).type, "double") == 0){
											(q->columns[q->cntColumns]).defaultdouble = $4;
										}
										(q->columns[q->cntColumns]).isPrimary = 0;
										q->cntColumns = q->cntColumns + 1;
										
									}
				| ID VARCHAR LPAREN INTNUM RPAREN {				
							strcpy((q->columns[q->cntColumns]).name, $1); 
							strcpy((q->columns[q->cntColumns]).type, "varchar"); 
							(q->columns[q->cntColumns]).sizeofField1 = $4;
							q->cntColumns = q->cntColumns + 1;
						}
				| ID VARCHAR LPAREN INTNUM RPAREN PRIMARY KEY {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "varchar"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										(q->columns[q->cntColumns]).isPrimary = 1;
										q->cntColumns = q->cntColumns + 1;
									}
				| ID VARCHAR LPAREN INTNUM RPAREN DEFAULT ID {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "varchar"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										strcpy((q->columns[q->cntColumns]).defaultstring, $7);
										q->cntColumns = q->cntColumns + 1;
									}
				| ID CHAR LPAREN INTNUM RPAREN {
							strcpy((q->columns[q->cntColumns]).name, $1); 
							strcpy((q->columns[q->cntColumns]).type, "char"); 
							(q->columns[q->cntColumns]).sizeofField1 = $4;
							q->cntColumns = q->cntColumns + 1;
						}
				| ID CHAR LPAREN INTNUM RPAREN PRIMARY KEY {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "char"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										(q->columns[q->cntColumns]).isPrimary = 1;
										q->cntColumns = q->cntColumns + 1;
									}
				| ID CHAR LPAREN INTNUM RPAREN DEFAULT ID {
										strcpy((q->columns[q->cntColumns]).name, $1); 
										strcpy((q->columns[q->cntColumns]).type, "char"); 
										(q->columns[q->cntColumns]).sizeofField1 = $4;
										strcpy((q->columns[q->cntColumns]).defaultstring, $7);
										q->cntColumns = q->cntColumns + 1;
									}
				;
				
	STA2:		INSERT INTO ID LPAREN columnorder RPAREN VALUES LPAREN values RPAREN {strcpy(q->type, "INSERT1"); strcpy(q->table ,$3);}
				| INSERT INTO ID VALUES LPAREN values RPAREN {strcpy(q->type, "INSERT2"); strcpy(q->table ,$3);}
				;
	
	columnorder:ID',' columnorder {strcpy(q->ins[count++].colname, $1);}
				| ID {strcpy(q->ins[count++].colname, $1);}
				;
	values:		QUOTE ID QUOTE',' values {strcpy(q->ins[q->cntColumns].str, $2); strcpy(q->ins[q->cntColumns].type, "string"); q->cntColumns = q->cntColumns + 1;}
				| INTNUM',' values {q->ins[q->cntColumns].i = $1; strcpy(q->ins[q->cntColumns].type, "integer"); q->cntColumns = q->cntColumns + 1;}
				| DBLNUM',' values {q->ins[q->cntColumns].f = $1; strcpy(q->ins[q->cntColumns].type, "float"); q->cntColumns = q->cntColumns + 1;}
				| DATETIME',' values {q->ins[q->cntColumns].d = getDate($1); strcpy(q->ins[q->cntColumns].type, "datetime"); q->cntColumns = q->cntColumns + 1;}
				|QUOTE ID QUOTE {strcpy(q->ins[q->cntColumns].str, $2); strcpy(q->ins[q->cntColumns].type, "string"); q->cntColumns = q->cntColumns + 1;}
				| INTNUM {q->ins[q->cntColumns].i = $1; strcpy(q->ins[q->cntColumns].type, "integer"); q->cntColumns = q->cntColumns + 1;}
 				| DBLNUM {q->ins[q->cntColumns].f = $1; strcpy(q->ins[q->cntColumns].type, "float"); q->cntColumns = q->cntColumns + 1;}
 				| DATETIME {q->ins[q->cntColumns].d = getDate($1); strcpy(q->ins[q->cntColumns].type, "datetime"); q->cntColumns = q->cntColumns + 1;}
				;
	
	STA3:		DELETE FROM ID {strcpy(q->type, "DELETE"); strcpy(q->table ,$3);}
				| DELETE FROM ID WHERE COND {strcpy(q->type, "DELETE"); strcpy(q->table ,$3);q->root = pop(); count=0;};
				;
				
	STA4:		UPDATE ID SET updateList {strcpy(q->type, "UPDATE"); strcpy(q->table ,$2);}
				|UPDATE ID SET updateList WHERE COND {strcpy(q->type, "UPDATE"); strcpy(q->table ,$2); q->root = pop(); count=0;};
				;
				
	updateList : ID EQ QUOTE ID QUOTE',' updateList {strcpy(q->ins[q->cntColumns].colname, $1); strcpy(q->ins[q->cntColumns].str, $4); strcpy(q->ins[q->cntColumns].type, "string"); q->cntColumns = q->cntColumns + 1;}
				|ID EQ INTNUM',' updateList {strcpy(q->ins[q->cntColumns].colname, $1); q->ins[q->cntColumns].i = $3; strcpy(q->ins[q->cntColumns].type, "integer"); q->cntColumns = q->cntColumns + 1;}
				|ID EQ DBLNUM',' updateList {strcpy(q->ins[q->cntColumns].colname, $1); q->ins[q->cntColumns].f = $3; strcpy(q->ins[q->cntColumns].type, "float"); q->cntColumns = q->cntColumns + 1;}
				|ID EQ DATETIME',' updateList {strcpy(q->ins[q->cntColumns].colname, $1); q->ins[q->cntColumns].d = getDate($3); strcpy(q->ins[q->cntColumns].type, "datetime"); q->cntColumns = q->cntColumns + 1;}
				|ID EQ QUOTE ID QUOTE {strcpy(q->ins[q->cntColumns].colname, $1); strcpy(q->ins[q->cntColumns].str, $4); strcpy(q->ins[q->cntColumns].type, "string"); q->cntColumns = q->cntColumns + 1;}
				|ID EQ INTNUM {strcpy(q->ins[q->cntColumns].colname, $1); q->ins[q->cntColumns].i = $3; strcpy(q->ins[q->cntColumns].type, "integer"); q->cntColumns = q->cntColumns + 1;}
				|ID EQ DBLNUM {strcpy(q->ins[q->cntColumns].colname, $1); q->ins[q->cntColumns].f = $3; strcpy(q->ins[q->cntColumns].type, "float"); q->cntColumns = q->cntColumns + 1;}
				|ID EQ DATETIME {strcpy(q->ins[q->cntColumns].colname, $1); q->ins[q->cntColumns].d = getDate($3); strcpy(q->ins[q->cntColumns].type, "datetime"); q->cntColumns = q->cntColumns + 1;}
				;

	STA5: 		SELECT attributeList FROM ID ST2 {strcpy(q->type ,"SELECT"); strcpy(q->table ,$4);}
				|SELECT DISTINCT attributeList FROM ID ST2 {strcpy(q->type, "DISTINCT"); strcpy(q->table ,$5);}
				;
    ST2: 		WHERE COND ST7 {q->root = pop(); count=0;}
				|ST7
				;
    /*ST3:		GROUP attributeList ST4
				|ST4
				;
    ST4:		HAVING COND ST5
				|ST5
				;
    ST5:		ORDER attributeList ST6 ST7
				| ST7
				;
    ST6:		DESC
				|ASC 
				| 
				; */
	ST7:		LIMIT INTNUM {q->lowerLimit = 0; q->countResults = $2;}
				| LIMIT INTNUM','INTNUM {q->lowerLimit = $2; q->countResults = $4;}
				|
				;
				
  attributeList : ID','attributeList {strcpy((q->columns[q->cntColumns]).name, $1); q->cntColumns = q->cntColumns + 1;}
				|  ID LPAREN ID RPAREN','attributeList {
									strcpy((q->columns[q->cntColumns]).function, $1); 
									strcpy((q->columns[q->cntColumns]).name, $3); 
									q->cntColumns = q->cntColumns + 1;
								}
				| '*' {strcpy((q->columns[q->cntColumns]).name, "*"); q->cntColumns = q->cntColumns + 1;}
				| ID {strcpy((q->columns[q->cntColumns]).name, $1); q->cntColumns = q->cntColumns + 1;}
				| ID LPAREN ID RPAREN {
									strcpy((q->columns[q->cntColumns]).function, $1); 
									strcpy((q->columns[q->cntColumns]).name, $3); 
									q->cntColumns = q->cntColumns + 1;
								}
				//| EXPR','attributeList
				//| EXPR
				;
	/*
	EXPR		: EXPR'+'EXPR
				| EXPR'-'EXPR
				| EXPR'*'EXPR
				| EXPR'/'EXPR
				| LPAREN EXPR RPAREN
				| ID
				;*/
               
    COND       : E { 		
						push(a[count]);
						count==0?count++:count--;
				}
			   | COND OR COND {condition *orcond = newConditionStruct(); orcond->rhs = pop(); orcond->lhs = pop(); orcond->cond=ORC; push(orcond); }
			   | COND AND COND {condition *andcond = newConditionStruct(); andcond->rhs = pop(); andcond->lhs = pop(); andcond->cond=ANDC; push(andcond);}
			   |  LPAREN COND RPAREN  
               ;
    E          : LHS LE RHS {(a[count])->cond = LEQ; }
               | LHS GE RHS {(a[count])->cond = GEQ; }
               | LHS EQ RHS {(a[count])->cond = EQL; }        	
               | LHS NE RHS {(a[count])->cond = NEQL; }
               | LHS OR RHS {(a[count])->cond = ORC; }
               | LHS AND RHS {(a[count])->cond = ANDC; }
               | LHS LIKE RHS {(a[count])->cond = LIKEC; }
			   | LHS GT RHS {(a[count])->cond = GRT; }
			   | LHS LT RHS {(a[count])->cond = LET; }
			   | LHS IN LPAREN RHSINT RPAREN {(a[count])->cond = INC; }
			   | LHS IN LPAREN RHSDBL RPAREN {(a[count])->cond = INC; }
			   | LHS IN LPAREN RHSSTR RPAREN {(a[count])->cond = INC; }
			   | LHS IN LPAREN RHSDTTM RPAREN {(a[count])->cond = INC; }
               ;
    LHS        : ID {a[count] = newConditionStruct(); strcpy((a[count])->colname, $1);  (a[count])->lt = STRING;}
			   ;
	RHS		   : ID {strcpy((a[count])->rightstr[++(a[count])->rightcnt], $1); (a[count])->rt = STRING;}
               | INTNUM {(a[count])->rightint[++(a[count])->rightcnt] = $1; (a[count])->rt = INTEGER;} 
			   | DBLNUM {(a[count])->rightflt[++(a[count])->rightcnt] = $1; (a[count])->rt = FLOAT;}
			   | DATETIME {a[count]->rightdttm[++(a[count])->rightcnt] = getDate($1); (a[count])->rt = DTTM;}
			   | QUOTE ID QUOTE {
									strcpy((a[count])->rightstr[++(a[count])->rightcnt], "\""); 
									strcat((a[count])->rightstr[(a[count])->rightcnt], $2); 
									strcat((a[count])->rightstr[(a[count])->rightcnt], "\""); 
									(a[count])->rt = STRING;
				}
               ; 
    RHSINT	   :INTNUM',' RHSINT {(a[count])->rightint[++(a[count])->rightcnt] = $1; (a[count])->rt = INTEGER;} 
			   | INTNUM {(a[count])->rightint[++(a[count])->rightcnt] = $1; (a[count])->rt = INTEGER;} 
			   ;
	RHSDBL	   :DBLNUM',' RHSDBL {(a[count])->rightflt[++(a[count])->rightcnt] = $1; (a[count])->rt = FLOAT;} 
			   | DBLNUM {(a[count])->rightflt[++(a[count])->rightcnt] = $1; (a[count])->rt = FLOAT;} 
			   ;
	RHSDTTM	   :DATETIME',' RHSDTTM {(a[count])->rightdttm[++(a[count])->rightcnt] = getDate($1); (a[count])->rt = DTTM;} 
			   | DATETIME  {(a[count])->rightdttm[++(a[count])->rightcnt] = getDate($1); (a[count])->rt = DTTM;}  
			   ;
	RHSSTR	   :QUOTE ID QUOTE',' RHSSTR {
									strcpy((a[count])->rightstr[++(a[count])->rightcnt], "\""); 
									strcat((a[count])->rightstr[(a[count])->rightcnt], $2); 
									strcat((a[count])->rightstr[(a[count])->rightcnt], "\""); 
									(a[count])->rt = STRING;
				}
			   | QUOTE ID QUOTE{
									strcpy((a[count])->rightstr[++(a[count])->rightcnt], "\""); 
									strcat((a[count])->rightstr[(a[count])->rightcnt], $2); 
									strcat((a[count])->rightstr[(a[count])->rightcnt], "\""); 
									(a[count])->rt = STRING;
				}
			   ;
%%

query load_query(const char *str) {
    q = new querystruct();
    q->cntColumns = 0;
    q->type[0] = '\0';
    q->error = 0; 
    yy_scan_string(str);
    yyparse();
    return q;
}

void push(condition* con){
	if(top < 100){
		stack[++top] = con;
	}
}

condition* pop(){
	if(top >= 0){
		return stack[top--];
	}
	return NULL;
}

condition* newConditionStruct(){
	condition* d = new conditionStruct();
	d->rightcnt = -1;
	d->lhs = NULL;
	d->rhs = NULL;
	return d;
}

dateStruct* getDate(char dt[]){
	dateStruct* d =(dateStruct *)malloc(sizeof(dateStruct));
	d->dd = 0;
	d->mm = 0;
	d->yyyy = 0;
	d->hh = 0;
	d->mi = 0;
	d->ss = 0;
	d->tz = 0;
	char t[5];
	int i=-1, j=-1, flag = 0;
	if(strlen(dt) > 0){
		do{
			i++;
			j++;
			if(dt[i]=='/' || dt[i]==' ' || dt[i]==':' || dt[i]=='\0'){
				t[j]='\0';
				j = -1;
				switch(flag)
				{
					case 0:
						d->dd = atoi(t);
						break;
					case 1:
						d->mm = atoi(t);
						break;
					case 2:
						d->yyyy = atoi(t);
						break;
					case 3:
						d->hh = atoi(t);
						break;
					case 4:
						d->mi = atoi(t);
						break;
					case 5:
						d->ss = atoi(t);
						break;
					case 6:
						d->tz = atoi(t);
						break;
				}
				flag++;
			}
			else{
				t[j] = dt[i];
			}
		}while(dt[i]!='\0');
	}
	return d;
}
