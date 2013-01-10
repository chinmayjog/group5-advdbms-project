%{
#include "parser.h"
#include "parser.tab.h"
#include <cstring>
using namespace std;
%}

%option noyywrap

alpha [A-Za-z]
digit [0-9]
hour  [0-2]
minsec [0-5]
%%
[ \t\n]
"$cache"					return CACHE;
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
primary						return PRIMARY;
key							return KEY;
autoincrement				return AUTOINCREMENT;
default						return DEFAULT;
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
char 						{strcpy(yylval.id, "CHAR$$$$"); return CHAR;}
varchar						{strcpy(yylval.id, "VARCHAR$"); return VARCHAR;}
int							{strcpy(yylval.id, "INT$$$$$"); return DATATYPE;}
bigint						{strcpy(yylval.id, "BIGINT$$"); return DATATYPE;}
ubig						{strcpy(yylval.id, "UBIG$$$$"); return DATATYPE;}						
uint						{strcpy(yylval.id, "UINT$$$$"); return DATATYPE;}
smallint					{strcpy(yylval.id, "SMALLINT"); return DATATYPE;}
usmall						{strcpy(yylval.id, "USMALL$$"); return DATATYPE;}
float						{strcpy(yylval.id, "FLOAT$$$"); return DATATYPE;}
double						{strcpy(yylval.id, "DOUBLE$$"); return DATATYPE;}
date						{strcpy(yylval.id, "DATE$$$$"); return DATATYPE;}
time						{strcpy(yylval.id, "TIME$$$$"); return DATATYPE;}
datetime					{strcpy(yylval.id, "DATETIME"); return DATATYPE;}
null						{strcpy(yylval.id, "#####"); return NULLTYPE;}
or                 			return OR;
and               			return AND;
in							return IN;
[-]?{digit}{digit}*			{strcpy(yylval.id, yytext); return INTNUM;}
[-]?{digit}+"."{digit}*		{strcpy(yylval.id, yytext); return DBLNUM;}
{alpha}({alpha}|{digit})* 	{strcpy(yylval.id, yytext); return ID;}
{digit}{digit}"/"{digit}{digit}"/"{digit}{digit}{digit}{digit}" "{hour}{digit}":"{minsec}{digit}":"{minsec}{digit} {strcpy(yylval.datetm, yytext); return DATETIME;}
{digit}{digit}"/"{digit}{digit}"/"{digit}{digit}{digit}{digit}  {strcpy(yylval.datetm, yytext); return DATE;}
{hour}{digit}":"{minsec}{digit}":"{minsec}{digit} {strcpy(yylval.datetm, yytext); return TIME;}
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
