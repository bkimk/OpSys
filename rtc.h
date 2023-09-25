#ifndef _RTC_H
#define _RTC_H
#ifndef ASM
#include "x86_desc.h"
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "file_sys.h"
#include "process_execute.h"

#define RTC_PORT        0x70    // RTC, first port number
#define RTC_REG_A       0x8A    // turn off NMI, access register A
#define RTC_REG_B       0x8B    // turn off NMI, access register B
#define RTC_REG_C_NMI   0x0C    // don't turn off NMI, access register C
#define IRQ_RTC     8
#define BIT_6_MASK  0x40
#define SET_FREQ_BITMASK 0xF0

// standard rtc freq
#define RTC_FREQ_DEF    1024
// user rtc freq
#define RTC_FREQ_USER    2

// Initialize the RTC
void rtc_init(void);
// Handler for RTC interrupts
void rtc_handler(void);

// system call functions for rtc driver
// wait until next rtc interrupt
int32_t read_rtc(file_t* file, int8_t* buf, int32_t nbytes);
// change the frequenct of the rtc interrupts
int32_t write_rtc(file_t* rtc, int8_t* buf, int32_t nbyte);
// open the rtc file and reset frequency
int32_t open_rtc(file_t* file);
// close the rtc file and reset frequency
int32_t close_rtc(file_t* file);

extern file_table_t rtc_file;

// rtc data structure
typedef struct rtc_data_t{
    uint32_t freq;
    uint32_t offset;
} rtc_data_t;

#endif /* ASM */
#endif /* _RTC_H */
