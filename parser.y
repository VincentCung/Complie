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

//  %type 定义非终结符的语义值类型
%type  <ptr> program ExtDefList ExtDef Specifier ExtDecList FuncDec CompSt VarList VarDec 
%type  <ptr> ParamDec Stmt StmList DefList Def DecList Dec Exp Args 
%type  <ptr> ArrayChild ArrayList ArrayDec ConList ExpStmt Value ValueList

//% token 定义终结符的语义值类型
%token <type_int> INT              //指定INT的语义值是type_int，有词法分析得到的数值
%token <type_id> ID RELOP TYPE CHAR  //指定ID,RELOP 的语义值是type_id，有词法分析得到的标识符字符串
%token <type_float> FLOAT         //指定ID的语义值是type_id，有词法分析得到的标识符字符串

%token LP RP LC RC SEMI COMMA LM RM //用bison对该文件编译时，带参数-d，生成的exp.tab.h中给这些单词进行编码，可在lex.l中包含parser.tab.h使用这些单词种类码
%token PLUS MINUS STAR DIV PER ASSIGNOP AND OR NOT IF ELSE WHILE RETURN DPLUS DMINUS FOR BREAK
%token PLUSASSIGN MINUSASSIGN MULTASSIGN DIVASSIGN PERASSIGN 

%left ASSIGNOP PLUSASSIGN MINUSASSIGN MULTASSIGN DIVASSIGN PERASSIGN
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left MULT DIV PER
%right UMINUS UPLUS NOT FDPLUS FDMINUS DPLUS DMINUS
%left LM RM

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%

program: ExtDefList    { display($1,0);/*semantic_Analysis0($1);*/}     /*显示语法树,语义分析*/
         ; 
ExtDefList: {$$=NULL;}
          | ExtDef ExtDefList {$$=mknode(EXT_DEF_LIST,$1,$2,NULL,yylineno);}   //每一个EXTDEFLIST的结点，其第1棵子树对应一个外部变量声明或函数
          ;  
ExtDef:   Specifier ExtDecList SEMI   {$$=mknode(EXT_VAR_DEF,$1,$2,NULL,yylineno);}   //该结点对应一个外部变量声明
         |Specifier FuncDec CompSt    {$$=mknode(FUNC_DEF,$1,$2,$3,yylineno);}         //该结点对应一个函数定义
         | error SEMI   {$$=NULL; }
         ;
Specifier:  TYPE    {$$=mknode(TYPE,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);$$->type=!strcmp($1,"int")?INT:FLOAT;}   
           ;      
ExtDecList:  VarDec      {$$=$1;}       /*每一个EXT_DECLIST的结点，其第一棵子树对应一个变量名(ID类型的结点),第二棵子树对应剩下的外部变量名*/
           | ArrayDec      {$$=$1;} 
           | VarDec COMMA ExtDecList {$$=mknode(EXT_DEC_LIST,$1,$3,NULL,yylineno);}
           | ArrayDec COMMA ExtDecList {$$=mknode(EXT_DEC_LIST,$1,$3,NULL,yylineno);}
           ;  
VarDec:  ID          {$$=mknode(ID,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}   //ID结点，标识符符号串存放结点的type_id
        ;
ArrayDec: ID ArrayList {$$=mknode(ARRAY_ELE,$2,NULL,NULL,yylineno);strcpy($$->type_id,$1);}        
        ;
FuncDec: ID LP VarList RP   {$$=mknode(FUNC_DEC,$3,NULL,NULL,yylineno);strcpy($$->type_id,$1);}//函数名存放在$$->type_id
	|ID LP  RP   {$$=mknode(FUNC_DEC,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}//函数名存放在$$->type_id
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
      | CompSt      {$$=$1;}      //复合语句结点直接最为语句结点，不再生成新的结点
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
Def:    Specifier DecList SEMI {$$=mknode(VAR_DEF,$1,$2,NULL,yylineno);} //声明语句
        ;
DecList: Dec  {$$=mknode(DEC_LIST,$1,NULL,NULL,yylineno);} //声明表
       | Dec COMMA DecList  {$$=mknode(DEC_LIST,$1,$3,NULL,yylineno);}
	   ;
Dec:     VarDec  {$$=$1;} //单个声明表达式
       | VarDec ASSIGNOP Exp  {$$=mknode(ASSIGNOP,$1,$3,NULL,yylineno);strcpy($$->type_id,"ASSIGNOP");}
       | ArrayDec ASSIGNOP LC ValueList RC {$$=mknode(ASSIGNOP,$1,$4,NULL,yylineno);strcpy($$->type_id,"ASSIGNOP");}
       | ArrayDec {$$=$1;} //单个数组声明
       ;
Exp:    Exp ASSIGNOP Exp {$$=mknode(ASSIGNOP,$1,$3,NULL,yylineno);strcpy($$->type_id,"ASSIGNOP");}//$$结点type_id空置未用，正好存放运算符
      | Exp PLUSASSIGN Exp {$$=mknode(PLUSASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"PLUSASSIGN");}
      | Exp MINUSASSIGN Exp {$$=mknode(MINUSASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"MINUSASSIGN");}
      | Exp MULTASSIGN Exp {$$=mknode(MULTASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"MULTASSIGN");}
      | Exp DIVASSIGN Exp {$$=mknode(DIVASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"DIVASSIGN");}
      | Exp PERASSIGN Exp {$$=mknode(PERASSIGN,$1,$3,NULL,yylineno);strcpy($$->type_id,"PERASSIGN");}
      | Exp AND Exp   {$$=mknode(AND,$1,$3,NULL,yylineno);strcpy($$->type_id,"AND");}
      | Exp OR Exp    {$$=mknode(OR,$1,$3,NULL,yylineno);strcpy($$->type_id,"OR");}
      | Exp RELOP Exp {$$=mknode(RELOP,$1,$3,NULL,yylineno);strcpy($$->type_id,$2);}  //词法分析关系运算符号自身值保存在$2中
      | Exp PLUS Exp  {$$=mknode(PLUS,$1,$3,NULL,yylineno);strcpy($$->type_id,"PLUS");}
      | Exp MINUS Exp {$$=mknode(MINUS,$1,$3,NULL,yylineno);strcpy($$->type_id,"MINUS");}
      | Exp MULT Exp  {$$=mknode(MULT,$1,$3,NULL,yylineno);strcpy($$->type_id,"MULT");}
      | Exp DIV Exp   {$$=mknode(DIV,$1,$3,NULL,yylineno);strcpy($$->type_id,"DIV");}
      | Exp PER Exp   {$$=mknode(PER,$1,$3,NULL,yylineno);strcpy($$->type_id,"PER");}
      | Exp DPLUS     {$$=mknode(DPLUS,$1,NULL,NULL,yylineno);strcpy($$->type_id,"DPLUS");}
      | Exp LM Exp RM
      | DPLUS Exp %prec FDPLUS    {$$=mknode(FDPLUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"FDPLUS");}
      | Exp DMINUS     {$$=mknode(DMINUS,$1,NULL,NULL,yylineno);strcpy($$->type_id,"DMINUS");}
      | DMINUS Exp %prec FDMINUS    {$$=mknode(FDMINUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"FDMINUS");}
      | LP Exp RP     {$$=$2;}
      | MINUS Exp %prec UMINUS   {$$=mknode(UMINUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"UMINUS");}
      | PLUS Exp %prec UPLUS   {$$=mknode(UPLUS,$2,NULL,NULL,yylineno);strcpy($$->type_id,"UPLUS");}
      | NOT Exp       {$$=mknode(NOT,$2,NULL,NULL,yylineno);strcpy($$->type_id,"NOT");}
      | ID LP Args RP {$$=mknode(FUNC_CALL,$3,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
      | ID LP RP      {$$=mknode(FUNC_CALL,NULL,NULL,NULL,yylineno);strcpy($$->type_id,$1);}
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
ArrayList: ArrayChild ArrayList  {$$=mknode(ARRAY_LIST,$1,$2,NULL,yylineno);}
       | ArrayChild {$$=mknode(ARRAY_LIST,$1,NULL,NULL,yylineno);}
       ;
ArrayChild:LM INT RM {$$=mknode(ARRAY_SUB,NULL,NULL,NULL,yylineno);$$->type_int=$2;$$->type=INT;}
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


 
