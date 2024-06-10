section .data
  num1 db 0xFF
  num2 db 2

section .text
global _start

_start:
  
  mov al, [num1]
  mov bl, [num2]
  mul bl
  int 80h