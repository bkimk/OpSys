#ifndef _PROCESS_EXECUTE_H
#define _PROCESS_EXECUTE_H
#ifndef ASM

#include "types.h"
#include "lib.h"
#include "file_sys.h"
#include "paging.h"
#include "keyboard.h"
#include "system_call.h"

#define PROCESSNUM          6
#define FILENUM             8
#define EIGHTMB             0x00800000
#define FOURMB              0x00400000
#define EIGHTKB             0x00002000
#define MB128               128/4
#define NUM_OF_FILES        17
#define USER_FILE_VIRT_LOC  0x08048000
#define USER_ESP            0x08000000+0x00400000-0x04
#define E_FLAG              0x0200
#define MAXWORD             128
#define FOURBYTE            4
#define PHYS_8MB_OFF        2
#define INSTR_RD_OFF        24
#define DS_                 0x2B
#define MAXNAME             32
#define HALT_CODE           256
#define EXE_CHECK_1         0x7f
#define EXE_CHECK_2         0x45
#define EXE_CHECK_3         0x4c
#define EXE_CHECK_4         0x46

// Stringizing
#define XSTR(x)  #x
#define STR(x)  XSTR(x)

// PCB Struction temporary need to change syntax
typedef struct pcb_t {
    int8_t process_id;
    int8_t parent_id;
    file_t file_desc[FILENUM];
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint8_t active;
    uint8_t argument[MAXNAME];
} pcb_t;

// PCB global arr
pcb_t* globalPCB[PROCESSNUM];

// halt program
void program_halt(int8_t status);

// execute program
int32_t program_execute(const int8_t* command);

int32_t get_free_pid();

#endif
#endif
