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
      display(T->ptr[1], indent + 6); //��ʾ�����б�
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
        display(T->ptr[0], indent + 3); //��ʾ���������б�
      }
      else
        printf("%*cnoPamrm function:\n", indent + 3, ' ');
      break;
    case PARAM_LIST:
      display(T->ptr[0], indent); //������ʾȫ���������ͺ����ƣ�
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
      printf("%*cReturn statement:", indent, ' ');
      display(T->ptr[0], indent + 3);
      break;
    case COMP_STM:
      printf("%*cComplete statement define part:", indent, ' ');
      display(T->ptr[0], indent + 3); 
      printf("%*cComplete statement state part:", indent + 3, ' ');
      display(T->ptr[1], indent + 6); 
      break;
    case STM_LIST:
      display(T->ptr[0], indent); //��ʾ��һ�����?
      display(T->ptr[1], indent); //��ʾʣ�����?
      break;
    case WHILE:
      printf("%*cwhile:", indent, ' ');
      printf("%*cLoop condition:\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //��ʾѭ������
      printf("%*cLoop body:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾѭ����
      break;
    case FOR:
      printf("%*cfor:", indent, ' ');
      printf("%*cLoop condition\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //��ʾѭ������
      printf("%*cLoop body:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾѭ����
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
      display(T->ptr[0], indent + 6); //��ʾ����
      printf("%*cIF satement:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾif�Ӿ�
      break;
    case IF_THEN_ELSE:
      printf("%*cCondition statement(IF_THEN_ELSE):\n", indent, ' ');
      printf("%*ccondition:\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //��ʾ����
      printf("%*cIF statement:\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾif�Ӿ�
      printf("%*cELSE statement:\n", indent + 3, ' ');
      display(T->ptr[2], indent + 6); //��ʾelse�Ӿ�
      break;
    case DEF_LIST:
      display(T->ptr[0], indent); //��ʾ�þֲ����������б��еĵ�һ��
      display(T->ptr[1], indent); //��ʾ�����ֲ���������
      break;
    case VAR_DEF:
      printf("%*cLocal var list:\n", indent, ' ');
      display(T->ptr[0], indent + 3); //��ʾ��������
      display(T->ptr[1], indent + 3); //��ʾ�ö����ȫ��������?
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
          if (T0->ptr[0]->ptr[0]->kind == ARR_ELE)
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
        else if (T0->ptr[0]->kind == ARR_ELE)
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
    case PERASSIGN:
    case AND:
    case OR:
    case RELOP:
    case PLUS:
    case MINUS:
    case MULT:
    case DIV:
    case PER:
      printf("%*c%s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      display(T->ptr[1], indent + 3);
      break;
    case NOT:
    case UPLUS:
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
      { //ARGS��ʾʵ�ʲ������ʽ���н�㣬���һ������Ϊ��һ��ʵ�ʲ������ʽ���ڶ�������Ϊʣ�µġ�
        struct node *T0 = T->ptr[0];
        printf("%*c%dth param expression:", indent, ' ', i++);
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
    case ARR_ELE:
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
