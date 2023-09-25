#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#ifndef ASM

#include "process_execute.h"
#define PIT_INIT_CODE 0x36
#define PIT_FREQ_LOW 0x42
#define PIT_FREQ_HIGH 0x09
#define PIT_CMD_REG 0x43
#define PIT_DATA_PORT_0 0x40

void pit_init();

void pit_handler(void);

void scheduler();

#endif
#endif
