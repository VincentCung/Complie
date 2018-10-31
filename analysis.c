#include "def.h"

int LEV = 0; //���

void semantic_error(int line, char *msg1, char *msg2)
{
  //�������ֻ�ռ�������Ϣ�������һ����ʾ
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

void setCodeVal(struct opn *src, struct node *T, enum node_kind type)
{
  switch (type)
  {
  case INT:
    src->const_int = T->type_int;
    break;
  case FLOAT:
    src->const_float = T->type_float;
    break;
  case CHAR:
    src->const_char = T->type_id[0];
    break;
  default:
    break;
  }
}

void setFalsyVal(struct opn *src, enum node_kind type)
{
  switch (type)
  {
  case INT:
    src->const_int = 0;
    break;
  case FLOAT:
    src->const_float = 0.0;
    break;
  case CHAR:
    src->const_char = '\0';
    break;
  default:
    break;
  }
}

void setOpn(struct opn *src, int kind, int type, int offset)
{
  src->kind = kind;
  src->offset = offset;
  src->type = type;
}

void mergeLogicOp(struct node *T, struct codenode *in, char *label, int originVal)
{
  struct opn opn1, opn2, result;
  setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
  strcpy(result.id, symbolTable.symbols[T->place].alias);
  opn1.kind = T->type;
  opn1.const_int = originVal ? 1 : 0;
  if (T->ptr[1])
    T->code = merge(4, T->ptr[0]->code, T->ptr[1]->code, genIR(ASSIGNOP, opn1, opn2, result), in);
  else
    T->code = merge(3, T->ptr[0]->code, genIR(ASSIGNOP, opn1, opn2, result), in);
  opn1.const_int = originVal ? 0 : 1;
  T->code = merge(3, T->code, genIR(ASSIGNOP, opn1, opn2, result), genLabel(label));
}

//����һ��TAC����Ľ����ɵ�˫��ѭ����������ͷָ��
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

//����һ�������䣬����ͷָ��
struct codenode *genLabel(char *label)
{
  struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
  h->op = LABEL;
  strcpy(h->result.id, label);
  h->next = h->prior = h;
  return h;
}

//����GOTO��䣬����ͷָ��
struct codenode *genGoto(char *label)
{
  struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
  h->op = GOTO;
  strcpy(h->result.id, label);
  h->next = h->prior = h;
  return h;
}

//�ϲ�����м�����˫��ѭ��������β����
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

//����м����
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
    if (h->opn1.kind == CHAR)
      sprintf(opnstr1, "#'%c'", h->opn1.const_char);
    if (h->opn1.kind == ID)
      sprintf(opnstr1, "%s", h->opn1.id);
    if (h->opn2.kind == INT)
      sprintf(opnstr2, "#%d", h->opn2.const_int);
    if (h->opn2.kind == FLOAT)
      sprintf(opnstr2, "#%f", h->opn2.const_float);
    if (h->opn2.kind == CHAR)
      sprintf(opnstr2, "#'%c'", h->opn2.const_char);
    if (h->opn2.kind == ID)
      sprintf(opnstr2, "%s", h->opn2.id);
    sprintf(resultstr, "%s", h->result.id);
    switch (h->op)
    {
    case ASSIGNOP:
      printf("  %s := %s\n", resultstr, opnstr1);
      break;
    case ARR_ACCESS:
      printf("  %s := %s [ %s ]\n", resultstr, opnstr1, opnstr2);
      break;
    case ARR_INIT:
      printf("  %s [ %s ] := %s\n", resultstr, opnstr1, opnstr2);
      break;
    case PLUS:
    case MINUS:
    case MULT:
    case DIV:
      printf("  %s := %s %c %s\n", resultstr, opnstr1,
             h->op == PLUS ? '+' : h->op == MINUS ? '-' : h->op == MULT ? '*' : '/', opnstr2);
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
  //���ȸ���name����ű������ظ����� �ظ����巵��-1
  int i;
  /*���Ų��أ������ⲿ��������ǰ�к������壬
    ���β������ڷ��ű��У���ʱ���ⲿ������ǰ�������β������������*/
  for (i = symbolTable.index - 1; symbolTable.symbols[i].level == level || (level == 0 && i >= 0); i--)
  {
    if (level == 0 && symbolTable.symbols[i].level == 1)
      continue; //�ⲿ�������ββ��رȽ�����
    if (!strcmp(symbolTable.symbols[i].name, name))
      return -1;
  }
  //��д���ű�����
  strcpy(symbolTable.symbols[symbolTable.index].name, name);
  strcpy(symbolTable.symbols[symbolTable.index].alias, alias);
  symbolTable.symbols[symbolTable.index].level = level;
  symbolTable.symbols[symbolTable.index].type = type;
  symbolTable.symbols[symbolTable.index].flag = flag;
  symbolTable.symbols[symbolTable.index].offset = offset;
  return symbolTable.index++; //���ص��Ƿ����ڷ��ű��е�λ����ţ��м��������ʱ�������ȡ�����ű���
}

//��д��ʱ���������ű�������ʱ�����ڷ��ű��е�λ��
int fill_Temp(char *name, int level, int type, char flag, int offset)
{
  strcpy(symbolTable.symbols[symbolTable.index].name, "");
  strcpy(symbolTable.symbols[symbolTable.index].alias, name);
  symbolTable.symbols[symbolTable.index].level = level;
  symbolTable.symbols[symbolTable.index].type = type;
  symbolTable.symbols[symbolTable.index].flag = flag;
  symbolTable.symbols[symbolTable.index].offset = offset;
  return symbolTable.index++; //���ص�����ʱ�����ڷ��ű��е�λ�����
}

void ext_var_list(struct node *T) //�����ⲿ�����б�
{
  int rtn, num = 1;
  switch (T->kind)
  {
  case EXT_DEC_LIST:
    T->ptr[0]->type = T->type;     //�������������´��ݱ������
    T->ptr[0]->offset = T->offset; //�ⲿ������ƫ�������´���
    ext_var_list(T->ptr[0]);
    T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
    T->ptr[1]->type = T->type;                //�������������´��ݱ������
    T->ptr[1]->offset = T->offset + T->width; //�ⲿ������ƫ�������´���
    T->ptr[1]->width = T->width;
    ext_var_list(T->ptr[1]);
    T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
    T->num = T->ptr[1]->num + T->ptr[0]->num;
    break;
  case ARR_INIT:
    rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'A', T->offset); //���һ��������
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
    rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'V', T->offset); //���һ��������
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
  struct node *T0 = T->ptr[1]; //T0Ϊ�������б�������ָ�룬��ID��ASSIGNOP�����ڵǼǵ����ű���Ϊ�ֲ�����
  struct node *T1;
  int width = getWidth(T->ptr[0]->type); //һ���������
  T->code = NULL;
  T->ptr[1]->type = T->ptr[0]->type; //ȷ���������и���������
  T->width = 0;
  T0->offset = T->offset;
  while (T0)
  {                                  //��������DEC_LIST���
    T0->ptr[0]->type = T0->type;     //�����������´���
    T0->ptr[0]->offset = T0->offset; //�����������´���
    if (T0->ptr[1])
    {
      T0->ptr[1]->type = T0->type;
      T0->ptr[1]->offset = T0->offset + width * T0->num;
    }
    if (T0->ptr[0]->kind == ID)
    {
      rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
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
    else if (T0->ptr[0]->kind == ARR_INIT)
    {
      rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'A', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
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
      rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
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
        strcpy(opn1.id, symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
        setOpn(&opn1,ID,T0->ptr[0]->type,symbolTable.symbols[T0->ptr[0]->ptr[1]->place].offset);
        strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
        setOpn(&result,ID,T0->ptr[0]->type,symbolTable.symbols[T0->ptr[0]->place].offset);
        T->code = merge(3, T->code, T0->ptr[0]->ptr[1]->code, genIR(ASSIGNOP, opn1, opn2, result));
      }
      T->num = T0->ptr[0]->ptr[0]->num;
      T->width += width + T0->ptr[0]->ptr[1]->width;
    }
    else if (T0->ptr[0]->kind == ASSIGNOP && T0->ptr[0]->ptr[0]->kind == ARR_INIT)
    {
      rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'A', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
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
        if (T1)
          T1->ptr[0]->offset = T->offset + T->width + width;
        while (T1)
        {
          if (T1->ptr[0]->type != T0->type)
          {
            T->error = 1;
            semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "define invalid type variable.");
          }
          strcpy(result.id, symbolTable.symbols[rtn].alias);
          setOpn(&result, ID, T0->type, symbolTable.symbols[rtn].offset);
          opn1.const_int = T0->ptr[0]->ptr[1]->num;
          setOpn(&opn1, INT, INT, 0);
          setCodeVal(&opn2, T1->ptr[0], T0->type);
          setOpn(&opn2, T0->type, T0->type, 0);
          T->code = merge(2, T->code, genIR(ARR_INIT, opn1, opn2, result));
          T0->ptr[0]->ptr[1]->num++;
          T1 = T1->ptr[1];
        }
        if (T0->ptr[0]->ptr[1] && T0->ptr[0]->ptr[1]->num > T0->ptr[0]->ptr[0]->num)
        {
          T->error = 1;
          semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "too much variables.");
        }
      }
      T->num = T0->ptr[0]->ptr[0]->num;
      T->width += width * T->num;
    }
    T0 = T0->ptr[1];
  }
}

//ƥ�������������
int match_param(int i, struct node *T, struct node *parent)
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
      parent->error = 1;
      semantic_error(parent->pos, "", "function's parameters are too few.");
      return 0;
    }
    type1 = symbolTable.symbols[i + j].type; //�β�����
    type2 = T->ptr[0]->type;
    if (type1 != type2)
    {
      parent->error = 1;
      semantic_error(parent->pos, "", "parameter doesn't match.");
      return 0;
    }
    T = T->ptr[1];
  }
  if (T)
  { //num�������Ѿ�ƥ���꣬����ʵ�α��ʽ
    parent->error = 1;
    semantic_error(parent->pos, "", "function's parameters are too many.");
    return 0;
  }
  return 1;
}
//�������ʽ���ο�����[2]p84��˼��
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
    case ID: //����ű���÷��ű��е�λ�ã�������type
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
    case RELOP: //�����ϵ������ʽ,2�������������������ʽ����
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
//����������ʽ���ο�����[2]p82��˼��
void Exp(struct node *T)
{
  int rtn, num, width;
  struct node *T0;
  struct opn opn1, opn2, result;
  char *label;
  struct codenode *code;
  if (T)
  {
    switch (T->kind)
    {
    case ID: //����ű���÷��ű��е�λ�ã�������type
      rtn = searchSymbolTable(T->type_id);
      if (rtn == -1)
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "variable hasn't defined.");
      }
      if (symbolTable.symbols[rtn].flag == 'A' && T->dimension == 0)
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "is a array,expression must be a modifiable lvalue.");
      }
      if (symbolTable.symbols[rtn].flag == 'F')
      {
        T->error = 1;
        semantic_error(T->pos, T->type_id, "is a function,type doesn't match.");
      }
      else
      {
        T->place = rtn; //��㱣������ڷ��ű��е�λ��
        T->code = NULL; //��ʶ������Ҫ����TAC
        T->type = symbolTable.symbols[rtn].type;
        T->offset = symbolTable.symbols[rtn].offset;
        T->width = 0; //δ��ʹ���µ�Ԫ
      }
      break;
    case INT:
    case FLOAT:
    case CHAR:
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset); //Ϊ���㳣������һ����ʱ����
      opn1.kind = T->type;
      setCodeVal(&opn1, T, T->type);
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
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
        Exp(T->ptr[0]); //������ֵ�����н�Ϊ����
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
        strcpy(opn1.id, symbolTable.symbols[T->ptr[1]->place].alias); //��ֵһ���Ǹ���������ʱ����
        setOpn(&opn1, ID, T->ptr[1]->type, symbolTable.symbols[T->ptr[1]->place].offset);
        strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
        setOpn(&result, ID, T->ptr[1]->type, symbolTable.symbols[T->ptr[0]->place].offset);
        T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
        strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
        setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
        strcpy(result.id, symbolTable.symbols[T->place].alias);
        setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
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
        strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
        setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
        strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
        setOpn(&opn2, ID, T->ptr[1]->type, symbolTable.symbols[T->ptr[1]->place].offset);
        strcpy(result.id, symbolTable.symbols[rtn].alias);
        setOpn(&result, ID, T->type, symbolTable.symbols[rtn].offset);
        T->code = merge(2, T->code, genIR(getOpFromAssign(T->kind), opn1, opn2, result));
        strcpy(opn1.id, symbolTable.symbols[rtn].alias); //��ֵһ���Ǹ���������ʱ����
        opn1.offset = symbolTable.symbols[rtn].offset;
        strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
        result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
        strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias); //��ֵһ���Ǹ���������ʱ����
        setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
        strcpy(result.id, symbolTable.symbols[T->place].alias);
        setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
        T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
      }
      break;
    case AND:
    case OR:
      T->type = INT;
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->ptr[1]->offset = T->offset + T->ptr[0]->width;
      Exp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->ptr[0]->type != INT || T->ptr[1]->type != INT)
      {
        semantic_error(T->pos, "", "type mismatched for logic operands.");
        T->error = 1;
      }
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
      T->width = T->ptr[0]->width + T->ptr[1]->width + getWidth(T->type);
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
      opn2.const_int = 0;
      setOpn(&opn2, INT, INT, 0);
      label = newLabel();
      strcpy(result.id, label);
      setOpn(&result, ID, T->type, 0);
      code = genIR(getRelOpFromString(T->kind == OR ? "!=" : "=="), opn1, opn2, result);
      strcpy(opn1.id, symbolTable.symbols[T->ptr[1]->place].alias);
      setOpn(&opn1, ID, T->ptr[1]->type, symbolTable.symbols[T->ptr[1]->place].offset);
      code = merge(2, code, genIR(getRelOpFromString(T->kind == OR ? "!=" : "=="), opn1, opn2, result));
      mergeLogicOp(T, code, label, T->kind == OR ? 1 : 0);
      break;
    case RELOP:
      T->type = INT;
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->ptr[1]->offset = T->offset + T->ptr[0]->width;
      Exp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->ptr[0]->type != T->ptr[1]->type)
      {
        semantic_error(T->pos, "", "type mismatched for logic operands.");
        T->error = 1;
      }
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
      T->width = T->ptr[0]->width + T->ptr[1]->width + getWidth(T->type);
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
      strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
      setOpn(&opn2, ID, T->ptr[1]->type, symbolTable.symbols[T->ptr[1]->place].offset);
      label = newLabel();
      strcpy(result.id, label);
      setOpn(&result, ID, T->type, 0);
      mergeLogicOp(T, genIR(getRelOpFromString(T->type_id), opn1, opn2, result), label, 1);
      break;
    case PLUS:
    case MINUS:
    case MULT:
    case DIV:
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->ptr[1]->offset = T->offset + T->ptr[0]->width;
      Exp(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->ptr[0]->type != T->ptr[1]->type)
      {
        semantic_error(T->pos, "", "type mismatched for operands.");
        T->error = 1;
      }
      else
        T->type = T->ptr[0]->type;
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
      T->width = T->ptr[0]->width + T->ptr[1]->width + getWidth(T->type);
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
      strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
      setOpn(&opn2, ID, T->ptr[1]->type, symbolTable.symbols[T->ptr[1]->place].offset);
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
      T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(T->kind, opn1, opn2, result));
      break;
    case NOT:
      T->type = INT;
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]); //������ֵ�����н�Ϊ����
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      if (T->ptr[0]->type != INT)
      {
        T->error = 1;
        semantic_error(T->pos, "", "type mismatched for logic operands.");
      }
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width);
      T->width = T->ptr[0]->width + getWidth(T->type);
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
      opn2.const_int = 0;
      setOpn(&opn2, INT, INT, symbolTable.symbols[T->place].offset);
      label = newLabel();
      strcpy(result.id, label);
      setOpn(&result, ID, T->type, 0);
      mergeLogicOp(T, genIR(getRelOpFromString("!="), opn1, opn2, result), label, 1);
      break;
    case UMINUS:
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]); //������ֵ�����н�Ϊ����
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->type = T->ptr[0]->type;
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width);
      T->width = T->ptr[0]->width + getWidth(T->type);
      opn1.const_int = 0;
      setOpn(&opn1, INT, INT, 0);
      strcpy(opn2.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&opn2, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
      T->code = merge(2, T->ptr[0]->code, genIR(MINUS, opn1, opn2, result));
      break;
    case DMINUS:
    case DPLUS:
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]);
      T->type = T->ptr[0]->type;
      if (T->ptr[0]->kind != ID && T->ptr[0]->kind != ARR_ACCESS)
      {
        semantic_error(T->pos, "", "only one varible can do this operands.");
        T->error = 1;
      }
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width);
      T->width = T->ptr[0]->width + getWidth(T->type);
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
      T->code = merge(2, T->ptr[0]->code, genIR(ASSIGNOP, opn1, opn2, result));
      opn2.const_int = 1;
      setOpn(&opn2, INT, INT, 0);
      strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&result, ID, T->type, symbolTable.symbols[T->ptr[0]->place].offset);
      T->code = merge(2, T->code, genIR(T->kind == DPLUS ? PLUS : MINUS, opn1, opn2, result));
      break;
    case FDPLUS:
    case FDMINUS:
      T->ptr[0]->offset = T->offset;
      Exp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->type = T->ptr[0]->type;
      if (T->ptr[0]->kind != ID && T->ptr[0]->kind != ARR_ACCESS)
      {
        semantic_error(T->pos, "", "only one varible can do this operands.");
        T->error = 1;
      }
      T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width);
      T->width = T->ptr[0]->width + getWidth(T->type);
      strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
      setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
      opn2.const_int = 1;
      setOpn(&opn2, INT, INT, 0);
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
      T->code = merge(2, T->ptr[0]->code, genIR(T->kind == FDPLUS ? PLUS : MINUS, opn1, opn2, result));
      break;
    case ARR_ACCESS:
      if (!T->ptr[0] || T->ptr[0]->kind != ID && T->ptr[0]->kind != ARR_ACCESS)
      {
        semantic_error(T->pos, "", "can't access this field.");
        T->error = 1;
      }
      else
      {
        if (T->ptr[0]->kind == ID)
        {
          rtn = searchSymbolTable(T->ptr[0]->type_id);
          if (symbolTable.symbols[rtn].flag != 'A')
          {
            T->error = 1;
            semantic_error(T->pos, T->ptr[0]->type_id, "is not a array, can't access it.");
          }
          T->ptr[0]->level = 0;
          T->ptr[0]->dimension = symbolTable.symbols[rtn].dimension;
          memcpy(T->ptr[0]->arrayLimit, symbolTable.symbols[rtn].arrayLimit, T->ptr[0]->dimension);
        }
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]);
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->level = T->ptr[0]->level + 1;
        T->type = T->ptr[0]->type;
        T->dimension = T->ptr[0]->dimension;
        memcpy(T->arrayLimit, T->ptr[0]->arrayLimit, T->dimension);
        if (T->level > T->dimension)
        {
          T->error = 1;
          semantic_error(T->pos, "", "access is out of the field.");
        }
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        Exp(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        if (T->ptr[1]->type != INT)
        {
          T->error = 1;
          semantic_error(T->pos, "", "it is not a integer, can't access the array.");
        }
        strcpy(T->type_id, T->ptr[0]->type_id);
        T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
        T->width = T->ptr[0]->width + T->ptr[1]->width + getWidth(T->type);
        strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
        setOpn(&opn1, ID, T->ptr[0]->type, symbolTable.symbols[T->ptr[0]->place].offset);
        strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
        setOpn(&opn2, ID, T->ptr[1]->type, symbolTable.symbols[T->ptr[1]->place].offset);
        strcpy(result.id, symbolTable.symbols[T->place].alias);
        setOpn(&result, ID, T->type, symbolTable.symbols[T->place].offset);
        T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(ARR_ACCESS, opn1, opn2, result));
        break;
      }
      break;
    case FUNC_CALL: //����T->type_id��������Ķ��壬���������������ʵ��̲ĵ�read��write��Ҫ��������һ��
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
      width = getWidth(T->type); //��ź�������ֵ�ĵ����ֽ���
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]); //��������ʵ�α��ʽ��ֵ��������
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->width = T->ptr[0]->width + width; //�ۼ��ϼ���ʵ��ʹ����ʱ�����ĵ�Ԫ��
        T->code = T->ptr[0]->code;
      }
      else
      {
        T->width = width;
        T->code = NULL;
      }
      match_param(rtn, T->ptr[0], T); //�������Բ�����ƥ��
      T0 = T->ptr[0];                 //��������б���м����
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
      strcpy(opn1.id, T->type_id); //���溯����
      opn1.offset = rtn;           //����offset���Ա��溯���������,��Ŀ���������ʱ���ܻ�ȡ��Ӧ��Ϣ
      result.kind = ID;
      strcpy(result.id, symbolTable.symbols[T->place].alias);
      result.offset = symbolTable.symbols[T->place].offset;
      T->code = merge(2, T->code, genIR(CALL, opn1, opn2, result)); //���ɺ��������м����
      break;
    case ARGS: //�˴��������ʵ�α��ʽ����ֵ�Ĵ������У�������ARG��ʵ��ϵ��
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
{ //�Գ����﷨�����ȸ�����,��display�Ŀ��ƽṹ�޸���ɷ��ű������������TAC���ɣ���䲿�֣�
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
      semantic_Analysis(T->ptr[0]); //�����ⲿ�����б��еĵ�һ��
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->code = T->ptr[0]->code;
      if (T->ptr[1])
      {
        T->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]); //���ʸ��ⲿ�����б��е������ⲿ����
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->code = merge(2, T->code, T->ptr[1]->code);
      }
      break;
    case EXT_VAR_DEF: //�����ⲿ˵��,����һ������(TYPE���)�е������͵��ڶ������ӵ�������
      T->type = T->ptr[1]->type = getType(T->ptr[0]->type_id);
      T->ptr[1]->offset = T->offset;        //����ⲿ������ƫ�������´���
      T->ptr[1]->width = getWidth(T->type); //��һ�������Ŀ�����´���
      ext_var_list(T->ptr[1]);              //�����ⲿ����˵���еı�ʶ������
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      T->width = getWidth(T->type) * T->ptr[1]->num; //��������ⲿ����˵���Ŀ��
      T->code = NULL;                                //����ٶ��ⲿ������֧�ֳ�ʼ��
      break;
    case FUNC_DEF: //��д����������Ϣ�����ű�
      if (T->ptr[0])
        T->ptr[1]->type = T->ptr[0]->type; //��ȡ�������������͵����������������Ľ��
      else
        T->ptr[1]->type = VOID;
      T->width = 0; //�����Ŀ������Ϊ0��������ⲿ�����ĵ�ַ�������Ӱ��
      tempOffset = T->offset;
      T->offset = DX;               //���þֲ������ڻ��¼�е�ƫ������ֵ
      semantic_Analysis(T->ptr[1]); //���������Ͳ�����㲿�֣����ﲻ�����üĴ������ݲ���
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      T->place = T->ptr[1]->place;
      T->offset += T->ptr[1]->width; //���βε�Ԫ����޸ĺ����ֲ���������ʼƫ����
      T->ptr[2]->offset = T->offset;
      strcpy(T->ptr[2]->Snext, newLabel()); //���������ִ�н������λ������
      T->ptr[2]->error = T->error;
      semantic_Analysis(T->ptr[2]); //����������
      T->error = T->ptr[2]->error ? T->ptr[2]->error : T->error;
      //������¼��С,����offset���Դ�ŵ��ǻ��¼��С������ƫ��
      symbolTable.symbols[T->ptr[1]->place].offset = T->offset + T->ptr[2]->width;
      T->code = merge(3, T->ptr[1]->code, T->ptr[2]->code, genLabel(T->ptr[2]->Snext)); //������Ĵ�����Ϊ�����Ĵ���
      T->offset = tempOffset;
      break;
    case FUNC_DEC:                                                         //���ݷ������ͣ���������д���ű�
      rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'F', 0); //���������������з��䵥Ԫ��ƫ����Ϊ0
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
      T->code = genIR(FUNCTION, opn1, opn2, result); //�����м���룺FUNCTION ������
      T->offset = DX;                                //������ʽ�����ڻ��¼�е�ƫ������ֵ
      if (T->ptr[0])
      { //�ж��Ƿ��в���
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //�����������б�
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->width = T->ptr[0]->width;
        symbolTable.symbols[rtn].paramnum = T->ptr[0]->num;
        T->code = merge(2, T->code, T->ptr[0]->code); //���Ӻ������Ͳ�����������
      }
      else
        symbolTable.symbols[rtn].paramnum = 0, T->width = 0;
      break;
    case PARAM_LIST: //��������ʽ�����б�
      T->ptr[0]->offset = T->offset;
      semantic_Analysis(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      if (T->ptr[1])
      {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->num = T->ptr[0]->num + T->ptr[1]->num;             //ͳ�Ʋ�������
        T->width = T->ptr[0]->width + T->ptr[1]->width;       //�ۼӲ�����Ԫ���
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code); //���Ӳ�������
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
        T->num = 1;                           //������������ĳ�ʼֵ
        T->width = getWidth(T->ptr[0]->type); //�������
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[rtn].alias);
        result.offset = T->offset;
        T->code = genIR(PARAM, opn1, opn2, result); //���ɣ�FUNCTION ������
      }
      break;
    case COMP_STM:
      LEV++;
      //���ò�ż�1�����ұ���ò�ֲ������ڷ��ű��е���ʼλ����symbol_scope_TX
      symbol_scope_TX.TX[symbol_scope_TX.top++] = symbolTable.index;
      T->width = 0;
      T->code = NULL;
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //����ò�ľֲ�����DEF_LIST
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->width += T->ptr[0]->width;
        T->code = T->ptr[0]->code;
      }
      if (T->ptr[1])
      {
        strcpy(T->ptr[1]->Sbreak, T->Sbreak);
        T->ptr[1]->offset = T->offset + T->width;
        strcpy(T->ptr[1]->Snext, T->Snext); //S.next�������´���
        semantic_Analysis(T->ptr[1]);       //�����������������
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->width += T->ptr[1]->width;
        T->code = merge(2, T->code, T->ptr[1]->code);
      }
      if (!T->error)
      {
        prn_symbol(); //c���˳�һ���������ǰ��ʾ�ķ��ű�
      }
      LEV--;                                                         //��������䣬��ż�1
      symbolTable.index = symbol_scope_TX.TX[--symbol_scope_TX.top]; //ɾ�����������еķ���
      break;
    case DEF_LIST:
      T->code = NULL;
      if (T->ptr[0])
      {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //����һ���ֲ���������
        T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
        T->code = T->ptr[0]->code;
        T->width = T->ptr[0]->width;
      }
      if (T->ptr[1])
      {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]); //����ʣ�µľֲ���������
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        T->code = merge(2, T->code, T->ptr[1]->code);
        T->width += T->ptr[1]->width;
      }
      break;
    case VAR_DEF: //����һ���ֲ���������,����һ������(TYPE���)�е������͵��ڶ������ӵ�������
      local_var_list(T);
      break;
    case STM_LIST:
      if (!T->ptr[0])
      {
        T->code = NULL;
        T->width = 0;
        break;
      }              //���������
      if (T->ptr[1]) //2������������ӣ������±����Ϊ��һ���������󵽴��λ��
        strcpy(T->ptr[0]->Snext, newLabel());
      else //������н���һ����䣬S.next�������´���
        strcpy(T->ptr[0]->Snext, T->Snext);
      T->ptr[0]->offset = T->offset;
      strcpy(T->ptr[0]->Sbreak, T->Sbreak);
      semantic_Analysis(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->code = T->ptr[0]->code;
      T->width = T->ptr[0]->width;
      if (T->ptr[1])
      {
        strcpy(T->ptr[1]->Sbreak, T->Sbreak);
        strcpy(T->ptr[1]->Snext, T->Snext); //2�������������,S.next�������´���
        T->ptr[1]->offset = T->offset;      //˳��ṹ����Ԫ��ʽ
        // T->ptr[1]->offset = T->offset + T->ptr[0]->width; //˳��ṹ˳����䵥Ԫ��ʽ
        semantic_Analysis(T->ptr[1]);
        T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
        //�����е�1��Ϊ���ʽ��䣬������䣬�������ʱ����2��ǰ����Ҫ���
        if (T->ptr[0]->kind == RETURN || T->ptr[0]->kind == EXP_STMT || T->ptr[0]->kind == COMP_STM)
          T->code = merge(2, T->code, T->ptr[1]->code);
        else
          T->code = merge(3, T->code, genLabel(T->ptr[0]->Snext), T->ptr[1]->code);
        if (T->ptr[1]->width > T->width)
          T->width = T->ptr[1]->width; //˳��ṹ����Ԫ��ʽ
                                       //T->width+=T->ptr[1]->width;//˳��ṹ˳����䵥Ԫ��ʽ
      }
      break;
    case IF_THEN:
      strcpy(T->ptr[0]->Etrue, newLabel()); //��������������ת��λ��
      strcpy(T->ptr[0]->Efalse, T->Snext);
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      boolExp(T->ptr[0]);
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      strcpy(T->ptr[1]->Snext, T->Snext);
      semantic_Analysis(T->ptr[1]); //if�Ӿ�
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
      break; //������䶼��û�д���offset��width����
    case IF_THEN_ELSE:
      strcpy(T->ptr[0]->Etrue, newLabel()); //��������������ת��λ��
      strcpy(T->ptr[0]->Efalse, newLabel());
      T->ptr[0]->offset = T->ptr[1]->offset = T->ptr[2]->offset = T->offset;
      boolExp(T->ptr[0]); //������Ҫ��������·���봦��
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      strcpy(T->ptr[1]->Snext, T->Snext);
      semantic_Analysis(T->ptr[1]); //if�Ӿ�
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      strcpy(T->ptr[2]->Snext, T->Snext);
      semantic_Analysis(T->ptr[2]); //else�Ӿ�
      T->error = T->ptr[2]->error ? T->ptr[2]->error : T->error;

      if (T->width < T->ptr[2]->width)
        T->width = T->ptr[2]->width;
      T->code = merge(6, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code,
                      genGoto(T->Snext), genLabel(T->ptr[0]->Efalse), T->ptr[2]->code);
      break;
    case WHILE:
      strcpy(T->ptr[0]->Etrue, newLabel()); //�ӽ��̳����Եļ���
      strcpy(T->ptr[0]->Efalse, T->Snext);
      T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
      boolExp(T->ptr[0]); //ѭ��������Ҫ��������·���봦��
      T->error = T->ptr[0]->error ? T->ptr[0]->error : T->error;
      T->width = T->ptr[0]->width;
      strcpy(T->ptr[1]->Snext, newLabel());
      strcpy(T->ptr[1]->Sbreak, T->Snext);
      semantic_Analysis(T->ptr[1]); //ѭ����
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
      T->code = merge(5, genLabel(T->ptr[1]->Snext), T->ptr[0]->code,
                      genLabel(T->ptr[0]->Etrue), T->ptr[1]->code, genGoto(T->ptr[1]->Snext));
      break;
    case FOR:
      T0 = T->ptr[0];
      if (!T0->ptr[0])
      {
        T0->ptr[0] = mknode(EXP_STMT, NULL, NULL, NULL, T->pos);
        T0->ptr[0]->code = NULL;
      }
      T0->ptr[0]->offset = T->offset;
      strcpy(T0->ptr[0]->Snext, newLabel());
      semantic_Analysis(T0->ptr[0]);
      T->width = T0->ptr[0]->width;
      T->error = T0->ptr[0]->error ? T0->ptr[0]->error : T->error;

      if (!T0->ptr[1])
      {
        T0->ptr[1] = mknode(EXP_STMT, NULL, NULL, NULL, T->pos);
        T0->ptr[1]->code = NULL;
      }
      strcpy(T0->ptr[1]->Etrue, newLabel());
      strcpy(T0->ptr[1]->Efalse, T->Snext);
      T0->ptr[1]->offset = T->offset;
      boolExp(T0->ptr[1]); //ѭ��������Ҫ��������·���봦��
      T->error = T0->ptr[1]->error ? T0->ptr[1]->error : T->error;
      if (T->width < T0->ptr[1]->width)
        T->width = T0->ptr[1]->width;

      if (!T0->ptr[2])
      {
        T0->ptr[2] = mknode(EXP_STMT, NULL, NULL, NULL, T->pos);
        T0->ptr[2]->code = NULL;
      }
      strcpy(T0->ptr[2]->Snext, T0->ptr[0]->Snext);
      T0->ptr[2]->offset = T->offset;
      semantic_Analysis(T0->ptr[2]);
      T->error = T0->ptr[2]->error ? T0->ptr[2]->error : T->error;
      if (T->width < T0->ptr[2]->width)
        T->width = T0->ptr[2]->width;

      strcpy(T->ptr[1]->Snext, newLabel());
      strcpy(T->ptr[1]->Sbreak, T->Snext);
      T->ptr[1]->offset = T->offset;
      semantic_Analysis(T->ptr[1]);
      T->error = T->ptr[1]->error ? T->ptr[1]->error : T->error;
      if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;

      T->code = merge(9, T0->ptr[0]->code, genLabel(T0->ptr[0]->Snext), T0->ptr[1]->code, genLabel(T->ptr[1]->Snext),
                      T0->ptr[2]->code, genGoto(T0->ptr[0]->Snext), genLabel(T0->ptr[1]->Etrue), T->ptr[1]->code, genGoto(T->ptr[1]->Snext));
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
      if (strlen(T->Sbreak) == 0)
      {
        T->error = 1;
        semantic_error(T->pos, " break can only be used in a loop", "");
      }
      else
      {
        T->code = genGoto(T->Sbreak);
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
    case MULT:
    case DIV:
    case NOT:
    case UMINUS:
    case DMINUS:
    case DPLUS:
    case FDPLUS:
    case FDMINUS:
    case ARR_ACCESS:
    case FUNC_CALL:
      Exp(T); //����������ʽ
      break;
    }
  }
}

void semantic_Analysis0(struct node *T)
{
  symbolTable.index = 0;
  fillSymbolTable("read", "", 0, INT, 'F', 4);
  symbolTable.symbols[0].paramnum = 0; //read���βθ���
  fillSymbolTable("write", "", 0, INT, 'F', 4);
  fillSymbolTable("x", "", 1, INT, 'P', DX);
  symbolTable.symbols[1].paramnum = 1;
  symbol_scope_TX.TX[0] = 0; //�ⲿ�����ڷ��ű��е���ʼ���Ϊ0
  symbol_scope_TX.top = 1;
  T->offset = 0; //�ⲿ��������������ƫ����
  semantic_Analysis(T);
  printf("%d", T->error);
  if (!T->error)
  {
    prnIR(T->code);
    objectCode(T->code);
  }
}