#include "def.h"

struct node *mknode(int kind, struct node *first, struct node *second, struct node *third, int pos)
{
  struct node *T = (struct node *)malloc(sizeof(struct node));
  T->kind = kind;
  T->ptr[0] = first;
  T->ptr[1] = second;
  T->ptr[2] = third;
  T->pos = pos;
  T->place = 0;
  T->error = 0;
  T->dimension = 0;
  T->num = 0;
  strcpy(T->Sbreak,"");
  return T;
}

void display(struct node *T, int indent)
{ 
  int i = 1;
  struct node *T0;
  if (T)
  {
    switch (T->kind)
    {
    case EXT_DEF_LIST:
      display(T->ptr[0], indent); 
      display(T->ptr[1], indent); 
      break;
    case EXT_VAR_DEF:
      printf("%*cExtern variable define:\n", indent, ' ');
      display(T->ptr[0], indent + 3); 
      printf("%*cvariable name:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); 
      break;
    case TYPE:
      printf("%*ctype:%s\n", indent, ' ', T->type_id);
      break;
    case EXT_DEC_LIST:
      display(T->ptr[0], indent); 
      display(T->ptr[1], indent); 
      break;
    case FUNC_DEF:
      printf("%*cFunction define:\n", indent, ' ');
      display(T->ptr[0], indent + 3); 
      display(T->ptr[1], indent + 3); 
      display(T->ptr[2], indent + 3); 
      break;
    case FUNC_DEC:
      printf("%*cFunction name:%s\n", indent, ' ', T->type_id);
      if (T->ptr[0])
      {
        printf("%*cformal params:\n", indent, ' ');
        display(T->ptr[0], indent + 3); 
      }
      else
        printf("%*cnoParam function.\n", indent + 3, ' ');
      break;
    case PARAM_LIST:
      display(T->ptr[0], indent); 
      display(T->ptr[1], indent);
      break;
    case PARAM_DEC:
      printf("%*ctype:%s,parameter name:%s\n", indent, ' ',
             T->ptr[0]->type == INT ? "int" : "float", T->ptr[1]->type_id);
      break;
    case EXP_STMT:
      printf("%*cExpression statement:\n", indent, ' ');
      if (T->ptr[0] == NULL)
      {
        printf("%*cnull.", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[0], indent + 3);
      }
      break;
    case RETURN:
      printf("%*cReturn statement:\n", indent, ' ');
      display(T->ptr[0], indent + 3);
      break;
    case COMP_STM:
      printf("%*cComplete statement define part:\n", indent, ' ');
      display(T->ptr[0], indent + 3); 
      printf("%*cComplete statement state part:\n", indent, ' ');
      display(T->ptr[1], indent + 6); 
      break;
    case STM_LIST:
      display(T->ptr[0], indent); 
      display(T->ptr[1], indent); 
      break;
    case WHILE:
      printf("%*cwhile:", indent, ' ');
      printf("%*cLoop condition:\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); 
      printf("%*cLoop body:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); 
      break;
    case FOR:
      printf("%*cfor:", indent, ' ');
      printf("%*cLoop condition\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); 
      printf("%*cLoop body:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); 
      break;
    case CON_LIST:

      printf("%*cCondition one:\n", indent, ' ');
      if (T->ptr[0] == NULL)
      {
        printf("%*cnull.", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[0], indent + 3);
      }
      printf("%*cCondition two:\n", indent, ' ');
      if (T->ptr[1] == NULL)
      {
        printf("%*cnull.", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[1], indent + 3);
      }
      printf("%*cCondition three:\n", indent, ' ');
      if (T->ptr[2] == NULL)
      {
        printf("%*cnull.", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[2], indent + 3);
      }
      break;
    case IF_THEN:
      printf("%*cCondition statement(IF_THEN):\n", indent, ' ');
      printf("%*ccondition:\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); 
      printf("%*cIF satement:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); 
      break;
    case IF_THEN_ELSE:
      printf("%*cCondition statement(IF_THEN_ELSE):\n", indent, ' ');
      printf("%*ccondition:\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); 
      printf("%*cIF statement:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); 
      printf("%*cELSE statement:\n", indent + 3, ' ');
      display(T->ptr[2], indent + 6); 
      break;
    case DEF_LIST:
      display(T->ptr[0], indent); 
      display(T->ptr[1], indent); 
      break;
    case VAR_DEF:
      printf("%*cLocal var list:\n", indent, ' ');
      display(T->ptr[0], indent + 3); 
      display(T->ptr[1], indent + 3); 
      break;
    case DEC_LIST:
      printf("%*cvar name:\n", indent, ' ');
      T0 = T;
      while (T0)
      {
        if (T0->ptr[0]->kind == ID)
          printf("%*c %s\n", indent + 3, ' ', T0->ptr[0]->type_id);
        else if (T0->ptr[0]->kind == ASSIGNOP)
        {
          if (T0->ptr[0]->ptr[0]->kind == ARR_INIT)
          {
            display(T->ptr[0]->ptr[0], indent + 3);
            printf("%*c ASSIGNOP\n", indent + 3, ' ');
          }
          else
          {
            printf("%*c %s ASSIGNOP\n ", indent + 3, ' ', T0->ptr[0]->ptr[0]->type_id);
          }
          //��ʾ��ʼ������?
          display(T0->ptr[0]->ptr[1], indent + strlen(T0->ptr[0]->ptr[0]->type_id) + 4);
          if (T0->ptr[0]->ptr[1] == NULL)
          {
            printf("%*cinitial value:\n", indent + 3, ' ');
          }
        }
        else if (T0->ptr[0]->kind == ARR_INIT)
        {
          display(T->ptr[0], indent + 3);
        }
        T0 = T0->ptr[1];
      }
      break;
    case ID:
      printf("%*cID: %s\n", indent, ' ', T->type_id);
      break;
    case INT:
      printf("%*cINT: %d\n", indent, ' ', T->type_int);
      break;
    case FLOAT:
      printf("%*cFLAOT: %f\n", indent, ' ', T->type_float);
      break;
    case CHAR:
      if (strlen(T->type_id) == 0)
      {
        printf("%*cCHAR: empty char\n", indent, ' ', T->type_id);
      }
      else
      {
        printf("%*cCHAR: %s\n", indent, ' ', T->type_id);
      }
      break;
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
      printf("%*c%s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      display(T->ptr[1], indent + 3);
      break;
    case NOT:
    case UMINUS:
    case FDPLUS:
    case FDMINUS:
      printf("%*c%s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case FUNC_CALL:
      printf("%*cCall Function:\n", indent, ' ');
      printf("%*cfunction name:%s\n", indent + 3, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case ARGS:
      i = 1;
      while (T)
      { 
        struct node *T0 = T->ptr[0];
        printf("%*c%dth param expression:\n", indent, ' ', i++);
        display(T0, indent + 3);
        T = T->ptr[1];
      }
      printf("\n");
      break;
    case DMINUS:
    case DPLUS:
      printf("%*c%s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case ARR_LIST:
      i = 1;
      while (T)
      {
        struct node *T0 = T->ptr[0];
        printf("%*c%dth dimension:", indent, ' ', i++);
        display(T0, indent);
        T = T->ptr[1];
      }
      printf("\n");
      break;
    case ARR_SUB:
      printf(" %d\n", T->type_int);
      break;
    case ARR_INIT:
      printf("%*cArray: %s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case ARR_ACCESS:
      printf("%*cTO:\n", indent, ' ');
      display(T->ptr[0], indent + 3);
      printf("%*cAccess:\n", indent, ' ');
      display(T->ptr[1], indent + 3);
      break;
    case VAL_LIST:
      i = 1;
      while (T)
      {
        struct node *T0 = T->ptr[0];
        printf("%*c%dth Value:\n", indent, ' ', i++);
        display(T0, indent + 3);
        T = T->ptr[1];
      }
      printf("\n");
      break;
    }
  }
}

void semantic_error(int line, char *msg1, char *msg2)
{
  //锟斤拷锟斤拷锟斤拷锟街伙拷占锟斤拷锟斤拷锟斤拷锟较拷锟斤拷锟斤拷锟斤拷一锟斤拷锟斤拷示
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


struct codenode *genLabel(char *label)
{
  struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
  h->op = LABEL;
  strcpy(h->result.id, label);
  h->next = h->prior = h;
  return h;
}


struct codenode *genGoto(char *label)
{
  struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
  h->op = GOTO;
  strcpy(h->result.id, label);
  h->next = h->prior = h;
  return h;
}


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

//锟斤拷锟斤拷屑锟斤拷锟斤拷
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
  int i;
  for (i = symbolTable.index - 1; symbolTable.symbols[i].level == level || (level == 0 && i >= 0); i--)
  {
    if (level == 0 && symbolTable.symbols[i].level == 1)
      continue; 
    if (!strcmp(symbolTable.symbols[i].name, name))
      return -1;
  }
  strcpy(symbolTable.symbols[symbolTable.index].name, name);
  strcpy(symbolTable.symbols[symbolTable.index].alias, alias);
  symbolTable.symbols[symbolTable.index].level = level;
  symbolTable.symbols[symbolTable.index].type = type;
  symbolTable.symbols[symbolTable.index].flag = flag;
  symbolTable.symbols[symbolTable.index].offset = offset;
  return symbolTable.index++; 
}


int fill_Temp(char *name, int level, int type, char flag, int offset)
{
  strcpy(symbolTable.symbols[symbolTable.index].name, "");
  strcpy(symbolTable.symbols[symbolTable.index].alias, name);
  symbolTable.symbols[symbolTable.index].level = level;
  symbolTable.symbols[symbolTable.index].type = type;
  symbolTable.symbols[symbolTable.index].flag = flag;
  symbolTable.symbols[symbolTable.index].offset = offset;
  return symbolTable.index++; 
}

