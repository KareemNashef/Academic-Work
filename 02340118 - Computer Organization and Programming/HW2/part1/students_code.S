.global hamming_weight, negate_first_k, bring_balance_to_the_word, read_input

.section .data
	toInt: .quad 0
	
.section .bss
    .lcomm path, 1024

.section .text
read_input:

    # Pushing used regs:
    push %rbx
    push %r12
    push %rdi
    
    # Getting the path to the file:
    lea path(%rip) , %rdi
    call get_path # rdi now has a pointer to the file path
    
    # Getting things ready for a sys_open:
    mov $2, %rax            # sys_open
    mov $0, %rsi            # read only
    syscall # rax now has the file descriptor
    
    # Pushing the file descriptor to the stack to close it later
    push %rax
    
    # Getting the length of the codeword:
    mov $0, %rbx # will hold the length in ascii
    subq $8, %rsp # will hold the read length
    
    # === CoWo loop ===
    CW_loop:
        ror $8, %rbx
        mov $0, %rax                    # sys_read
        mov 8(%rsp), %rdi               # file descriptor 
        mov %rsp, %rsi                  # read byte destination
        mov $1, %rdx                    # reading a single byte
        syscall
        
        # Assuming sys_read was successful, 'rsi' holds the location of the read byte
        mov (%rsi), %bl

        # Checking for the end '\n' byte:
        cmp $0xa, %bl
        jne CW_loop
    # === ==== ==== ===
    
    # Returning rsp to its previous location:
    addq $8, %rsp 
    
    # Fixing the number representation in rbx:
    shr $8, %rbx
    shl $8, %rbx # got rid of the '\n'

    # === RBX loop ===
    RBX_loop:
        rol $8, %rbx
        cmp $0, %bl
        jne RBX_loop
    # === === ==== ===
    shr $8, %rbx

    mov %rbx, toInt(%rip)
    
    lea toInt(%rip), %rdi
    call atam_atol # rax now holds the length of the codeword in numbers
    mov %rax, %rbx

    # Getting the the codeword:
    mov $0, %rax                        # sys_read
    mov (%rsp), %rdi                    # file descriptor    
    mov 8(%rsp), %rsi                   # read codeword destination
    mov %rbx, %rdx                      # reading a N bytes
    syscall
    
    # Closing the file:
    mov $3, %rax
    pop %rdi
    syscall
        
    # Returning N/8 in rax:
    mov %rbx, %rax
    shr $3, %rax

    # Popping used regs:
    pop %rdi
    pop %r12
    pop %rbx
    
  ret

hamming_weight:

    # Initializing counters:
    mov $0, %rax    # 1s counter
    mov $0, %r8     # overall bytes counter
    mov $0, %r9     # overall bits counter
    mov $0, %rdx    # will hold current byte
    imul $8, %rsi
    
    # === Main loop ===
    HW_Main:
    
    # Checking if the bytes are done:
    cmp $0, %rsi
    je HWM_done
    
    # Moving the current byte to be counted:
    movb (%rdi, %r8, 1), %dl
    
        # --- Secondary loop ---
        HW_Sec:
        # Checking the number of bits counted so far:
        cmp $8, %r9
        je HWS_done
        
        # Incrementing the number of bits counted so far:
        inc %r9
        
        # Checking if the current lowest bit is a 1:
        test $1, %dl
        jz HWS_continue
        
        # Incrementing the number of 1s:
        inc %rax
        
        HWS_continue:

        # Moving on to the next bit:
        shr %dl
        jmp HW_Sec
        
        # --- --------- ---- ---
        
    HWS_done:
    
    # Resetting bits counter
    mov $0, %r9
    
    # Incrementing the number of bytes counted so far:
    inc %r8
    dec %rsi
    
    jmp HW_Main
    # === ==== ==== ===
    
    HWM_done:
    
  ret

negate_first_k:

    # Initializing vars:
    mov $1, %rdx    # used to flip bits
    
    # === Main loop ===
    NF_loop:
    
    # Checking if the task is done:
    cmp $0, %rsi
    je NF_done
    
    # Flipping the bit in incrementing indexes:
    xor %rdx, %rdi
    shl $1, %rdx
    
    # Decrementing the counter & moving to the next loop:
    dec %rsi
    jmp NF_loop
    # === ==== ==== ===
    
    NF_done:
    
    # Setting the output:
    mov %rdi, %rax
    
  ret

bring_balance_to_the_word:

    # Pushing used regs:
    push %r12
    push %r13
    push %r14
    
    # Initializing vars:
    imul $32, %rsi, %r12 # balanced number
    mov $0, %r13 # current flipping index

    # Checking if the word is already balanced:
    push %rdi
    push %rsi
    
    call hamming_weight
    
    pop %rsi
    pop %rdi
    
    cmp %r12, %rax
    je BB_done

    # === Main loop ===
    BB_Main:
    
    # Incrementing the flip counter:
    inc %r13

    Count_done:
    
        # --- Secondary loop ---
        mov %r13, %r14      # getting the total number of flips
        mov $0, %r9         # counting the quads
        
        BB_Sec:
        cmp $64, %r14       # checking if we reached the final quad
        jle LastQuad
        
        # Othewise we flip an entire quad
        push %rdi
        push %rsi
        
        mov (%rdi,%r9,8), %rdi
        mov $64, %rsi
        
        call negate_first_k
        
        pop %rsi
        pop %rdi
        
        mov %rax, (%rdi,%r9,8)
        
        inc %r9
        sub $64, %r14
        jmp BB_Sec

        LastQuad:
        push %rdi
        push %rsi
        
        mov (%rdi,%r9,8), %rdi
        mov %r14, %rsi
        
        call negate_first_k
        
        pop %rsi
        pop %rdi
        
        mov %rax, (%rdi,%r9,8)
        # --- --------- ---- ---
     
    # Checking again if the word is now balanced:
    push %rdi
    push %rsi
    
    call hamming_weight
    
    pop %rsi
    pop %rdi
    
    cmp %r12, %rax
    je BB_done
        
    # === Otherwise we undo the last flip ===

        # --- Undo loop ---
        mov %r13, %r14      # getting the total number of flips
        mov $0, %r9         # counting the quads
        
        BBU_Sec:
        cmp $64, %r14       # checking if we reached the final quad
        jle LastQuadUndo
        
        # Othewise we flip an entire quad
        push %rdi
        push %rsi
        
        mov (%rdi,%r9,8), %rdi
        mov $64, %rsi
        
        call negate_first_k
        
        pop %rsi
        pop %rdi
        
        mov %rax, (%rdi,%r9,8)
        
        inc %r9
        sub $64, %r14
        mov %r13, %rax
        jmp BBU_Sec

        LastQuadUndo:
        push %rdi
        push %rsi
        
        mov (%rdi,%r9,8), %rdi
        mov %r14, %rsi
        
        call negate_first_k
        
        pop %rsi
        pop %rdi
        
        mov %rax, (%rdi,%r9,8)
        # --- ---- ---- ---
        
    # === ======== == ==== === ==== ==== ===

    # Moving on to the next loop:
    jmp BB_Main
    # === ==== ==== ===
    
    BB_done:
    
    # Putting the last flip index in the output:
    mov %r13, %rax
    
    # Popping used regs:
    pop %r14
    pop %r13
    pop %r12
  ret
