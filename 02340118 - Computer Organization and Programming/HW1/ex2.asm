.global _start
_start:
    # Moving the input and output into registers
    lea source, %eax
    lea destination, %ebx
    movl (num), %ecx
    movl (num), %edx # Index
    movl $0, %r10d # 2nd index
    
    # The start of the loop, handles whether the number is positive or negative, or a zero at the end of the loop
    loop_HW1:
    cmp $0, %ecx
    jl negativeNum_HW1
    jz done_HW1
    
    positiveNum_HW1:
    # Checking for a possible overlap
    jmp possibleOverlapPos_HW1
    
    noOverlapPos_HW1:
    dec %edx # Adjusting the index
    
    # Move the data from memory to a temporay register, then move it to the destination
    movb (%eax,%edx,1), %r8b
    movb %r8b, (%ebx,%edx,1)
    
    # Decrement num and go to the next iteration
    dec %ecx

    jmp loop_HW1

    negativeNum_HW1:
    # Checking for a possible overlap
    jmp possibleOverlapNeg_HW1
    
    noOverlapNeg_HW1:
    
    cmp $0, %edx # Flipping the index for the first run
    jl negativeIndex_HW1
    
    dec %edx # Adjusting the index
    
    # Move the data from memory to a temporay register, then move it to the destination
    movb (%ebx,%edx,1), %r8b
    movb %r8b, (%eax,%edx,1)
    
    
    # Increment num and go to the next iteration
    inc %ecx
    jmp loop_HW1

    negativeIndex_HW1:
    sub %ecx, %edx
    sub %ecx, %edx
    jmp negativeNum_HW1
    
    possibleOverlapPos_HW1:
    cmp %eax, %ebx
    jl dTs_HW1
    
    # Case = possible overlap that is solvable by the regular code
    jmp noOverlapPos_HW1    
    
    # Case = possible overlap where the destination is before the source in the memory
    dTs_HW1:
    mov %ecx, %r9d
    imul $8, %r9d
    add %ebx, %r9d # r9d holds the address for the last byte to be moved
    cmp %r9d, %eax # If the address for the source is lower than r9d then there's an overlap
    jg noOverlapPos_HW1
    
    # Move the data from memory to a temporay register, then move it to the destination
    movb (%eax,%r10d,1), %r8b
    movb %r8b, (%ebx,%r10d,1)
    
    # Decrement num, incerement index and go to the next iteration
    dec %ecx
    inc %r10d
    
    jmp loop_HW1
    
    possibleOverlapNeg_HW1:
    cmp %eax, %ebx
    jg sTdNeg_HW1
    
    # Case = possible overlap that is solvable by the regular code
    jmp noOverlapNeg_HW1    
    
    # Case = possible overlap where the destination is before the source in the memory
    sTdNeg_HW1:
    mov %ecx, %r9d
    imul $8, %r9d
    add %eax, %r9d # r9d holds the address for the last byte to be moved
    cmp %r9d, %ebx # If the address for the destination is lower than r9d then there's an overlap
    jl noOverlapNeg_HW1
    
    # Move the data from memory to a temporay register, then move it to the destination
    movb (%ebx,%r10d,1), %r8b
    movb %r8b, (%eax,%r10d,1)
    
    # Increment num, incerement index and go to the next iteration
    inc %ecx
    inc %r10d
    
    jmp loop_HW1
    done_HW1:
    