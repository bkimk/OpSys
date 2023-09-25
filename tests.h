#ifndef TESTS_H
#define TESTS_H

#include "keyboard.h"
#include "x86_desc.h"
#include "lib.h"
#include "file_sys.h"
#include "rtc.h"
#include "idt.h"
#include "paging.h"


// test launcher
void launch_tests();


/* Checkpoint 1 Tests */

// idt test
int idt_test();

// kernel test
int kernelTest();

// video memory test
int videoMemoryTest();

// dereferencing null pointer test
int nullTest();

// exception test
int exceptions_test();


/* Checkpoint 2 Tests */

// reading dentry by name test
int rd_dentry_name_test();

// reading dentry by index pos & neg test
int rd_dentry_index_pos_test();
int rd_dentry_index_neg_test();

// reading data from inodes
int rd_data_test();
int reading_more_test();

// terminal read/write tests
int terminal_read_test();
int terminal_write_test();

// rtc freq tests
int rtc_test();


/* Checkpoint 3 Tests */



/* Checkpoint 4 Tests */



/* Checkpoint 5 Tests */




#endif /* TESTS_H */
