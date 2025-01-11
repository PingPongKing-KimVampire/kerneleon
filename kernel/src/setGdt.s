section .text
global setGdt

gdtr DW 0       ; Limit 저장용
     DD 0       ; Base 저장용

setGdt:
   MOV   AX, [esp + 4]
   MOV   [gdtr], AX
   MOV   EAX, [ESP + 8]
   ADD   EAX, [ESP + 12]
   MOV   [gdtr + 2], EAX
   LGDT  [gdtr]
   RET