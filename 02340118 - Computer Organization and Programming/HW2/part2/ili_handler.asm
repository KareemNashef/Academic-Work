.globl my_ili_handler

.text
.align 4, 0x90

my_ili_handler:
  ####### Some smart student's code here #######
  # Pushing the used regs:
  push %rax
  push %rbx
  push %rdi

  # Getting the illegal opcode:
  mov 24(%rsp), %rbx
  mov (%rbx), %rbx

  # Checking if the opcode is one or two bytes:
  cmp $0x0f, %bl
  je TwoBytes

  # === Case - One byte ===
  OneByte:
    mov $0, %rdi
    mov %rbx, %rdi
    call what_to_do
    jmp HandlerCheck
  # =======================

  # === Case - Two bytes ===
  TwoBytes:
    mov $0, %rdi
    shr $8, %rbx
    mov %rbx, %rdi
    call what_to_do
  # ========================

  # Depending on the 'what_to_do' output we jump to the appropriate handler:
  HandlerCheck:
    cmp $0, %rax
    je OldHandler

  # Using the new handler:
  NewHandler:
    mov %rax, %rdi          # moving the 'what_to_do' output to rdi
      # Popping the used regs:
      pop %rbx              # no need to pop rdi
      pop %rbx
      pop %rax
    add $2, (%rsp)          # moving the rip to the next instruction
    jmp ili_done            # finishing

  # Using the old handler:
  OldHandler:
    # Popping the used regs:
    pop %rdi
    pop %rbx
    pop %rax
    jmp *old_ili_handler    # jumping to the old handler

  ili_done:
  iretq
