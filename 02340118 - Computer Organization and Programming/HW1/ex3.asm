.global _start
_start:
    # Moving the input and output into registers
    lea array1, %eax
    lea array2, %ebx
    lea mergedArray, %ecx
    
    # The start of the loop
    loop_HW1:
    movl (%eax), %r11d # Getting the current data from array1
    movl (%ebx), %r12d # Getting the current data from array2
    
    # Checking if A1 is finished
    cmpl $0, %r11d
    je a1IsFinished
    
    # Checking if A2 is finished
    cmpl $0, %r12d
    je a2IsFinished
    
    # Comparing array1[i] with array2[j], the larger number will be added to mergedArray
    cmpl %r11d, %r12d
    jz equalData_HW1
    ja a2IsGreater_HW1
    
    # Case = array1 has a larger number
    a1IsGreater_HW1:
    # Checking if the number is already in the array
    mov %ecx, %r8d
    sub $4, %r8d
    cmpl (%r8d), %r11d
    je incrementA1_HW1
    
    movl %r11d, (%ecx) # Adding the data to mergedArray
     
    add $4, %ecx # Increment iterating variable for mergedArray   
    incrementA1_HW1:
    add $4, %eax # Increment iterating variable for array1
    
    
    jmp loop_HW1
    
    # Case = array2 has a larger number
    a2IsGreater_HW1:
    # Checking if the number is already in the array
    mov %ecx, %r8d
    sub $4, %r8d
    cmpl (%r8d), %r12d
    je incrementA2_HW1
    
    movl %r12d, (%ecx) # Adding the data to mergedArray
    
    add $4, %ecx # Increment iterating variable for mergedArray
    incrementA2_HW1:
    add $4, %ebx # Increment iterating variable for array2
    
    jmp loop_HW1
    
    # Case = the numbers are equal
    equalData_HW1:
    # Checking if the number is already in the array
    mov %ecx, %r8d
    sub $4, %r8d
    cmpl (%r8d), %r12d
    je incrementAll_HW1
    
    movl %r11d, (%ecx) # Adding the data to mergedArray
    add $4, %ecx # Increment iterating variable for mergedArray
    
    incrementAll_HW1:
    add $4, %eax # Increment iterating variable for array1
    add $4, %ebx # Increment iterating variable for array2
    
    jmp loop_HW1
    
    a1IsFinished:
    # Checking if A2 is finished
    cmpl $0, %r12d
    je almostDone_HW1
    
    jmp a2IsGreater_HW1 # Fill the rest of A2
    
    a2IsFinished:
    # Checking if A1 is finished
    cmpl $0, %r11d
    je almostDone_HW1
    
    jmp a1IsGreater_HW1 # Fill the rest of A1
    
    almostDone_HW1:
    movl $0, (%ecx) # Adding a zero to the end of mergedArray
    
    done_HW1:
    