%{
#include "parser.h"
#include "parser.tab.h"
#include <cstring>
using namespace std;
%}

%option noyywrap

alpha [A-Za-z]
digit [0-9]
%%
[ \t\n]
create						return CREATE;
show						return SHOW;
use							return USE;
alter						return ALTER;
column						return COLUMN;
drop						return DROP;
add							return ADD;
modify						return MODIFY;
database					return DATABASE;
databases					return DATABASES;
table						return TABLE;
index						return INDEX;
on							return ON;
tables						return TABLES;  
select 						return SELECT;
insert 						return INSERT;
delete 						return DELETE;
update						return UPDATE;
set							return SET;
into 						return INTO;
values						return VALUES;
distinct         			return DISTINCT;
from             			return FROM;
where           			return WHERE;
like               			return LIKE;
desc              			return DESC;
asc                			return ASC;
limit						return LIMIT;
"group by"    				return GROUP;
having          			return HAVING;
"order by"    				return ORDER;
"char" 						return CHAR;
"varchar"					return VARCHAR;
or                 			return OR;
and               			return AND;
in							return IN;
[-]?{digit}{digit}*			{yylval.integer = atoi(yytext); return INTNUM;}
[-]?{digit}+"."{digit}*		{yylval.flt = atof(yytext); return DBLNUM;}
{alpha}({alpha}|{digit})* 	{strcpy(yylval.id, yytext); return ID;}
{digit}{digit}"/"{digit}{digit}"/"{digit}{digit}{digit}{digit}" "{digit}{digit}":"{digit}{digit}":"{digit}{digit} {strcpy(yylval.datetm, yytext); return DATETIME;}
"<"							return LT;
"<="             			return LE;
">"							return GT;
">="             			return GE;
"="             			return EQ;
"!="              			return NE;
";"							return END;
"\""						return QUOTE;
"("							return LPAREN;
")"							return RPAREN;
. 							return *yytext;
%%