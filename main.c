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
{ //�Գ����﷨�����ȸ�����
  int i = 1;
  struct node *T0;
  if (T)
  {
    switch (T->kind)
    {
    case EXT_DEF_LIST:
      display(T->ptr[0], indent); //��ʾ���ⲿ�����б��еĵ�һ��
      display(T->ptr[1], indent); //��ʾ���ⲿ�����б��е������ⲿ����
      break;
    case EXT_VAR_DEF:
      printf("%*c�ⲿ�������壺\n", indent, ' ');
      display(T->ptr[0], indent + 3); //��ʾ�ⲿ��������
      printf("%*c��������\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾ�����б�
      break;
    case TYPE:
      printf("%*c���ͣ� %s\n", indent, ' ', T->type_id);
      break;
    case EXT_DEC_LIST:
      display(T->ptr[0], indent); //������ʾ�ⲿ��������
      display(T->ptr[1], indent); //����������ͬ�ģ�����ʾ�﷨���˴��������Ժ����ƴ���ϲ�
      break;
    case FUNC_DEF:
      printf("%*c�������壺\n", indent, ' ');
      display(T->ptr[0], indent + 3); //��ʾ������������
      display(T->ptr[1], indent + 3); //��ʾ�������Ͳ���
      display(T->ptr[2], indent + 3); //��ʾ������
      break;
    case FUNC_DEC:
      printf("%*c��������%s\n", indent, ' ', T->type_id);
      if (T->ptr[0])
      {
        printf("%*c�����βΣ�\n", indent, ' ');
        display(T->ptr[0], indent + 3); //��ʾ���������б�
      }
      else
        printf("%*c�޲κ���\n", indent + 3, ' ');
      break;
    case PARAM_LIST:
      display(T->ptr[0], indent); //������ʾȫ���������ͺ����ƣ�
      display(T->ptr[1], indent);
      break;
    case PARAM_DEC:
      printf("%*c���ͣ�%s, ��������%s\n", indent, ' ',
             T->ptr[0]->type == INT ? "int" : "float", T->ptr[1]->type_id);
      break;
    case EXP_STMT:
      printf("%*c���ʽ��䣺\n", indent, ' ');
      if (T->ptr[0] == NULL)
      {
        printf("%*c�ձ��ʽ\n", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[0], indent + 3);
      }
      break;
    case RETURN:
      printf("%*c������䣺\n", indent, ' ');
      display(T->ptr[0], indent + 3);
      break;
    case COMP_STM:
      printf("%*c������䣺\n", indent, ' ');
      display(T->ptr[0], indent + 3); //��ʾ���岿��
      printf("%*c����������䲿�֣�\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾ��䲿��
      break;
    case STM_LIST:
      display(T->ptr[0], indent); //��ʾ��һ�����
      display(T->ptr[1], indent); //��ʾʣ�����
      break;
    case WHILE:
      printf("%*cwhileѭ����䣺\n", indent, ' ');
      printf("%*cѭ��������\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //��ʾѭ������
      printf("%*cѭ���壺\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾѭ����
      break;
    case FOR:
      printf("%*cforѭ����䣺\n", indent, ' ');
      printf("%*cѭ��������\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //��ʾѭ������
      printf("%*cѭ���壺\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾѭ����
      break;
    case CON_LIST:

      printf("%*c����һ��\n", indent, ' ');
      if (T->ptr[0] == NULL)
      {
        printf("%*c�ձ��ʽ\n", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[0], indent + 3);
      }
      printf("%*c��������\n", indent, ' ');
      if (T->ptr[1] == NULL)
      {
        printf("%*c�ձ��ʽ\n", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[1], indent + 3);
      }
      printf("%*c��������\n", indent, ' ');
      if (T->ptr[2] == NULL)
      {
        printf("%*c�ձ��ʽ\n", indent + 3, ' ');
      }
      else
      {
        display(T->ptr[2], indent + 3);
      }
      break;
    case IF_THEN:
      printf("%*c�������(IF_THEN)��\n", indent, ' ');
      printf("%*c������\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //��ʾ����
      printf("%*cIF�Ӿ䣺\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾif�Ӿ�
      break;
    case IF_THEN_ELSE:
      printf("%*c�������(IF_THEN_ELSE)��\n", indent, ' ');
      printf("%*c������\n", indent + 3, ' ');
      display(T->ptr[0], indent + 6); //��ʾ����
      printf("%*cIF�Ӿ䣺\n", indent + 3, ' ');
      display(T->ptr[1], indent + 6); //��ʾif�Ӿ�
      printf("%*cELSE�Ӿ䣺\n", indent + 3, ' ');
      display(T->ptr[2], indent + 6); //��ʾelse�Ӿ�
      break;
    case DEF_LIST:
      display(T->ptr[0], indent); //��ʾ�þֲ����������б��еĵ�һ��
      display(T->ptr[1], indent); //��ʾ�����ֲ���������
      break;
    case VAR_DEF:
      printf("%*c�ֲ��������壺\n", indent, ' ');
      display(T->ptr[0], indent + 3); //��ʾ��������
      display(T->ptr[1], indent + 3); //��ʾ�ö����ȫ��������
      break;
    case DEC_LIST:
      printf("%*cVAR_NAME��\n", indent, ' ');
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
          //��ʾ��ʼ�����ʽ
          display(T0->ptr[0]->ptr[1], indent + strlen(T0->ptr[0]->ptr[0]->type_id) + 4);
          if (T0->ptr[0]->ptr[1] == NULL)
          {
            printf("%*c�޳�ʼֵ\n", indent + 3, ' ');
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
      printf("%*cID�� %s\n", indent, ' ', T->type_id);
      break;
    case INT:
      printf("%*cINT��%d\n", indent, ' ', T->type_int);
      break;
    case FLOAT:
      printf("%*cFLAOT��%f\n", indent, ' ', T->type_float);
      break;
    case CHAR:
      if (strlen(T->type_id) == 0)
      {
        printf("%*cCHAR�����ַ�\n", indent, ' ', T->type_id);
      }
      else
      {
        printf("%*cCHAR��%s\n", indent, ' ', T->type_id);
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
      printf("%*c�������ã�\n", indent, ' ');
      printf("%*c��������%s\n", indent + 3, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case ARGS:
      i = 1;
      while (T)
      { //ARGS��ʾʵ�ʲ������ʽ���н�㣬���һ������Ϊ��һ��ʵ�ʲ������ʽ���ڶ�������Ϊʣ�µġ�
        struct node *T0 = T->ptr[0];
        printf("%*c��%d��ʵ�ʲ������ʽ��\n", indent, ' ', i++);
        display(T0, indent + 3);
        T = T->ptr[1];
      }
      //                    printf("%*c��%d��ʵ�ʲ������ʽ��\n",indent,' ',i);
      //                  display(T,indent+3);
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
        printf("%*c��%dά��", indent, ' ', i++);
        display(T0, indent);
        T = T->ptr[1];
      }
      printf("\n");
      break;
    case ARR_SUB:
      printf(" %d\n", T->type_int);
      break;
    case ARR_ELE:
      printf("%*c���飺%s\n", indent, ' ', T->type_id);
      display(T->ptr[0], indent + 3);
      break;
    case ARR_ACCESS:
      printf("%*c��\n", indent, ' ');
      display(T->ptr[0], indent + 3);
      printf("%*c����\n", indent, ' ');
      display(T->ptr[1], indent + 3);
      break;
    case VAL_LIST:
      i = 1;
      while (T)
      {
        struct node *T0 = T->ptr[0];
        printf("%*c��%d��ֵ��\n", indent, ' ', i++);
        display(T0, indent + 3);
        T = T->ptr[1];
      }
      printf("\n");
      break;
    }
  }
}
