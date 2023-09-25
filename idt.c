#include "idt.h"

/* idt_init()
 * 
 * Initializes the idt table entries
 * Inputs: None
 * Outputs: None
 * Side Effects: Sets the idt entries to the table
 * Coverage: IDT
 * Files: idt.c, idt.h, idt.s
 */
void idt_init() {
    int i;
    for (i = 0; i < NUM_VEC; i++) {
        /* Set all 64 bits to zero. */
        idt[i].val[0] = 0;
        idt[i].val[1] = 0;

        /* Interrupts happen in kernel code segment */
        idt[i].seg_selector = KERNEL_CS;

        /* Set idt entry to present if listed below */
        if (i <= EXCEPT_MAX || i == KEY_VAL || i == RTC_VAL || i == SYS_CALL_VAL || i == PIT_VAL) {
            idt[i].present = 1;
        }

        // Software interrupts require user priviledge level
        if (i == SYS_CALL_VAL) {
            idt[i].dpl = USER_PRIVILEGE;
        }

        // Size == 1 since gate is two 32 bits
        idt[i].size = 1;
        idt[i].reserved1 = 1; 
        idt[i].reserved2 = 1; 

        // System Call and Exceptions use TRAP gates
        if (i == SYS_CALL_VAL || i <= EXCEPT_MAX) {
            idt[i].reserved3 = 1;
        }
    }
    
    // Set the exception pointer for each element in the idt
    SET_IDT_ENTRY(idt[EXCEPT_0_VAL], exception_0_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_1_VAL], exception_1_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_2_VAL], exception_2_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_3_VAL], exception_3_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_4_VAL], exception_4_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_5_VAL], exception_5_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_6_VAL], exception_6_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_7_VAL], exception_7_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_8_VAL], exception_8_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_9_VAL], exception_9_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_10_VAL], exception_10_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_11_VAL], exception_11_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_12_VAL], exception_12_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_13_VAL], exception_13_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_14_VAL], exception_14_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_15_VAL], exception_15_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_16_VAL], exception_16_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_17_VAL], exception_17_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_18_VAL], exception_18_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_19_VAL], exception_19_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_20_VAL], exception_20_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_21_VAL], exception_21_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_22_VAL], exception_22_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_23_VAL], exception_23_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_24_VAL], exception_24_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_25_VAL], exception_25_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_26_VAL], exception_26_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_27_VAL], exception_27_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_28_VAL], exception_28_wrapper);
    SET_IDT_ENTRY(idt[EXCEPT_29_VAL], exception_29_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_30_VAL], exception_30_wrapper); 
    SET_IDT_ENTRY(idt[EXCEPT_31_VAL], exception_31_wrapper); 
    SET_IDT_ENTRY(idt[KEY_VAL], keyboard_wrapper);
    SET_IDT_ENTRY(idt[RTC_VAL], rtc_wrapper);
    SET_IDT_ENTRY(idt[SYS_CALL_VAL], system_call_wrapper);
    SET_IDT_ENTRY(idt[PIT_VAL], pit_wrapper);
    lidt(idt_desc_ptr);     //load idtr with the ptr to the idt
}

/* exception_{i}()
 * 
 * These are the Exception Handler Functions
 * Inputs: None
 * Outputs: None
 * Side Effects: prints on the console and goes into infinite loop
 * Coverage: None
 * Files: idt.c, idt.h, idt.s
 */
void exception_0() {
    printf("division error\n");           //Prints the corresponding error type
    sys_halt(HALT_CODE);                         
}
void exception_1() {
    printf("Debug Exception\n");
    sys_halt(HALT_CODE);
}
void exception_2() {
    printf("NMI interrupt exception\n");
    sys_halt(HALT_CODE);
}
void exception_3() {
    printf("breakpoint exception\n");
    sys_halt(HALT_CODE);
}
void exception_4() {
    printf("overflow exception\n");
    sys_halt(HALT_CODE);
}
void exception_5() {
    printf("BOUND Range Exceeded Exception\n");
    sys_halt(HALT_CODE);
}
void exception_6() {
    printf("Invalid opcode exception\n");
    sys_halt(HALT_CODE);
}
void exception_7() {
    printf("Device Not Available Exception (#NM)\n");
    sys_halt(HALT_CODE);
}
void exception_8() {
    printf("Double Fault Exception\n");
    sys_halt(HALT_CODE);
}
void exception_9() {
    printf("Coprocessor Segment Overrun\n");
    sys_halt(HALT_CODE);
}
void exception_10() {
    printf("Invalid TSS exception\n");
    sys_halt(HALT_CODE);
}
void exception_11() {
    printf("Segment not present\n");
    sys_halt(HALT_CODE);
}
void exception_12() {
    printf("Stack Fault Exception\n");
    sys_halt(HALT_CODE);
}
void exception_13() {
    printf("General Protection Exception\n");
    sys_halt(HALT_CODE);
}
void exception_14() {
    printf("Page Fault Exception\n");
    sys_halt(HALT_CODE);
}

// void exception_14(uint32_t error) {
//     printf("Page Fault Exception\n");
//     int32_t va;
//     asm volatile (
//         "movl %%cr2, %0;"
//         : "=r" (va)
//         :
//         : "eax", "cc"
//     );
//     printf("Failed to access memory at: %x\n", va);
//     printf("Page protection violation: %d\n", error&0x1);
//     printf("Write access if set, read if not: %d\n", (error&0x2)>>1);
//     printf("Caused in user level 3: %d\n", (error&0x4)>>2);
//     printf("Reserved page access: %d\n", (error&0x8)>>3);
//     printf("Instruction fetch failure: %d\n", (error&0xF)>>4);
//     while(1);
//     sys_halt(HALT_CODE);
// }

void exception_15() {
    printf("reserved\n");
    sys_halt(HALT_CODE);
}
void exception_16() {
    printf("x87 FPU Floating-Point\n");
    sys_halt(HALT_CODE);
}
void exception_17() {
    printf("Allignment check exception\n");
    sys_halt(HALT_CODE);
}
void exception_18() {
    printf("Machine Check Exception\n");
    sys_halt(HALT_CODE);
}
void exception_19() {
    printf("SIMD Floating-Point Exception\n");
    sys_halt(HALT_CODE);
}
void exception_20() {
    printf("Virtualization Exception\n");
    sys_halt(HALT_CODE);
}
void exception_21() {
    printf("Control prtection Exception\n");
    sys_halt(HALT_CODE);
}
void exception_22() {
    printf("Reserved\n");
    sys_halt(HALT_CODE);
}
void exception_23() {
    printf("Reserved\n");
    sys_halt(HALT_CODE);
}
void exception_24() {
    printf("Reserved\n");
    sys_halt(HALT_CODE);
}
void exception_25() {
    printf("Reserved\n");
    sys_halt(HALT_CODE);
}
void exception_26() {
    printf("Reserved\n");
    sys_halt(HALT_CODE);
}
void exception_27() {
    printf("Reserved\n");
    sys_halt(HALT_CODE);
}
void exception_28() {
    printf("Hypervisor Injection Exception\n");
    sys_halt(HALT_CODE);
}
void exception_29() {
    printf("VMM Communication Exception\n");
    sys_halt(HALT_CODE);
}
void exception_30() {
    printf("Security Exception\n");
    sys_halt(HALT_CODE);
}
void exception_31() {
    printf("Reserved\n");
    sys_halt(HALT_CODE);
}
