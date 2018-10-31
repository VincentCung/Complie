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
  addi $sp, $sp, -24
  li $t3, 5
  sw $t3, 16($sp)
  lw $t1, 16($sp)
  move $t3, $t1
  sw $t3, 12($sp)
label3:
  li $t3, 0
  sw $t3, 20($sp)
  lw $t1, 12($sp)
  lw $t2, 20($sp)
  bgt $t1,$t2,label4
  j label2
label5:
  lw $t1, 12($sp)
  move $t3, $t1
  sw $t3, 20($sp)
  lw $t1, 12($sp)
  li $t2, 1
  sub $t3,$t1,$t2
  sw $t3, 12($sp)
  j label3
label4:
  lw $a0, 12($sp)
  addi $sp, $sp, -4
  sw $ra,0($sp)
  jal write
  lw $ra,0($sp)
  addi $sp, $sp, 4
  j label5
label2:
  li $t3, 0
  sw $t3, 20($sp)
  lw $v0,20($sp)
  jr $ra
label1:
