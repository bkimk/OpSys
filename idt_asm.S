#define ASM  1
#include "idt.h"

.text


# File Holding All Wrappers for Exceptions, Interrupts, and System Call (Assembly Linkage)

.globl   exception_0_wrapper
.align   4


# Exceptions Wrappers (0-31)

exception_0_wrapper:
    pushal                          # Push all registers to stack
    cld                             # OSDEV, clear Direction Flag
    call exception_0                # call C-program for exception 0
    popal                           # Pop all registers from stack
    iret                            # return for interrupts


.globl   exception_1_wrapper
.align   4

exception_1_wrapper:
    pushal
    cld    # OSDEV
    call exception_1
    popal
    iret


.globl   exception_2_wrapper
.align   4

exception_2_wrapper:
    pushal
    cld    # OSDEV
    call exception_2
    popal
    iret


.globl   exception_3_wrapper
.align   4

exception_3_wrapper:
    pushal
    cld    # OSDEV
    call exception_3
    popal
    iret


.globl   exception_4_wrapper
.align   4

exception_4_wrapper:
    pushal
    cld    # OSDEV
    call exception_4
    popal
    iret


.globl   exception_5_wrapper
.align   4

exception_5_wrapper:
    pushal
    cld    # OSDEV
    call exception_5
    popal
    iret


.globl   exception_6_wrapper
.align   4

exception_6_wrapper:
    pushal
    cld    # OSDEV
    call exception_6
    popal
    iret


.globl   exception_7_wrapper
.align   4

exception_7_wrapper:
    pushal
    cld    # OSDEV
    call exception_7
    popal
    iret


.globl   exception_8_wrapper
.align   4

exception_8_wrapper:
    pushal
    cld    # OSDEV
    call exception_8
    popal
    iret


.globl   exception_9_wrapper
.align   4

exception_9_wrapper:
    pushal
    cld    # OSDEV
    call exception_9
    popal
    iret


.globl   exception_10_wrapper
.align   4

exception_10_wrapper:
    pushal
    cld    # OSDEV
    call exception_10
    popal
    iret


.globl   exception_11_wrapper
.align   4

exception_11_wrapper:
    pushal
    cld    # OSDEV
    call exception_11
    popal
    iret


.globl   exception_12_wrapper
.align   4

exception_12_wrapper:
    pushal
    cld    # OSDEV
    call exception_12
    popal
    iret


.globl   exception_13_wrapper
.align   4

exception_13_wrapper:
    pushal
    cld    # OSDEV
    call exception_13
    popal
    iret


.globl   exception_14_wrapper
.align   4

exception_14_wrapper:
    pushal
    cld    # OSDEV
    call exception_14
    popal
    iret


.globl   exception_15_wrapper
.align   4

exception_15_wrapper:
    pushal
    cld    # OSDEV
    call exception_15
    popal
    iret


.globl   exception_16_wrapper
.align   4

exception_16_wrapper:
    pushal
    cld    # OSDEV
    call exception_16
    popal
    iret


.globl   exception_17_wrapper
.align   4

exception_17_wrapper:
    pushal
    cld    # OSDEV
    call exception_17
    popal
    iret


.globl   exception_18_wrapper
.align   4

exception_18_wrapper:
    pushal
    cld    # OSDEV
    call exception_18
    popal
    iret


.globl   exception_19_wrapper
.align   4

exception_19_wrapper:
    pushal
    cld    # OSDEV
    call exception_19
    popal
    iret


.globl   exception_20_wrapper
.align   4

exception_20_wrapper:
    pushal
    cld    # OSDEV
    call exception_20
    popal
    iret


.globl   exception_21_wrapper
.align   4

exception_21_wrapper:
    pushal
    cld    # OSDEV
    call exception_21
    popal
    iret


.globl   exception_22_wrapper
.align   4

exception_22_wrapper:
    pushal
    cld    # OSDEV
    call exception_22
    popal
    iret


.globl   exception_23_wrapper
.align   4

exception_23_wrapper:
    pushal
    cld    # OSDEV
    call exception_23
    popal
    iret


.globl   exception_24_wrapper
.align   4

exception_24_wrapper:
    pushal
    cld    # OSDEV
    call exception_24
    popal
    iret


.globl   exception_25_wrapper
.align   4

exception_25_wrapper:
    pushal
    cld    # OSDEV
    call exception_25
    popal
    iret


.globl   exception_26_wrapper
.align   4

exception_26_wrapper:
    pushal
    cld    # OSDEV
    call exception_26
    popal
    iret


.globl   exception_27_wrapper
.align   4

exception_27_wrapper:
    pushal
    cld    # OSDEV
    call exception_27
    popal
    iret


.globl   exception_28_wrapper
.align   4

exception_28_wrapper:
    pushal
    cld    # OSDEV
    call exception_28
    popal
    iret


.globl   exception_29_wrapper
.align   4

exception_29_wrapper:
    pushal
    cld    # OSDEV
    call exception_29
    popal
    iret


.globl   exception_30_wrapper
.align   4

exception_30_wrapper:
    pushal
    cld    # OSDEV
    call exception_30
    popal
    iret


.globl   exception_31_wrapper
.align   4

exception_31_wrapper:
    pushal
    cld    # OSDEV
    call exception_31
    popal
    iret

# Wrapper for keyboard_hander
.globl   keyboard_wrapper
.align   4

keyboard_wrapper:
    pushal
    cld    # OSDEV
    call keyboard_handler
    popal
    iret

# Wrapper for rtc_handler
.globl   rtc_wrapper
.align   4

rtc_wrapper:
    pushal
    cld    # OSDEV
    call rtc_handler
    popal
    iret

# Wrapper for pit_handler
.globl   pit_wrapper
.align   4

pit_wrapper:
    pushal
    cld    # OSDEV
    call pit_handler
    popal
    iret

# Wrapper for system_call
.globl   system_call_wrapper
.align   4

system_call_wrapper:

    # push_all

    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx

    cld    # OSDEV
    cmpl $10, %eax       # Make sure sys call number is valid
    jg invalid_num
    cmpl $1, %eax       # Make sure sys call number is valid
    jl invalid_num
    subl $1, %eax       
    call *sys_call_tbl(,%eax,4)     # Length of Addr
    
    # pop_all
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    iret

invalid_num:
    movl $-1, %eax      # Return Error Code
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    iret

.globl   sys_call_tbl
.align   4

sys_call_tbl:
    .long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, set_handler, sigreturn
    
