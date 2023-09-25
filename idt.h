#ifndef _IDT_H
#define _IDT_H

#include "x86_desc.h"
#include "system_call.h"
#include "process_execute.h"

#ifndef ASM

#define IRQ_OFFSET  0x20

#define EXCEPT_MAX      0x1F
#define KEY_VAL         0x21
#define RTC_VAL         0x28
#define PIT_VAL         0x20
#define SYS_CALL_VAL    0x80
#define USER_PRIVILEGE  3
#define EXCEPT_0_VAL    0x00
#define EXCEPT_1_VAL    0x01
#define EXCEPT_2_VAL    0x02
#define EXCEPT_3_VAL    0x03
#define EXCEPT_4_VAL    0x04
#define EXCEPT_5_VAL    0x05
#define EXCEPT_6_VAL    0x06
#define EXCEPT_7_VAL    0x07
#define EXCEPT_8_VAL    0x08
#define EXCEPT_9_VAL    0x09
#define EXCEPT_10_VAL   0x0A
#define EXCEPT_11_VAL   0x0B
#define EXCEPT_12_VAL   0x0C
#define EXCEPT_13_VAL   0x0D
#define EXCEPT_14_VAL   0x0E
#define EXCEPT_15_VAL   0x0F
#define EXCEPT_16_VAL   0x10
#define EXCEPT_17_VAL   0x11
#define EXCEPT_18_VAL   0x12
#define EXCEPT_19_VAL   0x13
#define EXCEPT_20_VAL   0x14
#define EXCEPT_21_VAL   0x15
#define EXCEPT_22_VAL   0x16
#define EXCEPT_23_VAL   0x17
#define EXCEPT_24_VAL   0x18
#define EXCEPT_25_VAL   0x19
#define EXCEPT_26_VAL   0x1A
#define EXCEPT_27_VAL   0x1B
#define EXCEPT_28_VAL   0x1C
#define EXCEPT_29_VAL   0x1D
#define EXCEPT_30_VAL   0x1E
#define EXCEPT_31_VAL   0x1F

/* IDT initialization */
void idt_init();

/* All types of exceptions (C-program) */
void exception_0();
void exception_1();
void exception_2();
void exception_3();
void exception_4();
void exception_5();
void exception_6();
void exception_7();
void exception_8();
void exception_9();
void exception_10();
void exception_11();
void exception_12();
void exception_13();
void exception_14();
void exception_15();
void exception_16();
void exception_17();
void exception_18();
void exception_19();
void exception_20();
void exception_21();
void exception_22();
void exception_23();
void exception_24();
void exception_25();
void exception_26();
void exception_27();
void exception_28();
void exception_29();
void exception_30();
void exception_31();
void system_call();

/* All types of exception wrappers for Assembly Linkage (ASM) */
extern void exception_0_wrapper(void);
extern void exception_1_wrapper(void);
extern void exception_2_wrapper(void);
extern void exception_3_wrapper(void);
extern void exception_4_wrapper(void);
extern void exception_5_wrapper(void);
extern void exception_6_wrapper(void);
extern void exception_7_wrapper(void);
extern void exception_8_wrapper(void);
extern void exception_9_wrapper(void);
extern void exception_10_wrapper(void);
extern void exception_11_wrapper(void);
extern void exception_12_wrapper(void);
extern void exception_13_wrapper(void);
extern void exception_14_wrapper(void);
extern void exception_15_wrapper(void);
extern void exception_16_wrapper(void);
extern void exception_17_wrapper(void);
extern void exception_18_wrapper(void);
extern void exception_19_wrapper(void);
extern void exception_20_wrapper(void);
extern void exception_21_wrapper(void);
extern void exception_22_wrapper(void);
extern void exception_23_wrapper(void);
extern void exception_24_wrapper(void);
extern void exception_25_wrapper(void);
extern void exception_26_wrapper(void);
extern void exception_27_wrapper(void);
extern void exception_28_wrapper(void);
extern void exception_29_wrapper(void);
extern void exception_30_wrapper(void);
extern void exception_31_wrapper(void);

/* Hardware Interrupt wrappers for Assembly Linkage (ASM) */
extern void keyboard_wrapper(void);
extern void rtc_wrapper(void);
extern void pit_wrapper(void);

/* System Call wrapper for Assembly Linkage (ASM) */
extern void system_call_wrapper(void);

#endif /* ASM */
#endif /* _IDT_H */
