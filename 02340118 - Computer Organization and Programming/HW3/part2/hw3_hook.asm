.global hook

.section .data
msg: .ascii "This code was hacked by Ella Lee's gang\n"
endmsg:

.section .text
hook:
  movq $60, %rax
  movq $0, %rdi
  syscall
