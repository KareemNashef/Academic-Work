#include <asm/desc.h>

void my_store_idt(struct desc_ptr *idtr) {
    // <STUDENT FILL> - HINT: USE INLINE ASSEMBLY
    asm("sidt %0" : "=m"(*idtr));  // Storing the IDTR into the given pointer
    // </STUDENT FILL>
}

void my_load_idt(struct desc_ptr *idtr) {
    // <STUDENT FILL> - HINT: USE INLINE ASSEMBLY
    asm("lidt %0" : : "m"(*idtr));  // Loading the IDTR from the given pointer
    // <STUDENT FILL>
}

void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
    // <STUDENT FILL> - HINT: NO NEED FOR INLINE ASSEMBLY
    gate->offset_low = addr;           // Setting the lower 16 bits
    gate->offset_middle = addr >> 16;  // Setting the middle 16 bits
    gate->offset_high = addr >> 32;    // Setting the higher 32 bits
    // </STUDENT FILL>
}

unsigned long my_get_gate_offset(gate_desc *gate) {
    // <STUDENT FILL> - HINT: NO NEED FOR INLINE ASSEMBLY
    unsigned long addr = gate->offset_high;  // Getting the higher 32 bits
    addr = addr << 16;  // Shifting the value 16 bits to the left
    addr = addr + gate->offset_middle;  // Getting the middle 16 bits
    addr = addr << 16;               // Shifting the value 16 bits to the left
    addr = addr + gate->offset_low;  // Getting the lower 16 bits
    return addr;
    // </STUDENT FILL>
}
