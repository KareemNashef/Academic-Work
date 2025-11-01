.global _start
_start:
    # Moving the input into registers
    lea head, %rax
    mov src, %rbx
    mov dst, %rcx
    
    # Checking if the src and dst are equal
    cmp %rbx, %rcx
    je done_HW1
    
    # Checking if the list is empty
    cmp $0, (%rax)
    je done_HW1
    
    # Counting how many times src and dst appear in the list
    mov $0, %r8d # src counter
    mov $0, %r9d # dst counter
    
    # Moving rax to the first node
    mov (%rax), %rax
    
    countLoop_HW1:
    cmp (%rax), %rbx # Comparing with src
    je srcFound_HW1
    
    cmp (%rax), %rcx # Comparing with dst
    je dstFound_HW1
    
    
    nextNode_HW1:
    
    # Saving current node in case for previous
    mov %rax, %r12
    
    # Move rax to the next node
    add $8, %rax
    mov (%rax), %rax
    
    # Check if we reached the end of the list
    cmp $0, %rax
    je loopEnd_HW1
    
    # Next iteration
    jmp countLoop_HW1
    
    # Case = found src
    srcFound_HW1:
    inc %r8 # Incrementing src counter
    mov %rax, %r10 # Saving src location
    mov %r12, %r13 # Saving prev location
    
    jmp nextNode_HW1
    
    # Case = found dst
    dstFound_HW1:
    inc %r9 # Incrementing dst counter
    mov %rax, %r11 # Saving dst location
    mov %r12, %r14 # Saving prev location
    
    jmp nextNode_HW1
    
    loopEnd_HW1:
    # Checking if src and dst exist in the list multiple times or at all
    cmp $1, %r8
    jne done_HW1
    
    cmp $1, %r9
    jne done_HW1
    
    # Swaping the data between src and dst
    cmp (head), %r10
    je headSRC_HW1
    
    cmp (head), %r11
    je headDST_HW1
    
    add $8, %r14
    mov %r10, (%r14) # dst prev next is now src
    
    add $8, %r13
    mov %r11, (%r13) # src prev next is now dst
    
    jmp almostDone_HW1
    
    # Case = src is head
    headSRC_HW1:
    add $8, %r14
    mov %r10, (%r14) # dst prev next is now src
    
    lea head, %rax
    mov %r11, (%rax) # head is now dst
    
    jmp almostDone_HW1
    
    # Case = dst is head
    headDST_HW1:
    add $8, %r13
    mov %r11, (%r13) # src prev next is now dst
    
    lea head, %rax
    mov %r10, (%rax) # head is now dst
    
    jmp almostDone_HW1
    
    almostDone_HW1:
    mov %r10, %r12
    add $8, %r12 # src next
    mov (%r12), %r8 # src next address
    
    mov %r11, %r15
    add $8, %r15 # dst next
    mov (%r15), %r9 # dst next address
    
    add $8, %r10 # src next
    add $8, %r11 # dst next
    
    mov %r9, (%r10) # src next is now dst next
    mov %r8, (%r11) # dst next is now src next
    
    done_HW1:
    
    