.section .data
gdtr:
	.word 0
	.long 0

.section .text
.global setGdt

setGdt:
   movw 4(%esp), %ax
   movw %ax, gdtr
   movl 8(%esp), %eax
   addl 12(%esp), %eax
   movl %eax, gdtr+2
   lgdt gdtr
   ret
