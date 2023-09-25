#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#ifndef ASM

#include "x86_desc.h"
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "file_sys.h"
#include "process_execute.h"
#include "system_call.h"
#include "idt.h"
// port number
#define KBD_PORT        0x60
// number of keys
#define NUM_KEYS        128
#define IRQ_KBD         1
#define FOUR_KB         0x1000
#define VID_KB_OFF      0xb8
#define TERM_NUM        3
#define LOW_4BIT        0x0F
#define HIGH_4BIT       0xF0
#define ALL_8BIT        0xFF
#define F1              17
#define F3              19

// scan codes for special characters
// _r is for released characters
#define SCAN_CL         0x3A
#define SCAN_CL_R       0xBA
#define SCAN_R_SHIFT    0x36
#define SCAN_L_SHIFT    0x2A
#define SCAN_R_SHIFT_R  0xB6
#define SCAN_L_SHIFT_R  0xAA
#define SCAN_R_CTRL     0x1D
#define SCAN_R_CTRL_R   0x9D
#define SCAN_L_CTRL     0x1D
#define SCAN_L_CTRL_R   0x9D
#define SCAN_L_ALT      0x38
#define SCAN_L_ALT_R    0xB8

#define TAB_LEN          4

// struct of the keyboard specific variables
typedef struct keyboard_t {
    uint8_t shift_pressed;
    uint8_t ctrl_pressed;
    uint8_t alt_pressed;
    uint8_t cl_pressed;
    uint8_t cl_on;
} keyboard_t;

// declaring instance of terminal and keyboard
keyboard_t keyboard;

// Handle keyboard interrupt to read from port and print character
void keyboard_handler(void);

// initialize keyboard by enabling IRQ
void keyboard_init();

// system call functions
// open the terminal and set variables
int32_t terminal_open(file_t* file);
// close the terminal file
int32_t terminal_close(file_t* file);
// write buffer to the terminal
int32_t terminal_write(file_t* file, int8_t* buf, int32_t nbytes);
// read from keyboard presses to the buffer
int32_t terminal_read(file_t* file, int8_t* buf, int32_t nbytes);
// boots up all three terminals
void terminal_boot();
// switch terminal
void terminal_switch(uint32_t num);

// declare stdin and stdout
file_table_t stdin_file;
file_table_t stdout_file;

#endif /* ASM */
#endif /* _KEYBOARD_H */
