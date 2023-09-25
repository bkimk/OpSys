# x86_desc.S - Set up x86 segment descriptors, descriptor tables
# vim:ts=4 noexpandtab

#define ASM     1
#include "x86_desc.h"
#define NUM_DIR_TBL    1024

.text

.globl ldt_size, tss_size
.globl gdt_desc, ldt_desc, tss_desc
.globl tss, tss_desc_ptr, ldt, ldt_desc_ptr
.globl gdt_ptr
.globl idt_desc_ptr, idt

.align 4


tss_size:
    .long tss_bottom - tss - 1

ldt_size:
    .long ldt_bottom - ldt - 1

    .word 0 # Padding
ldt_desc:
    .word KERNEL_LDT
    .long ldt

    .align 4

# Specify the value to put into GDTR, ultimately to load the GDT
gdt_desc:
    .word gdt_bottom - gdt - 1 # limit a.k.a gdt_size
    .long gdt                  # gdt address


    .align  16

tss:
_tss:
    .rept 104
    .byte 0
    .endr
tss_bottom:

    .align  16
gdt:
_gdt:

    # First GDT entry cannot be used
    .quad 0

    # NULL entry
    .quad 0

    # Segmentation will not be used
    # CS and DS both are 0-4GB r/w segments
    #
    # The layout is (from Intel IA-32 reference manual):
    #  31        24 23  22  21  20  19   16 15  14 13 12  11   8 7          0
    # |----------------------------------------------------------------------|
    # |            |   | D |   | A |  Seg  |   |  D  |   |      |            |
    # | Base 31:24 | G | / | 0 | V | Limit | P |  P  | S | Type | Base 23:16 |
    # |            |   | B |   | L | 19:16 |   |  L  |   |      |            |
    # |----------------------------------------------------------------------|
    #
    # |----------------------------------------------------------------------|
    # |                                    |                                 |
    # | Base 15:0                          | Segment Limit 15:0              |
    # |                                    |                                 |
    # |----------------------------------------------------------------------|

gdt_ptr:
    # Set up an entry for kernel CS
    .quad 0x00CF9A000000FFFF

    # Set up an entry for kernel DS
    .quad 0x00CF92000000FFFF

    # Set up an entry for user CS
    .quad 0x00CFFA000000FFFF

    # Set up an entry for user DS
    .quad 0x00CFF2000000FFFF

    # Set up an entry for TSS
tss_desc_ptr:
    .quad 0

    # Set up one LDT
ldt_desc_ptr:
    .quad 0

gdt_bottom:

    .align 16
ldt:
    .rept 4
    .quad 0
    .endr
ldt_bottom:

.align 4
    .word 0 # Padding
idt_desc_ptr:
    .word idt_bottom - idt - 1
    .long idt


    .align  16
idt:
_idt:
    .rept NUM_VEC
    .quad 0
    .endr

idt_bottom:



.globl loadPageDirectory
.globl enablePaging

# The final step is to actually enable paging. First we
# tell the processor where to find our page directory by 
# putting it's address into the CR3 register. Because C code cannot 
# directly access the computer's registers, we will need to use 
# assembly code to access CR3. The following assembly is written for 
# GAS. If you use a different assembler then you will need to 
# translate between this assembly format and the format supported by 
# your assembler.

loadPageDirectory:
    push %ebp
    mov %esp, %ebp
    mov 8(%esp), %eax       # getting page directory address from two bytes down
    mov %eax, %cr3
    leave
    ret


# This small assembly function takes one parameter: the address 
# of the page directory. It then loads the address onto the CR3 
# register, where the MMU will find it. But wait! Paging is not 
# still enabled. That's what we will do next. We must set the 32th 
# bit in the CR0 register, the paging bit. This operation also 
# requires assembly code. Once done, paging will be enabled.

enablePaging:
    push %ebp
    mov %esp, %ebp

# setting PSE = 1, allowing for usage of 4kb and 4mb pages
    mov %cr4, %eax
    or $0x0000010, %eax     # oring second bit with one
    mov %eax, %cr4

# setting the paging bit = 1
    mov %cr0, %eax
    or $0x80000000, %eax    # oring most significant bit with one
    mov %eax, %cr0
    leave
    ret
