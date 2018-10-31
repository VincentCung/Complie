#include "def.h"

//��δ�����ʵ��̲ģ��������ؼĴ�������,��ɽ��м����ת����MISPS���룬�����MIPS�����ļ�

void objectCode(struct codenode *head)
{
    char opnstr1[32], opnstr2[32], resultstr[32];
    struct codenode *h = head, *p;
    int i;
    FILE *fp;
    fp = fopen("object.s", "w");
    fprintf(fp, ".data\n");
    fprintf(fp, "_Prompt: .asciiz \"Enter an integer:  \"\n");
    fprintf(fp, "_ret: .asciiz \"\\n\"\n");
    fprintf(fp, ".globl main\n");
    fprintf(fp, ".text\n");
    fprintf(fp, "read:\n");
    fprintf(fp, "  li $v0,4\n");
    fprintf(fp, "  la $a0,_Prompt\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0,5\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  jr $ra\n");
    fprintf(fp, "write:\n");
    fprintf(fp, "  li $v0,1\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0,4\n");
    fprintf(fp, "  la $a0,_ret\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  move $v0,$0\n");
    fprintf(fp, "  jr $ra\n");
    do
    {
        switch (h->op)
        {
        case ASSIGNOP:
            if (h->opn1.kind == INT)
                fprintf(fp, "  li $t3, %d\n", h->opn1.const_int);
            else
            {
                fprintf(fp, "  lw $t1, %d($sp)\n", h->opn1.offset);
                fprintf(fp, "  move $t3, $t1\n");
            }
            fprintf(fp, "  sw $t3, %d($sp)\n", h->result.offset);
            break;
        case ARR_INIT:
            fprintf(fp, "  li $t3, %d\n", h->opn2.const_int);
            fprintf(fp, "  sw $t3, %d($sp)\n", h->result.offset + 4 * (h->opn2.const_int-1));
            break;
        case ARR_ACCESS:
            fprintf(fp, "  lw $t2, %d($sp)\n",h->opn2.offset);
            fprintf(fp, "  mul $t2, $t2 ,4\n");
            fprintf(fp, "  addi $t1, $sp,%d\n",h->opn1.offset);
            fprintf(fp, "  add $t1, $t1,$t2\n");
            fprintf(fp, "  lw $t3, 0($t1)\n");
            fprintf(fp, "  sw $t3, %d($sp)\n", h->result.offset);
            break;
        case PLUS:
        case MINUS:
        case MULT:
        case DIV:
            if (h->opn1.kind == INT)
                fprintf(fp, "  li $t1, %d\n", h->opn1.const_int);
            else
                fprintf(fp, "  lw $t1, %d($sp)\n", h->opn1.offset);
            if (h->opn2.kind == INT)
                fprintf(fp, "  li $t2, %d\n", h->opn2.const_int);
            else
                fprintf(fp, "  lw $t2, %d($sp)\n", h->opn2.offset);
            if (h->op == PLUS)
                fprintf(fp, "  add $t3,$t1,$t2\n");
            else if (h->op == MINUS)
                fprintf(fp, "  sub $t3,$t1,$t2\n");
            else if (h->op == MULT)
                fprintf(fp, "  mul $t3,$t1,$t2\n");
            else
            {
                fprintf(fp, "  div $t1, $t2\n");
                fprintf(fp, "  mflo $t3\n");
            }
            fprintf(fp, "  sw $t3, %d($sp)\n", h->result.offset);
            break;
        case FUNCTION:
            fprintf(fp, "\n%s:\n", h->result.id);
            if (!strcmp(h->result.id, "main")) //���⴦��main
                fprintf(fp, "  addi $sp, $sp, -%d\n", symbolTable.symbols[h->result.offset].offset);
            break;
        case PARAM: //ֱ����������һ��
            break;
        case LABEL:
            fprintf(fp, "%s:\n", h->result.id);
            break;
        case GOTO:
            fprintf(fp, "  j %s\n", h->result.id);
            break;
        case JLE:
        case JLT:
        case JGE:
        case JGT:
        case EQ:
        case NEQ:
            fprintf(fp, "  lw $t1, %d($sp)\n", h->opn1.offset);
            fprintf(fp, "  lw $t2, %d($sp)\n", h->opn2.offset);
            if (h->op == JLE)
                fprintf(fp, "  ble $t1,$t2,%s\n", h->result.id);
            else if (h->op == JLT)
                fprintf(fp, "  blt $t1,$t2,%s\n", h->result.id);
            else if (h->op == JGE)
                fprintf(fp, "  bge $t1,$t2,%s\n", h->result.id);
            else if (h->op == JGT)
                fprintf(fp, "  bgt $t1,$t2,%s\n", h->result.id);
            else if (h->op == EQ)
                fprintf(fp, "  beq $t1,$t2,%s\n", h->result.id);
            else
                fprintf(fp, "  bne $t1,$t2,%s\n", h->result.id);
            break;
        case ARG: //ֱ����������һ��,��ͷ����
            break;
        case CALL:
            if (!strcmp(h->opn1.id, "read"))
            { //���⴦��read
                fprintf(fp, "  addi $sp, $sp, -4\n");
                fprintf(fp, "  sw $ra,0($sp)\n"); //����ص��?
                fprintf(fp, "  jal read\n");      //����ص��?
                fprintf(fp, "  lw $ra,0($sp)\n"); //�ָ����ص�ַ
                fprintf(fp, "  addi $sp, $sp, 4\n");
                fprintf(fp, "  sw $v0, %d($sp)\n", h->result.offset);
                break;
            }
            if (!strcmp(h->opn1.id, "write"))
            { //���⴦��write
                fprintf(fp, "  lw $a0, %d($sp)\n", h->prior->result.offset);
                fprintf(fp, "  addi $sp, $sp, -4\n");
                fprintf(fp, "  sw $ra,0($sp)\n");
                fprintf(fp, "  jal write\n");
                fprintf(fp, "  lw $ra,0($sp)\n");
                fprintf(fp, "  addi $sp, $sp, 4\n");
                break;
            }

            for (p = h, i = 0; i < symbolTable.symbols[h->opn1.offset].paramnum; i++) //��λ����һ��ʵ�εĽ��??
                p = p->prior;
            //�����¼�ռ�
            fprintf(fp, "  move $t0,$sp\n"); //���浱ǰ������sp��$t0�У�Ϊ��ȡʵ�α��ʽ���?
            fprintf(fp, "  addi $sp, $sp, -%d\n", symbolTable.symbols[h->opn1.offset].offset);
            fprintf(fp, "  sw $ra,0($sp)\n"); //����ص��?
            i = h->opn1.offset + 1;           //��һ���βα����ڷ��ű��λ�����
            while (symbolTable.symbols[i].flag == 'P')
            {
                fprintf(fp, "  lw $t1, %d($t0)\n", p->result.offset); //ȡʵ��ֵ
                fprintf(fp, "  move $t3,$t1\n");
                fprintf(fp, "  sw $t3,%d($sp)\n", symbolTable.symbols[i].offset); //�͵������ú������βε�Ԫ
                p = p->next;
                i++;
            }
            fprintf(fp, "  jal %s\n", h->opn1.id);                                          //�ָ����ص�ַ
            fprintf(fp, "  lw $ra,0($sp)\n");                                               //�ָ����ص�ַ
            fprintf(fp, "  addi $sp,$sp,%d\n", symbolTable.symbols[h->opn1.offset].offset); //�ͷŻ��¼�ռ�
            fprintf(fp, "  sw $v0,%d($sp)\n", h->result.offset);                            //ȡ����ֵ
            break;
        case RETURN:
            fprintf(fp, "  lw $v0,%d($sp)\n", h->result.offset); //����ֵ�͵�$v0
            fprintf(fp, "  jr $ra\n");
            break;
        }
        h = h->next;
    } while (h != head);
    fclose(fp);
}
