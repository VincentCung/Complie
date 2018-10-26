%error-verbose
%locations
%{
#include<stdarg.h>
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "def.h"
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
%}

%union {
	int    type_int;
	float  type_float;
	char   type_id[32];
	struct node *ptr;
};

//  %type ������ս��������ֵ����
%type  <ptr> program ExtDefList ExtDef Specifier ExtDecList FuncDec CompSt VarList VarDec 
%type  <ptr> ParamDec Stmt StmList DefList Def DecList Dec Exp Args 
%type  <ptr> ArrayChild ArrayList ArrayDec ConList ExpStmt Value ValueList

//% token �����ս��������ֵ����
%token <type_int> INT              //ָ��INT������ֵ��type_int���дʷ������õ�����ֵ
%token <type_id> ID RELOP TYPE CHAR  //ָ��ID,RELOP ������ֵ��type_id���дʷ������õ��ı�ʶ���ַ���
%token <type_float> FLOAT         //ָ��ID������ֵ��type_id���дʷ������õ��ı�ʶ���ַ���

%token LP RP LC RC SEMI COMMA LM RM //��bison�Ը��ļ�����ʱ��������-d�����ɵ�exp.tab.h�и���Щ���ʽ��б��룬����lex.l�а���parser.tab.hʹ����Щ����������
%token PLUS MINUS STAR DIV  ASSIGNOP AND OR NOT IF ELSE WHILE RETURN DPLUS DMINUS FOR BREAK VOID
%token PLUSASSIGN MINUSASSIGN MULTASSIGN DIVASSIGN 

%right ASSIGNOP PLUSASSIGN MINUSASSIGN MULTASSIGN DIVASSIGN 
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left MULT DIV 
%right UMINUS UPLUS NOT FDPLUS FDMINUS DPLUS DMINUS
%left LM RM

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%

program: ExtDefList    { /*display($1,0);*/semantic_Analysis0($1);}     /*��ʾ�﷨��,�������*/
         ; 
ExtDefList: {$$=NULL;}
          | ExtDef ExtDefList {$$=mknode(EXT_DEF_LIST,$1,$2,NULL,yylineno);}   //ÿһ��EXTDEFLIST�Ľ�㣬���1��������Ӧһ���ⲿ������������
          ;  
ExtDef:    Specifier ExtDecList SEMI   {$$=mknode(EXT_VAR_DEF,$1,$2,NULL,yylineno);}   //�ý���Ӧһ���ⲿ��������
         | Specifier FuncDec CompSt    {$$=mknode(FUNC_DEF,$1,$2,$3,yylineno);}         //�ý���Ӧһ����������
         | VOID FuncDec CompSt    {$$=mknode(FUNC_DEF,NULL,$2,$3,yylineno);}         //�ý���Ӧһ����������
         | error SEMI   {$$=NULL; }
         ;
Specifier:  TYPE    {$$=mknode(TYPE,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);$$->type=getType($1);}   
           ;      
ExtDecList:  VarDec      {$$=$1;}       /*ÿһ��EXT_DECLIST�Ľ�㣬���һ��������Ӧһ��������(ID���͵Ľ��),�ڶ���������Ӧʣ�µ��ⲿ������*/
           | ArrayDec      {$$=$1;} 
           | VarDec COMMA ExtDecList {$$=mknode(EXT_DEC_LIST,$1,$3,NULL,yylineno);}
           | ArrayDec COMMA ExtDecList {$$=mknode(EXT_DEC_LIST,$1,$3,NULL,yylineno);}
           ;  
VarDec:  ID          {$$=mknode(ID,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}   //ID��㣬��ʶ�����Ŵ���Ž���type_id
        ;
ArrayDec: ID ArrayList {$$=mknode(ARR_ELE,$2,NULL,NULL,yylineno);strcpy($$->type_id,$1);}        
        ;
FuncDec: ID LP VarList RP   {$$=mknode(FUNC_DEC,$3,NULL,NULL,yylineno);strcpy($$->type_id,$1);}//�����������$$->type_id
	|ID LP  RP   {$$=mknode(FUNC_DEC,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}//�����������$$->type_id
        ;  
VarList: ParamDec  {$$=mknode(PARAM_LIST,$1,NULL,NULL,yylineno);}
        | ParamDec COMMA  VarList  {$$=mknode(PARAM_LIST,$1,$3,NULL,yylineno);}
        ;
ParamDec: Specifier VarDec         {$$=mknode(PARAM_DEC,$1,$2,NULL,yylineno);}
         ;
CompSt: LC DefList StmList RC    {$$=mknode(COMP_STM,$2,$3,NULL,yylineno);}
       ;
StmList: {$$=NULL; }  
        | Stmt StmList  {$$=mknode(STM_LIST,$1,$2,NULL,yylineno);}
        ;
Stmt:  ExpStmt {$$=$1;}
      | CompSt      {$$=$1;}      //���������ֱ����Ϊ����㣬���������µĽ��
      | RETURN Exp SEMI   {$$=mknode(RETURN,$2,NULL,NULL,yylineno);}
      | IF LP Exp RP Stmt %prec LOWER_THEN_ELSE   {$$=mknode(IF_THEN,$3,$5,NULL,yylineno);}
      | IF LP Exp RP Stmt ELSE Stmt   {$$=mknode(IF_THEN_ELSE,$3,$5,$7,yylineno);}
      | WHILE LP Exp RP Stmt {$$=mknode(WHILE,$3,$5,NULL,yylineno);}
      | FOR LP ConList RP Stmt {$$=mknode(FOR,$3,$5,NULL,yylineno);}
      | BREAK SEMI {$$=mknode(BREAK,NULL,NULL,NULL,yylineno);}
      ;
ConList: ExpStmt ExpStmt Exp {$$=mknode(CON_LIST,$1->ptr[0],$2->ptr[0],$3,yylineno);}
        | ExpStmt ExpStmt {$$=mknode(CON_LIST,$1->ptr[0],$2->ptr[0],NULL,yylineno);}
        ;
ExpStmt: Exp SEMI {$$=mknode(EXP_STMT,$1,NULL,NULL,yylineno);}
        | SEMI {$$=mknode(EXP_STMT,NULL,NULL,NULL,yylineno);}
        ;
DefList: {$$=NULL; }
        | Def DefList {$$=mknode(DEF_LIST,$1,$2,NULL,yylineno);}
        ;
Def:    Specifier DecList SEMI {$$=mknode(VAR_DEF,$1,$2,NULL,yylineno);} //�������
        ;
DecList: Dec  {$$=mknode(DEC_LIST,$1,NULL,NULL,yylineno);} //������
       | Dec COMMA DecList  {$$=mknode(DEC_LIST,$1,$3,NULL,yylineno);}
	   ;
Dec:     VarDec  {$$=$1;} //�����������ʽ
       | VarDec ASSIGNOP Exp  {$$=mknode(ASSIGNOP,$1,$3,NULL,yylineno);strcpy($$->type_id,"ASSIGNOP");}
       | ArrayDec ASSIGNOP LC ValueList RC {$$=mknode(ASSIGNOP,$1,$4,NULL,yylineno);strcpy($$->type_id,"ASSIGNOP");}
       | ArrayDec {$$=$1;} //������������
       ;
Exp:    Exp ASSIGNOP Exp {$$=mknode(ASSIGNOP,$1,$3,NULL,yylineno);strcpy($$->type_id,"ASSIGNOP");}//$$���type_id����δ�ã����ô�������
      | Exp PLUSASSIGN Exp {$$=mknode(PLUSASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"PLUSASSIGN");}
      | Exp MINUSASSIGN Exp {$$=mknode(MINUSASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"MINUSASSIGN");}
      | Exp MULTASSIGN Exp {$$=mknode(MULTASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"MULTASSIGN");}
      | Exp DIVASSIGN Exp {$$=mknode(DIVASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"DIVASSIGN");}
      | Exp AND Exp   {$$=mknode(AND,$1,$3,NULL,yylineno);strcpy($$->type_id,"AND");}
      | Exp OR Exp    {$$=mknode(OR,$1,$3,NULL,yylineno);strcpy($$->type_id,"OR");}
      | Exp RELOP Exp {$$=mknode(RELOP,$1,$3,NULL,yylineno);strcpy($$->type_id,$2);}  //�ʷ�������ϵ�����������ֵ������$2��
      | Exp PLUS Exp  {$$=mknode(PLUS,$1,$3,NULL,yylineno);strcpy($$->type_id,"PLUS");}
      | Exp MINUS Exp {$$=mknode(MINUS,$1,$3,NULL,yylineno);strcpy($$->type_id,"MINUS");}
      | Exp MULT Exp  {$$=mknode(MULT,$1,$3,NULL,yylineno);strcpy($$->type_id,"MULT");}
      | Exp DIV Exp   {$$=mknode(DIV,$1,$3,NULL,yylineno);strcpy($$->type_id,"DIV");}
      | Exp DPLUS     {$$=mknode(DPLUS,$1,NULL,NULL,yylineno);strcpy($$->type_id,"DPLUS");}
      | Exp DMINUS     {$$=mknode(DMINUS,$1,NULL,NULL,yylineno);strcpy($$->type_id,"DMINUS");}
      | DPLUS Exp %prec FDPLUS    {$$=mknode(FDPLUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"FDPLUS");}
      | DMINUS Exp %prec FDMINUS    {$$=mknode(FDMINUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"FDMINUS");}
      | MINUS Exp %prec UMINUS   {$$=mknode(UMINUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"UMINUS");}
      | PLUS Exp %prec UPLUS   {$$=mknode(UPLUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"UPLUS");}
      | NOT Exp       {$$=mknode(NOT,$2,NULL,NULL,yylineno);strcpy($$->type_id,"NOT");}
      | ID LP Args RP {$$=mknode(FUNC_CALL,$3,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
      | ID LP RP      {$$=mknode(FUNC_CALL,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
      | Exp LM Exp RM {$$=mknode(ARR_ACCESS,$1,$3,NULL,yylineno);strcpy($$->type_id,"ARR");}
      | LP Exp RP     {$$=$2;}
      | VarDec        {$$=$1;}
      | Value         {$$=$1;}
      ;
Value: INT           {$$=mknode(INT,NULL,NULL,NULL,yylineno);$$->type_int=$1;$$->type=INT;}
      | FLOAT         {$$=mknode(FLOAT,NULL,NULL,NULL,yylineno);$$->type_float=$1;$$->type=FLOAT;}
      | CHAR         {$$=mknode(CHAR,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);$$->type=CHAR;}
      ;
ValueList: {$$=NULL}
        | Value COMMA ValueList {$$=mknode(VAL_LIST,$1,$3,NULL,yylineno);}
        | Value {$$=mknode(VAL_LIST,$1,NULL,NULL,yylineno);}
        ;
Args:    Exp COMMA Args    {$$=mknode(ARGS,$1,$3,NULL,yylineno);}
       | Exp               {$$=mknode(ARGS,$1,NULL,NULL,yylineno);}
       ;
ArrayList: ArrayChild ArrayList  {$$=mknode(ARR_LIST,$1,$2,NULL,yylineno);}
       | ArrayChild {$$=mknode(ARR_LIST,$1,NULL,NULL,yylineno);}
       ;
ArrayChild:LM INT RM {$$=mknode(ARR_SUB,NULL,NULL,NULL,yylineno);$$->type_int=$2;$$->type=INT;}
       ;
%%


int main(int argc, char *argv[])
{
    yyin=fopen(argv[1],"r");
	if (!yyin) return;
	yylineno=1;
	yyparse();
	return 0;
}

void yyerror(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Error type B at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}	


 
