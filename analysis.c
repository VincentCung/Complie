#include "def.h"

int LEV = 0; //层号

void semantic_error(int line, char *msg1, char *msg2)
{
  //这里可以只收集错误信息，最后在一次显示
  printf("Error type C at Line %d :%s %s\n", line, msg1, msg2);
}

char *strcat0(char *s1, char *s2)
{
  static char result[10];
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

char *newAlias()
{
  static int no = 1;
  char s[10];
  itoa(no++, s, 10);
  return strcat0("v", s);
}

char *newLabel()
{
  static int no = 1;
  char s[10];
  itoa(no++, s, 10);
  return strcat0("label", s);
}

char *newTemp()
{
  static int no = 1;
  char s[10];
  itoa(no++, s, 10);
  return strcat0("temp", s);
}

char *getTypeChar(enum node_kind src)
{
  switch (src)
  {
  case INT:
    return "int";
    break;
  case FLOAT:
    return "float";
    break;
  case CHAR:
    return "char";
    break;
  case VOID:
    return "void";
    break;
  default:
    return NULL;
  }
  return NULL;
}

int getWidth(enum node_kind src)
{
  switch (src)
  {
  case INT:
    return INT_LENGTH;
    break;
  case FLOAT:
    return FLOAT_LENGTH;
    break;
  case CHAR:
    return CHAR_LENGTH;
    break;
  default:
    return 0;
  }
  return 0;
}

enum node_kind getType(char *src)
{
  if (!strcmp(src, "int"))
  {
    return INT;
  }
  else if (!strcmp(src, "float"))
  {
    return FLOAT;
  }
  else if (!strcmp(src, "char"))
  {
    return CHAR;
  }
  else
  {
    return -1;
  }
}

enum node_kind getOpFromAssign(enum node_kind src)
{
  switch (src)
  {
  case DIVASSIGN:
    return DIV;
  case PLUSASSIGN:
    return PLUS;
  case MINUSASSIGN:
    return MINUS;
  case MULTASSIGN:
    return MULT;
  default:
    return;
  }
}

enum node_kind getRelOpFromString(char *src)
{
  if (strcmp(src, "<") == 0)
    return JLT;
  else if (strcmp(src, "<=") == 0)
    return JLE;
  else if (strcmp(src, ">") == 0)
    return JGT;
  else if (strcmp(src, ">=") == 0)
    return JGE;
  else if (strcmp(src, "==") == 0)
    return EQ;
  else if (strcmp(src, "!=") == 0)
    return NEQ;
  else
    return 0;
}

void setArrayAribute(struct node *T, int rtn)
{
  struct node *T0 = T->ptr[0];
  while (T0)
  {
    if (T0->ptr[0]->type_int <= 0)
    {
      semantic_error(T->pos, T->type_id, "set bound is invalid.");
      T->error = 1;
    }
    T->arrayLimit[T->dimension++] = T0->ptr[0]->type_int;
    T0 = T0->ptr[1];
  }
  T->num = 1;
  int num = T->dimension;
  symbolTable.symbols[rtn].dimension = T->dimension;
  while (num--)
  {
    T->num *= T->arrayLimit[num];
    symbolTable.symbols[rtn].arrayLimit[num] = T->arrayLimit[num];
  }
}

void setCodeVal(struct opn src, struct node *T, enum node_kind type)
{
  switch (type)
  {
  case INT:
    src.const_int = T->type_int;
    break;
  case FLOAT:
    src.const_float = T->type_float;
    break;
  case CHAR:
    src.const_char = T->type_id[0];
    break;
  default:
    break;
  }
}

void setFalsyVal(struct opn src, enum node_kind type)
{
  switch (type)
  {
  case INT:
    src.const_int = 0;
    break;
  case FLOAT:
    src.const_float = 0.0;
    break;
  case CHAR:
    src.const_char = '\0';
    break;
  default:
    break;
  }
}

//生成一条TAC代码的结点组成的双向循环链表，返回头指针
struct codenode *genIR(int op, struct opn opn1, struct opn opn2, struct opn result)
{
  struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
  h->op = op;
  h->opn1 = opn1;
  h->opn2 = opn2;
  h->result = result;
  h->next = h->prior = h;
  return h;
}

//生成一条标号语句，返回头指针
struct codenode *genLabel(char *label)
{
  struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
  h->op = LABEL;
  strcpy(h->result.id, label);
  h->next = h->prior = h;
  return h;
}

//生成GOTO语句，返回头指针
struct codenode *genGoto(char *label)
{
  struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
  h->op = GOTO;
  strcpy(h->result.id, label);
  h->next = h->prior = h;
  return h;
}

//合并多个中间代码的双向循环链表，首尾相连
struct codenode *merge(int num, ...)
{
  struct codenode *h1, *h2, *p, *t1, *t2;
  va_list ap;
  va_start(ap, num);
  h1 = va_arg(ap, struct codenode *);
  while (--num > 0)
  {
    h2 = va_arg(ap, struct codenode *);
    if (h1 == NULL)
      h1 = h2;
    else if (h2)
    {
      t1 = h1->prior;
      t2 = h2->prior;
      t1->next = h2;
      t2->next = h1;
      h1->prior = t2;
      h2->prior = t1;
    }
  }
  va_end(ap);
  return h1;
}

//输出中间代码
void prnIR(struct codenode *head)
{
  char opnstr1[32], opnstr2[32], resultstr[32];
  struct codenode *h = head;
  do
  {
    if (h->opn1.kind == INT)
      sprintf(opnstr1, "#%d", h->opn1.const_int);
    if (h->opn1.kind == FLOAT)
      sprintf(opnstr1, "#%f", h->opn1.const_float);
    if (h->opn1.kind == ID)
      sprintf(opnstr1, "%s", h->opn1.id);
    if (h->opn2.kind == INT)
      sprintf(opnstr2, "#%d", h->opn2.const_int);
    if (h->opn2.kind == FLOAT)
      sprintf(opnstr2, "#%f", h->opn2.const_float);
    if (h->opn2.kind == ID)
      sprintf(opnstr2, "%s", h->opn2.id);
    sprintf(resultstr, "%s", h->result.id);
    switch (h->op)
    {
    case ASSIGNOP:
      printf("  %s := %s\n", resultstr, opnstr1);
      break;
    case PLUS:
    case MINUS:
    case STAR:
    case DIV:
      printf("  %s := %s %c %s\n", resultstr, opnstr1,
             h->op == PLUS ? '+' : h->op == MINUS ? '-' : h->op == STAR ? '*' : '\\', opnstr2);
      break;
    case FUNCTION:
      printf("\nFUNCTION %s :\n", h->result.id);
      break;
    case PARAM:
      printf("  PARAM %s\n", h->result.id);
      break;
    case LABEL:
      printf("LABEL %s :\n", h->result.id);
      break;
    case GOTO:
      printf("  GOTO %s\n", h->result.id);
      break;
    case JLE:
      printf("  IF %s <= %s GOTO %s\n", opnstr1, opnstr2, resultstr);
      break;
    case JLT:
      printf("  IF %s < %s GOTO %s\n", opnstr1, opnstr2, resultstr);
      break;
    case JGE:
      printf("  IF %s >= %s GOTO %s\n", opnstr1, opnstr2, resultstr);
      break;
    case JGT:
      printf("  IF %s > %s GOTO %s\n", opnstr1, opnstr2, resultstr);
      break;
    case EQ:
      printf("  IF %s == %s GOTO %s\n", opnstr1, opnstr2, resultstr);
      break;
    case NEQ:
      printf("  IF %s != %s GOTO %s\n", opnstr1, opnstr2, resultstr);
      break;
    case ARG:
      printf("  ARG %s\n", h->result.id);
      break;
    case CALL:
      printf("  %s := CALL %s\n", resultstr, opnstr1);
      break;
    case RETURN:
      if (h->result.kind)
        printf("  RETURN %s\n", resultstr);
      else
        printf("  RETURN\n");
      break;
    }
    h = h->next;
  } while (h != head);
}

void prn_symbol()
{
  int i = 0;
  printf("%6s %6s %6s  %6s %6s %6s\n", "var", "alias", "level", "type", "label", "offset");
  for (i = 0; i < symbolTable.index; i++)
    printf("%6s %6s %6d  %6s %6c %6d\n", symbolTable.symbols[i].name,
           symbolTable.symbols[i].alias, symbolTable.symbols[i].level,
           getTypeChar(symbolTable.symbols[i].type),
           symbolTable.symbols[i].flag, symbolTable.symbols[i].offset);
}

int searchSymbolTable(char *name)
{
  int i;
  for (i = symbolTable.index - 1; i >= 0; i--)
    if (!strcmp(symbolTable.symbols[i].name, name))
      return i;
  return -1;
}

int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset)
{
  //首先根据name查符号表，不能重复定义 重复定义返回-1
  int i;
  /*符号查重，考虑外部变量声明前有函数定义，
    其形参名还在符号表中，这时的外部变量与前函数的形参重名是允许的*/
  for (i = symbolTable.index - 1; symbolTable.symbols[i].level == level || (level == 0 && i >= 0); i--)
  {
    if (level == 0 && symbolTable.symbols[i].level == 1)
      continue; //外部变量和形参不必比较重名
    if (!strcmp(symbolTable.symbols[i].name, name))
      return -1;
  }
  //填写符号表内容
  strcpy(symbolTable.symbols[symbolTable.index].name, name);
  strcpy(symbolTable.symbols[symbolTable.index].alias, alias);
  symbolTable.symbols[symbolTable.index].level = level;
  symbolTable.symbols[symbolTable.index].type = type;
  symbolTable.symbols[symbolTable.index].flag = flag;
  symbolTable.symbols[symbolTable.index].offset = offset;
  return symbolTable.index++; //返回的是符号在符号表中的位置序号，中间代码生成时可用序号取到符号别名
}

//填写临时变量到符号表，返回临时变量在符号表中的位置
int fill_Temp(char *name, int level, int type, char flag, int offset)
{
  strcpy(symbolTable.symbols[symbolTable.index].name, "");
  strcpy(symbolTable.symbols[symbolTable.index].alias, name);
  symbolTable.symbols[symbolTable.index].level = level;
  symbolTable.symbols[symbolTable.index].type = type;
  symbolTable.symbols[symbolTable.index].flag = flag;
  symbolTable.symbols[symbolTable.index].offset = offset;
  return symbolTable.index++; //返回的是临时变量在符号表中的位置序号
}

void ext_var_list(struct node *T) //处理外部变量列表
{
  int rtn, num = 1;
  switch (T->kind)
  {
  case EXT_DEC_LIST:
    T->ptr[0]->type = T->type;     //将类型属性向下传递变量结点
    T->ptr[0]->offset = T->offset; //外部变量的偏移量向下传递
    ext_var_list(T->ptr[0]);
    T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
    T->ptr[1]->type = T->type;                //将类型属性向下传递变量结点
    T->ptr[1]->offset = T->offset + T->width; //外部变量的偏移量向下传递
    T->ptr[1]->width = T->width;
    ext_var_list(T->ptr[1]);
    T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
    T->num = T->ptr[1]->num + T->ptr[0]->num;
    break;
  case ARR_ELE:
    rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'A', T->offset); //最后一个变量名
    if (rtn == -1)
    {
      T->error = 1;
      semantic_error(T->pos, T->type_id, "same variablehas defined.");
    }
    else
    {
      T->place = rtn;
    }
    setArrayAribute(T, rtn);
    break;
  case ID:
    rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'V', T->offset); //最后一个变量名
    if (rtn == -1)
    {
      T->error = 1;
      semantic_error(T->pos, T->type_id, "same variable has defined.");
    }
    else
    {
      T->place = rtn;
    }
    T->num = 1;
    break;
  }
}

void local_var_list(struct node *T)
{
  int num, rtn;
  struct opn opn1, opn2, result;
  T->code = NULL;
  T->ptr[1]->type = T->ptr[0]->type; //确定变量序列各变量类型
  struct node *T0 = T->ptr[1];       //T0为变量名列表子树根指针，对ID、ASSIGNOP类结点在登记到符号表，作为局部变量
  struct node *T1;
  T0->offset = T->offset;
  T->width = 0;
  int width = getWidth(T->ptr[1]->type); //一个变量宽度
  while (T0)
  {                                  //处理所以DEC_LIST结点
    T0->ptr[0]->type = T0->type;     //类型属性向下传递
    T0->ptr[0]->offset = T0->offset; //类型属性向下传递
    if (T0->ptr[1])
    {
      T0->ptr[1]->type = T0->type;
      T0->ptr[1]->offset = T0->offset + width * T0->num;
    }
    if (T0->ptr[0]->kind == ID)
    {
      rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T->offset + T->width); //此处偏移量未计算，暂时为0
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T0->ptr[0]->pos, T0->ptr[0]->type_id, "same variable has defined.");
      }
      else
        T0->ptr[0]->place = rtn;
      T0->num = 1;
      T->width += width;
    }
    else if (T0->ptr[0]->kind == ARR_ELE)
    {
      rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'A', T->offset + T->width); //此处偏移量未计算，暂时为0
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T0->ptr[0]->pos, T0->ptr[0]->type_id, "same variable has defined.");
      }
      else
        T0->ptr[0]->place = rtn;
      setArrayAribute(T0->ptr[0], rtn);
      T0->num = T0->ptr[0]->num;
      T->width += width * T0->num;
    }
    else if (T0->ptr[0]->kind == ASSIGNOP && T0->ptr[0]->ptr[0]->kind == ID)
    {
      rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T->offset + T->width); //此处偏移量未计算，暂时为0
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "same variable has defined.");
      }
      else
      {
        T0->ptr[0]->place = rtn;
        T0->ptr[0]->ptr[1]->offset = T->offset + T->width + width;
        Exp(T0->ptr[0]->ptr[1]);
        T->error = T0->ptr[0]->ptr[1]->error ? T0->ptr[0]->ptr[1]->error : T->error;
        opn1.kind = ID;
        strcpy(opn1.id, symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
        T->code = merge(3, T->code, T0->ptr[0]->ptr[1]->code, genIR(ASSIGNOP, opn1, opn2, result));
      }
      T->num = T0->ptr[0]->ptr[0]->num;
      T->width += width + T0->ptr[0]->ptr[1]->width;
    }
    else if (T0->ptr[0]->kind == ASSIGNOP && T0->ptr[0]->ptr[0]->kind == ARR_ELE)
    {
      rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'A', T->offset + T->width); //此处偏移量未计算，暂时为0
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "same variable has defined.");
      }
      else
      {
        setArrayAribute(T0->ptr[0]->ptr[0], rtn);
        T0->ptr[0]->place = rtn;
        T1 = T0->ptr[0]->ptr[1];
        while (T1)
        {
          if (T1->ptr[0]->type != T0->type)
          {
            T->error = 1;
            semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "define invalid type variable.");
          }
          T1->ptr[0]->place = fill_Temp(newTemp(), LEV, T0->type, 'T', T->offset);
          T1->ptr[0]->type = T0->type;
          opn1.kind = T0->type;
          setCodeVal(opn1, T1->ptr[0], T0->type);
          result.kind = ID;
          strcpy(result.id, symbolTable.symbols[T1->ptr[0]->place].alias);
          result.offset = symbolTable.symbols[T1->ptr[0]->place].offset;
          T1->ptr[0]->code = genIR(ASSIGNOP, opn1, opn2, result);
          T0->ptr[0]->ptr[1]->num++;
          T1 = T1->ptr[1];
        }
        if (T0->ptr[0]->ptr[1] && T0->ptr[0]->ptr[1]->num > T0->ptr[0]->ptr[0]->num)
        {
          T->error = 1;
          semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "too much variables.");
        }
        // if(!T0->ptr[0]->ptr[1])
        // {
        //   rtn = fill_Temp(newTemp(), LEV, T0->type, 'T', T->offset);
        //   opn1.kind = T0->type;
        //   setFalsyVal(opn1, T0->type);
        //   result.kind = ID;
        //   strcpy(result.id, symbolTable.symbols[T->place].alias);
        //   result.offset = symbolTable.symbols[T->place].offset;
        //   T->code = genIR(ASSIGNOP, opn1, opn2, result);
        // }
        //T0->ptr[0]->ptr[1]->offset = T->offset + T->width + width*T0->ptr[0]->ptr[0]->num;
        // opn1.kind = ID;
        // strcpy(opn1.id, symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
        // result.kind = ID;
        // strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
        // T->code = merge(3, T->code, T0->ptr[0]->ptr[1]->code, genIR(ASSIGNOP, opn1, opn2, result));
      }

      T->num = T0->ptr[0]->ptr[0]->num;
      T->width += width * T->num;
    }
    T0 = T0->ptr[1];
  }
}

//匹配参数个数类型
int match_param(int i, struct node *T, int pos)
{
  int j, num = symbolTable.symbols[i].paramnum;
  int type1, type2;
  struct node *T0 = T;
  if (num == 0 && T == NULL)
  {
    return 1;
  }
  for (j = 1; j <= num; j++)
  {
    if (!T)
    {
      T0->error = 1;
      semantic_error(pos, "", "function's parameters are too few.");
      return 0;
    }
    type1 = symbolTable.symbols[i + j].type; //形参类型
    type2 = T->ptr[0]->type;
    if (type1 != type2)
    {
      T0->error = 1;
      semantic_error(pos, "", "parameter doesn't match.");
      return 0;
    }
    T = T->ptr[1];
  }
  if (T)
  { //num个参数已经匹配完，还有实参表达式
    T0->error = 1;
    semantic_error(pos, "", "function's parameters are too many.");
    return 0;
  }
  return 1;
}
//布尔表达式，参考文献[2]p84的思想
void boolExp(struct node *T)
{
  struct opn opn1, opn2, result;
  int op;
  int rtn;
  if (T)
  {
    switch (T->kind)
    {
    case INT:
      if (T->type_int != 0)
        T->code = genGoto(T->Etrue);
      else
        T->code = genGoto(T->Efalse);
      T->width = 0;
      break;
    case FLOAT:
      if (T->type_float != 0.0)
        T->code = genGoto(T->Etrue);
      else
        T->code = genGoto(T->Efalse);
      T->width = 0;
      break;
    case CHAR:
      if (strlen(T->type_id) != 0)
        T->code = genGoto(T->Etrue);
      else
        T->code = genGoto(T->Efalse);
    case ID: //查符号表，获得符号表中的位置，类型送type
      rtn = searchSymbolTable(T->type_id);
      if (rtn == -1)
      {
        semantic_error(T->pos, T->type_id, "variable has not defined");
        T->error = 1;
      }
      if (symbolTable.symbols[rtn].flag == 'F')
      {
        semantic_error(T->pos, T->type_id, "is a function,type doesn't match.");
        T->error = 1;
      }
      else
      {
        opn1.kind = ID;
        strcpy(opn1.id, symbolTable.symbols[rtn].alias);
        opn1.offset = symbolTable.symbols[rtn].offset;
        opn2.kind = INT;
        opn2.const_int = 0;
        result.kind = ID;
        strcpy(result.id, T->Etrue);
        T->code = genIR(NEQ, opn1, opn2, result);
        T->code = merge(2, T->code, genGoto(T->Efalse));
      }
      T->width = 0;
      break;
    case RELOP: //处理关系运算表达式,2个操作数都按基本表达式处理
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      Exp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      opn1.kind = ID;
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
      opn2.kind = ID;
      strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
      opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
      result.kind = ID;
      strcpy(result.id, T->Etrue);
      T->code = genIR(getRelOpFromString(T->type_id), opn1, opn2, result);
      T->code = merge(4, T->ptr[0]->code, T->ptr[1]->code, T->code, genGoto(T->Efalse));
      break;
    case AND:
    case OR:
      if (T->kind == AND)
      {
        strcpy(T->ptr[0]->Etrue, newLabel());
        strcpy(T->ptr[0]->Efalse, T->Efalse);
      }
      else
      {
        strcpy(T->ptr[0]->Etrue, T->Etrue);
        strcpy(T->ptr[0]->Efalse, newLabel());
      }
      strcpy(T->ptr[1]->Etrue, T->Etrue);
      strcpy(T->ptr[1]->Efalse, T->Efalse);
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      boolExp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      boolExp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      if (T->kind == AND)
        T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
      else
        T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Efalse), T->ptr[1]->code);
      break;
    case NOT:
      strcpy(T->ptr[0]->Etrue, T->Efalse);
      strcpy(T->ptr[0]->Efalse, T->Etrue);
      boolExp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->code = T->ptr[0]->code;
      break;
    }
  }
}
//处理基本表达式，参考文献[2]p82的思想
void Exp(struct node *T)
{
  int rtn, num, width;
  struct node *T0;
  struct opn opn1, opn2, result;
  if (T)
  {
    switch (T->kind)
    {
    case ID: //查符号表，获得符号表中的位置，类型送type
      rtn = searchSymbolTable(T->type_id);
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "variable hasn't defined.");
      }
      if (symbolTable.symbols[rtn].flag == 'F')
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "is a function,type doesn't match.");
      }
      else
      {
        T->place = rtn; //结点保存变量在符号表中的位置
        T->code = NULL; //标识符不需要生成TAC
        T->type = symbolTable.symbols[rtn].type;
        T->offset = symbolTable.symbols[rtn].offset;
        T->width = 0; //未再使用新单元
      }
      break;
    case INT:
    case FLOAT:
    case CHAR:
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset); //为浮点常量生成一个临时变量
      opn1.kind = T->type;
      setCodeVal(opn1, T, T->type);
      result.kind = ID;
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      result.offset = symbolTable.symbols[T->place].offset;
      T->code = genIR(ASSIGNOP, opn1, opn2, result);
      T->width = getWidth(T->type);
      break;
    case ASSIGNOP:
      if (T->ptr[0]->kind != ID && T->ptr[0]->kind != ARR_ACCESS)
      {
        T->error = 1;
        semantic_error(T->pos, "", "assign expression needs a left value.");
      }
      else
      {
        Exp(T->ptr[0]); //处理左值，例中仅为变量
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->ptr[1]->offset = T->offset;
        Exp(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        if (T->ptr[1]->type != T->ptr[0]->type)
        {
          T->error = 1;
          semantic_error(T->pos, "", "type mismatched for assignment.");
        }
        T->type = T->ptr[0]->type;
        T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[1]->width);
        T->width = T->ptr[1]->width + getWidth(T->type);
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
        opn1.kind = ID;
        strcpy(opn1.id, symbolTable.symbols[T->ptr[1]->place].alias); //右值一定是个变量或临时变量
        opn1.offset = symbolTable.symbols[T->ptr[1]->place].offset;
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
        result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
      }
      break;
    case PLUSASSIGN:
    case MINUSASSIGN:
    case MULTASSIGN:
    case DIVASSIGN:
      if (T->ptr[0]->kind != ID && T->ptr[0]->kind != ARR_ACCESS)
      {
        T->error = 1;
        semantic_error(T->pos, "", "assign expression needs a left value.");
      }
      else
      {
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]);
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        Exp(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        if (T->ptr[1]->type != T->ptr[0]->type)
        {
          T->error = 1;
          semantic_error(T->pos, "", "type mismatched for assignment.");
        }
        T->type = T->ptr[0]->type;
        rtn = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[1]->width);
        T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[1]->width + getWidth(T->type));
        T->width = T->ptr[1]->width + getWidth(T->type) * 2;
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
        opn1.kind = ID;
        strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
        opn1.type = T->ptr[0]->type;
        opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        opn2.kind = ID;
        strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
        opn2.type = T->ptr[1]->type;
        opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[rtn].alias);
        result.type = T->type;
        result.offset = symbolTable.symbols[rtn].offset;
        T->code = merge(2, T->code, genIR(getOpFromAssign(T->kind), opn1, opn2, result));
        opn1.kind = ID;
        strcpy(opn1.id, symbolTable.symbols[rtn].alias); //右值一定是个变量或临时变量
        opn1.offset = symbolTable.symbols[rtn].offset;
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
        result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
      }
      break;
    case AND:   
    case OR:    
          T->type = INT;
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      Exp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->ptr[0]->type !=INT && T->ptr[1]->type!=INT)
      {
        semantic_error(T->pos, "", "type mismatched for logic operands.");
        T->error = 1;
      }
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
      opn1.kind = ID;
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      opn1.type = T->ptr[0]->type;
      opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
      opn2.kind = ID;
      strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
      opn2.type = T->ptr[1]->type;
      opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
      result.kind = ID;
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      result.type = T->type;
      result.offset = symbolTable.symbols[T->place].offset;
      T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(T->kind, opn1, opn2, result));
      T->width = T->ptr[0]->width + T->ptr[1]->width + getWidth(T->type);
      break;
    case RELOP: 
      T->type = INT;
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      Exp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->ptr[0]->type != T->ptr[1]->type)
      {
        semantic_error(T->pos, "", "type mismatched for operands.");
        T->error = 1;
      }
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
      opn1.kind = ID;
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      opn1.type = T->ptr[0]->type;
      opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
      opn2.kind = ID;
      strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
      opn2.type = T->ptr[1]->type;
      opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
      result.kind = ID;
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      result.type = T->type;
      result.offset = symbolTable.symbols[T->place].offset;
      T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(getRelOpFromString(T->type_id), opn1, opn2, result));
      T->width = T->ptr[0]->width + T->ptr[1]->width + getWidth(T->type);
      break;
    case PLUS:
    case MINUS:
    case STAR:
    case DIV:
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->ptr[1]->offset = T->offset + T->ptr[0]->width;
      Exp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      //判断T->ptr[0]，T->ptr[1]类型是否正确，可能根据运算符生成不同形式的代码，给T的type赋值
      //下面的类型属性计算，没有考虑错误处理情况
      if (T->ptr[0]->type != T->ptr[1]->type)
      {
        semantic_error(T->pos, "", "type mismatched for operands.");
        T->error = 1;
      }
      else
        T->type = T->ptr[0]->type;
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
      opn1.kind = ID;
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      opn1.type = T->ptr[0]->type;
      opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
      opn2.kind = ID;
      strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
      opn2.type = T->ptr[1]->type;
      opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
      result.kind = ID;
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      result.type = T->type;
      result.offset = symbolTable.symbols[T->place].offset;
      T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(T->kind, opn1, opn2, result));
      T->width = T->ptr[0]->width + T->ptr[1]->width + getWidth(T->type);
      break;
    case NOT: //TODO:
      break;
    case UMINUS: //TODO:
    case UPLUS:
    case DMINUS:
    case DPLUS:
    case FDPLUS:
    case FDMINUS:
      break;
    case ARR_ACCESS:
      break;
    case FUNC_CALL: //根据T->type_id查出函数的定义，如果语言中增加了实验教材的read，write需要单独处理一下
      rtn = searchSymbolTable(T->type_id);
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "function has not defined.");
        break;
      }
      if (symbolTable.symbols[rtn].flag != 'F')
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "isn't a function.");
        break;
      }
      T->type = symbolTable.symbols[rtn].type;
      width = getWidth(T->type); //存放函数返回值的单数字节数
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]); //处理所有实参表达式求值，及类型
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->width = T->ptr[0]->width + width; //累加上计算实参使用临时变量的单元数
        T->code = T->ptr[0]->code;
      }
      else
      {
        T->width = width;
        T->code = NULL;
      }
      match_param(rtn, T->ptr[0], T->pos); //处理所以参数的匹配
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T0 = T->ptr[0]; //处理参数列表的中间代码
      while (T0)
      {
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
        result.offset = symbolTable.symbols[T0->ptr[0]->place].offset;
        T->code = merge(2, T->code, genIR(ARG, opn1, opn2, result));
        T0 = T0->ptr[1];
      }
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->width - width);
      opn1.kind = ID;
      strcpy(opn1.id, T->type_id); //保存函数名
      opn1.offset = rtn;           //这里offset用以保存函数定义入口,在目标代码生成时，能获取相应信息
      result.kind = ID;
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      result.offset = symbolTable.symbols[T->place].offset;
      T->code = merge(2, T->code, genIR(CALL, opn1, opn2, result)); //生成函数调用中间代码
      break;
    case ARGS: //此处仅处理各实参表达式的求值的代码序列，不生成ARG的实参系列
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      T->code = T->ptr[0]->code;
      if (T->ptr[1])
      {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        Exp(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->width += T->ptr[1]->width;
        T->code = merge(2, T->code, T->ptr[1]->code);
      }
      break;
    }
  }
}

void semantic_Analysis(struct node *T)
{ //对抽象语法树的先根遍历,按display的控制结构修改完成符号表管理和语义检查和TAC生成（语句部分）
  int rtn, num, width, tempOffset;
  struct node *T0;
  struct opn opn1, opn2, result;
  if (T)
  {
    switch (T->kind)
    {
    case EXT_DEF_LIST:
      if (!T->ptr[0])
        break;
      T->ptr[0]->offset = T->offset;
      semantic_Analysis(T->ptr[0]); //访问外部定义列表中的第一个
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->code = T->ptr[0]->code;
      if (T->ptr[1])
      {
        T->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]); //访问该外部定义列表中的其它外部定义
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->code = merge(2, T->code, T->ptr[1]->code);
      }
      break;
    case EXT_VAR_DEF: //处理外部说明,将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
      T->type = T->ptr[1]->type = getType(T->ptr[0]->type_id);
      T->ptr[1]->offset = T->offset;        //这个外部变量的偏移量向下传递
      T->ptr[1]->width = getWidth(T->type); //将一个变量的宽度向下传递
      ext_var_list(T->ptr[1]);              //处理外部变量说明中的标识符序列
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      T->width = getWidth(T->type) * T->ptr[1]->num; //计算这个外部变量说明的宽度
      T->code = NULL;                                //这里假定外部变量不支持初始化
      break;
    case FUNC_DEF: //填写函数定义信息到符号表
      if (T->ptr[0])
        T->ptr[1]->type = T->ptr[0]->type; //获取函数返回类型送到含函数名、参数的结点
      else
        T->ptr[1]->type = VOID;
      T->width = 0; //函数的宽度设置为0，不会对外部变量的地址分配产生影响
      tempOffset = T->offset;
      T->offset = DX;               //设置局部变量在活动记录中的偏移量初值
      semantic_Analysis(T->ptr[1]); //处理函数名和参数结点部分，这里不考虑用寄存器传递参数
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      T->place = T->ptr[1]->place;
      T->offset += T->ptr[1]->width; //用形参单元宽度修改函数局部变量的起始偏移量
      T->ptr[2]->offset = T->offset;
      strcpy(T->ptr[2]->Snext, newLabel()); //函数体语句执行结束后的位置属性
      semantic_Analysis(T->ptr[2]);         //处理函数体结点
      T->error = T->ptr[2]->error ? T->ptr[2]->error : T->error;
      //计算活动记录大小,这里offset属性存放的是活动记录大小，不是偏移
      symbolTable.symbols[T->ptr[1]->place].offset = T->offset + T->ptr[2]->width;
      T->code = merge(3, T->ptr[1]->code, T->ptr[2]->code, genLabel(T->ptr[2]->Snext)); //函数体的代码作为函数的代码
      T->offset = tempOffset;
      break;
    case FUNC_DEC:                                                         //根据返回类型，函数名填写符号表
      rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'F', 0); //函数不在数据区中分配单元，偏移量为0
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "same function has defined.");
        break;
      }
      else
        T->place = rtn;
      result.kind = ID;
      strcpy(result.id, T->type_id);
      result.offset = rtn;
      T->code = genIR(FUNCTION, opn1, opn2, result); //生成中间代码：FUNCTION 函数名
      T->offset = DX;                                //设置形式参数在活动记录中的偏移量初值
      if (T->ptr[0])
      { //判断是否有参数
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //处理函数参数列表
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->width = T->ptr[0]->width;
        symbolTable.symbols[rtn].paramnum = T->ptr[0]->num;
        T->code = merge(2, T->code, T->ptr[0]->code); //连接函数名和参数代码序列
      }
      else
        symbolTable.symbols[rtn].paramnum = 0, T->width = 0;
      break;
    case PARAM_LIST: //处理函数形式参数列表
      T->ptr[0]->offset = T->offset;
      semantic_Analysis(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      if (T->ptr[1])
      {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->num = T->ptr[0]->num + T->ptr[1]->num;             //统计参数个数
        T->width = T->ptr[0]->width + T->ptr[1]->width;       //累加参数单元宽度
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code); //连接参数代码
      }
      else
      {
        T->num = T->ptr[0]->num;
        T->width = T->ptr[0]->width;
        T->code = T->ptr[0]->code;
      }
      break;
    case PARAM_DEC:
      rtn = fillSymbolTable(T->ptr[1]->type_id, newAlias(), 1, T->ptr[0]->type, 'P', T->offset);
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T->ptr[1]->pos, T->ptr[1]->type_id, "same parameter has defined.");
      }
      else
      {
        T->ptr[1]->place = rtn;
        T->num = 1;                           //参数个数计算的初始值
        T->width = getWidth(T->ptr[0]->type); //参数宽度
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[rtn].alias);
        result.offset = T->offset;
        T->code = genIR(PARAM, opn1, opn2, result); //生成：FUNCTION 函数名
      }
      break;
    case COMP_STM:
      LEV++;
      //设置层号加1，并且保存该层局部变量在符号表中的起始位置在symbol_scope_TX
      symbol_scope_TX.TX[symbol_scope_TX.top++] = symbolTable.index;
      T->width = 0;
      T->code = NULL;
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //处理该层的局部变量DEF_LIST
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->width += T->ptr[0]->width;
        T->code = T->ptr[0]->code;
      }
      if (T->ptr[1])
      {
        T->ptr[1]->Sbreak = T->Sbreak;
        T->ptr[1]->offset = T->offset + T->width;
        strcpy(T->ptr[1]->Snext, T->Snext); //S.next属性向下传递
        semantic_Analysis(T->ptr[1]);       //处理复合语句的语句序列
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->width += T->ptr[1]->width;
        T->code = merge(2, T->code, T->ptr[1]->code);
      }
      prn_symbol();                                                  //c在退出一个符合语句前显示的符号表
      LEV--;                                                         //出复合语句，层号减1
      symbolTable.index = symbol_scope_TX.TX[--symbol_scope_TX.top]; //删除该作用域中的符号
      break;
    case DEF_LIST:
      T->code = NULL;
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //处理一个局部变量定义
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->code = T->ptr[0]->code;
        T->width = T->ptr[0]->width;
      }
      if (T->ptr[1])
      {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]); //处理剩下的局部变量定义
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->code = merge(2, T->code, T->ptr[1]->code);
        T->width += T->ptr[1]->width;
      }
      break;
    case VAR_DEF: //处理一个局部变量定义,将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
      local_var_list(T);
      break;
    case STM_LIST:
      if (!T->ptr[0])
      {
        T->code = NULL;
        T->width = 0;
        break;
      }              //空语句序列
      if (T->ptr[1]) //2条以上语句连接，生成新标号作为第一条语句结束后到达的位置
        strcpy(T->ptr[0]->Snext, newLabel());
      else //语句序列仅有一条语句，S.next属性向下传递
        strcpy(T->ptr[0]->Snext, T->Snext);
      T->ptr[0]->offset = T->offset;
      T->ptr[0]->Sbreak = T->Sbreak;
      semantic_Analysis(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->code = T->ptr[0]->code;
      T->width = T->ptr[0]->width;
      if (T->ptr[1])
      {
        strcpy(T->ptr[1]->Snext, T->Snext); //2条以上语句连接,S.next属性向下传递
        T->ptr[1]->offset = T->offset;      //顺序结构共享单元方式
        // T->ptr[1]->offset = T->offset + T->ptr[0]->width; //顺序结构顺序分配单元方式
        semantic_Analysis(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        //序列中第1条为表达式语句，返回语句，复合语句时，第2条前不需要标号
        if (T->ptr[0]->kind == RETURN || T->ptr[0]->kind == EXP_STMT || T->ptr[0]->kind == COMP_STM)
          T->code = merge(2, T->code, T->ptr[1]->code);
        else
          T->code = merge(3, T->code, genLabel(T->ptr[0]->Snext), T->ptr[1]->code);
        if (T->ptr[1]->width > T->width)
          T->width = T->ptr[1]->width; //顺序结构共享单元方式
                                       //T->width+=T->ptr[1]->width;//顺序结构顺序分配单元方式
      }
      break;
    case IF_THEN:
      strcpy(T->ptr[0]->Etrue, newLabel()); //设置条件语句真假转移位置
      strcpy(T->ptr[0]->Efalse, T->Snext);
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      boolExp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      strcpy(T->ptr[1]->Snext, T->Snext);
      semantic_Analysis(T->ptr[1]); //if子句
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
      break; //控制语句都还没有处理offset和width属性
    case IF_THEN_ELSE:
      strcpy(T->ptr[0]->Etrue, newLabel()); //设置条件语句真假转移位置
      strcpy(T->ptr[0]->Efalse, newLabel());
      T->ptr[0]->offset = T->ptr[1]->offset = T->ptr[2]->offset = T->offset;
      boolExp(T->ptr[0]); //条件，要单独按短路代码处理
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      strcpy(T->ptr[1]->Snext, T->Snext);
      semantic_Analysis(T->ptr[1]); //if子句
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      strcpy(T->ptr[2]->Snext, T->Snext);
      semantic_Analysis(T->ptr[2]); //else子句
      T->error = T->ptr[2]->error ? T->ptr[2]->error : T->error;

      if (T->width < T->ptr[2]->width)
        T->width = T->ptr[2]->width;
      T->code = merge(6, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code,
                      genGoto(T->Snext), genLabel(T->ptr[0]->Efalse), T->ptr[2]->code);
      break;
    case WHILE:
      strcpy(T->ptr[0]->Etrue, newLabel()); //子结点继承属性的计算
      strcpy(T->ptr[0]->Efalse, T->Snext);
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      boolExp(T->ptr[0]); //循环条件，要单独按短路代码处理
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      strcpy(T->ptr[1]->Snext, newLabel());
      T->ptr[1]->Sbreak = 1;
      semantic_Analysis(T->ptr[1]); //循环体
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      T->code = merge(5, genLabel(T->ptr[1]->Snext), T->ptr[0]->code,
                      genLabel(T->ptr[0]->Etrue), T->ptr[1]->code, genGoto(T->ptr[1]->Snext));
      break;
    case FOR:
      break;
    case EXP_STMT:
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]);
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->code = T->ptr[0]->code;
        T->width = T->ptr[0]->width;
      }
      break;
    case RETURN:
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]);
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        num = symbolTable.index;
        do
          num--;
        while (symbolTable.symbols[num].flag != 'F');
        if (T->ptr[0]->type != symbolTable.symbols[num].type)
        {
          semantic_error(T->pos, "type mismatched for return. ", "");
          T->error = 1;
          T->width = 0;
          T->code = NULL;
          break;
        }
        T->width = T->ptr[0]->width;
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
        result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        T->code = merge(2, T->ptr[0]->code, genIR(RETURN, opn1, opn2, result));
      }
      else
      {
        T->width = 0;
        result.kind = 0;
        T->code = genIR(RETURN, opn1, opn2, result);
      }
      break;
    case BREAK:
      if (!T->Sbreak)
      {
        T->error = 1;
        semantic_error(T->pos, " break can only be used in a loop", "");
      }
      break;
    case ID:
    case INT:
    case FLOAT:
    case CHAR:
    case ASSIGNOP:
    case PLUSASSIGN:
    case MINUSASSIGN:
    case MULTASSIGN:
    case DIVASSIGN:
    case AND:
    case OR:
    case RELOP:
    case PLUS:
    case MINUS:
    case STAR:
    case DIV:
    case NOT:
    case UMINUS:
    case UPLUS:
    case DMINUS:
    case DPLUS:
    case FDPLUS:
    case FDMINUS:
    case ARR_ACCESS:
    case FUNC_CALL:
      Exp(T); //处理基本表达式
      break;
    }
  }
}

void semantic_Analysis0(struct node *T)
{
  symbolTable.index = 0;
  fillSymbolTable("read", "", 0, INT, 'F', 4);
  symbolTable.symbols[0].paramnum = 0; //read的形参个数
  fillSymbolTable("write", "", 0, INT, 'F', 4);
  fillSymbolTable("x", "", 1, INT, 'P', DX);
  symbolTable.symbols[1].paramnum = 1;
  symbol_scope_TX.TX[0] = 0; //外部变量在符号表中的起始序号为0
  symbol_scope_TX.top = 1;
  T->offset = 0; //外部变量在数据区的偏移量
  semantic_Analysis(T);
  printf("%d", T->error);
  //prnIR(T->code);
}