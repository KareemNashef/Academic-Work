.global _start
_start:
    # Getting the data from the root of the tree and the new node
    mov root, %rax    # Contains the address for the root node
    mov new_node, %rbx  # Contains the data from the new node
    
    # Checking if the tree is empty
    cmp $0, %rax
    je newRoot_HW1
    
    
    loop_HW1:
    
    # Compare the current node data to the new node data
    cmp (%rax), %rbx
    jl leftNode_HW1
    je done_HW1
    
    # Case = new data is larger than current data
    rightNode_HW1:
    add $16, %rax # rax now points to the right node
    
    cmp $0, (%rax) # Check if the node is empty
    je newNode_HW1
    
    mov (%rax), %rax # The node is not empty, rax now points to the new data
    
    jmp loop_HW1
    
    # Case = new data is smaller than current data
    leftNode_HW1:
    add $8, %rax # rax now points to the left node
    
    cmp $0, (%rax) # Check if the node is empty
    je newNode_HW1
    
    mov (%rax), %rax # The node is not empty, rax now points to the new data
    
    jmp loop_HW1
    
    # Adding the new node to the location specified by rax
    newNode_HW1:
    lea new_node, %rbx
    mov %rbx, (%rax)
    
    jmp done_HW1
    
    # Case = Tree is empty, setting a new root
    newRoot_HW1:
    lea root, %rax
    lea new_node, %rbx
    mov %rbx, (%rax)
    
    done_HW1:
    
    