.data
_Prompt: .asciiz "Enter an integer:  "
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0,4
  la $a0,_Prompt
  syscall
  li $v0,5
  syscall
  jr $ra
write:
  li $v0,1
  syscall
  li $v0,4
  la $a0,_ret
  syscall
  move $v0,$0
  jr $ra

main:
  addi $sp, $sp, -36
  li $t3, 1
  sw $t3, 12($sp)
  li $t3, 2
  sw $t3, 16($sp)
  li $t3, 3
  sw $t3, 20($sp)
  li $t3, 0
  sw $t3, 28($sp)
  lw $t1, 28($sp)
  move $t3, $t1
  sw $t3, 24($sp)
  lw $t1, 24($sp)
  move $t3, $t1
  sw $t3, 32($sp)
label3:
  li $t3, 3
  sw $t3, 28($sp)
  lw $t1, 24($sp)
  lw $t2, 28($sp)
  blt $t1,$t2,label4
  j label2
label5:
  lw $t1, 24($sp)
  move $t3, $t1
  sw $t3, 28($sp)
  lw $t1, 24($sp)
  li $t2, 1
  add $t3,$t1,$t2
  sw $t3, 24($sp)
  j label3
label4:
  lw $t2, 24($sp)
  mul $t2, $t2 ,4
  addi $t1, $sp,12
  add $t1, $t1,$t2
  lw $t3, 0($t1)
  sw $t3, 28($sp)
  lw $a0, 28($sp)
  addi $sp, $sp, -4
  sw $ra,0($sp)
  jal write
  lw $ra,0($sp)
  addi $sp, $sp, 4
  j label5
label2:
  li $t3, 0
  sw $t3, 28($sp)
  lw $v0,28($sp)
  jr $ra
label1:
