#ifndef _SYSTEM_CALL_H
#define _SYSTEM_CALL_H
#ifndef ASM

#include "keyboard.h"
#include "rtc.h"
#include "process_execute.h"
#include "paging.h"

#define LEN_RTC 4
#define LEN_STDIN 6
#define LEN_STDOUT 7
#define VIDEO   0xB8000
#define S_START 0x8400000
#define USER_START 0x8000000
#define USER_END 0x83FFFFD
//Declaration of sys_call functions

void sys_halt(int32_t status);

int32_t sys_execute(int8_t* cmmd);

int32_t sys_read(uint32_t fd, int8_t* buf, uint32_t nbyte);

int32_t sys_write(uint32_t fd, int8_t* buf, uint32_t nbyte);

int32_t sys_open(int8_t* filename);

int32_t sys_close(uint32_t fd);

int32_t sys_getargs(uint8_t* buf, int32_t nbytes);

int32_t sys_vidmap(uint8_t** screen_start);

int32_t set_handler(int32_t signum, void* handler_address);

int32_t sigreturn(void);

#endif /* ASM */
#endif /* _SYSTEM_CALL_H */
