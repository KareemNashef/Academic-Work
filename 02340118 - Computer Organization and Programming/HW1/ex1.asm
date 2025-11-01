.global _start
_start:
    # Moving the input and output into registers
    mov (num), %rax
    mov $0, %ebx
    
    # Creating a counter for the loop
    mov $0, %dl
    
    # Loop start, runs 64 times 
    loop_HW1:
    cmp $64, %dl
    jz done_HW1
    
    # Bitwise AND between num and 0x1b, if the result is zero then the lsb is zero
    test $0b1, %al
    jz even_HW1
    
    # If the result was not zero then we increment CountBits
    inc %ebx
    
    # Moving on to the next lsb and incrementing the counter
    even_HW1:
    shr $1, %rax
    inc %dl
    
    # Next loop
    jmp loop_HW1
    
    done_HW1:
    mov %ebx, (CountBits)
    