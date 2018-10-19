%{
#include "parser.tab.h"
#include "string.h"
#include "def.h"
int yycolumn=1;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;
typedef union {
	int type_int;
	int type_float;
	char type_id[32];
	struct node *ptr;
} YYLVAL;
#define YYSTYPE YYLVAL

%}
%option yylineno

id   [A-Za-z][A-Za-z0-9]*  
int    [0-9]+
float  ([0-9]*\.[0-9]+)|([0-9]+\.)
char   (')(.)*(')
string (")(.|\n)*(")

%%
\/\/.* 	{}
(\/\*)(.|\n)*(\*\/) {}

{int}        {yylval.type_int=atoi(yytext); return INT;}
{float}      {yylval.type_float=atof(yytext); return FLOAT;}
{char}			 {memcpy(yylval.type_id-1,  yytext,strlen(yytext)-1);return CHAR;}
"int"        {strcpy(yylval.type_id,  yytext);return TYPE;}
"float"      {strcpy(yylval.type_id,  yytext);return TYPE;}
"char"			 {strcpy(yylval.type_id,  yytext);return TYPE;}
         
"return"     {return RETURN;}
"if"         {return IF;}
"else"       {return ELSE;}
"while"      {return WHILE;}
"for"				 {return FOR;}

{id}       {strcpy(yylval.type_id,  yytext); return ID;}/*���ڹؼ��ֵ���ʽҲ���ϱ�ʾ���Ĺ������԰ѹؼ��ֵĴ���ȫ�����ڱ�ʶ����ǰ�棬����ʶ��*/
";"			{return SEMI;}
","			{return COMMA;}
">"|"<"|">="|"<="|"=="|"!=" {strcpy(yylval.type_id, yytext);;return RELOP;}
"+="    {return PLUSASSIGN;}
"-="		{return MINUSASSIGN;}
"*="		{return MULTASSIGN;}
"/="		{return DIVASSIGN;}
"%="		{return PERASSIGN;}
"++"		{return DPLUS;}
"--"    {return DMINUS;}
"="			{return ASSIGNOP;}
"+"			{return PLUS;}
"%"			{return PER;}
"-"			{return MINUS;}
"*"			{return MULT;}
"/"			{return DIV;}
"&&"    {return AND;}
"||"    {return OR;}
"!"			{return NOT;}
"("			{return LP;}
")"			{return RP;}
"{"			{return LC;}
"}"			{return RC;}
"["			{return LM;}
"]"			{return RM;}
[\n]           {yycolumn=1;}   
[ \r\t]          {}   
.			{printf("Error type A at Line %d\n :Unknown character \"%s\"",yylineno,yytext);}

%%

int yywrap()
{
return 1;
}

