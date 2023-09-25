#define PASS 1
#define FAIL 0
#define kernelPageStart 0x400000
#define kernelPageEnd 0x800000
#define videoMemoryStart 0xB8000
#define videoMemoryEnd 0xB9000
#define RANDOMPAGEMEM 0x555555
#define RANDOMVIDEOMEM 0xB8555
#define NUM_OF_FILES 17
#define VID_MAP_START 0x8400000

#include "tests.h"

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* kernelTest
 * 
 * Asserts that one memory location above kernelPage upper bound and one below kernelPage lower bound cannot be accessed
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: call page fault exception
 * Coverage: none
 * Files: idt.h, idt.S, idt.c
 */
int kernelTest() {
	TEST_HEADER;

	int i;

	//Comment out bound you don't want to test; Should page fault
	// char upperOut = *((char*)(kernelPageStart-1));				//One mem location above upper bound; page fault
	// char lowerOUT = *((char*)(kernelPageEnd));				//One mem location beneath lower bound; page fault

	//Testing both inside bounds + random mem inside; shouldnt page fault
	for (i = kernelPageStart; i < kernelPageEnd; i++) {
		// char noPageFault = *((char*)(i));
	}

	return PASS;
}


/* videoMemoryTest
 * 
 * Asserts that memory location 1 above and below videoMemory cannot be accessed
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: call page fault exception
 * Coverage: none
 * Files: idt.h, idt.S, idt.c
 */
int videoMemoryTest() {
	TEST_HEADER;

	int i;

	//Comment out bound you don't want to test; Should page fault
	// char upperOut = *((char*)(videoMemoryStart-1));					//One mem location above upper bound; page fault
	// char lowerOUT = *((char*)(videoMemoryEnd));						//One mem location beneath lower bound; page fault

	//Testing both inside bounds + random mem inside; shouldnt page fault
	for (i = videoMemoryStart; i < videoMemoryEnd; i++) {
		// char noPageFault = *((char*)(i));
	}

	return PASS;
}

/* nullTest
 * 
 * Asserts that cannot dereference null pointer
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: call page fault exception
 * Coverage: none
 * Files: idt.h, idt.S, idt.c
 */
int nullTest() {
	TEST_HEADER;

	// int *ptr = NULL;
	// int ptr2 = *ptr;	//Should call page fault

	return PASS;
}

/* Exceptions Test
 * 
 * Tests if exceptions are called properly
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: prints exception and goes into infinite while loop
 * Coverage: None
 * Files: idt.h, idt.S, idt.c
 */
int exceptions_test(){ 
  
	//Comment everything except for exception that you want to test
	asm volatile("int $0x00");			//except_0_val			
	asm volatile("int $0x01");			//except_1_val
	asm volatile("int $0x02");			//except_2_val 
	asm volatile("int $0x03");			//except_3_val
	asm volatile("int $0x04");			//except_4_val 
	asm volatile("int $0x05");			//except_5_val 
	asm volatile("int $0x06");			//except_6_val 
	asm volatile("int $0x07");			//except_7_val 
	asm volatile("int $0x08");			//except_8_val 
	asm volatile("int $0x09");			//except_9_val 
	asm volatile("int $0x0A");			//except_10_val
	asm volatile("int $0x0B");			//except_11_val
	asm volatile("int $0x0C");			//except_12_val
	asm volatile("int $0x0D");			//except_13_val
	asm volatile("int $0x0E");			//except_14_val
	asm volatile("int $0x0F");			//except_15_val
	asm volatile("int $0x10");			//except_16_val			
	asm volatile("int $0x11");			//except_17_val
	asm volatile("int $0x12");			//except_18_val 
	asm volatile("int $0x13");			//except_19_val
	asm volatile("int $0x14");			//except_20_val 
	asm volatile("int $0x15");			//except_21_val 
	asm volatile("int $0x16");			//except_22_val 
	asm volatile("int $0x17");			//except_23_val 
	asm volatile("int $0x18");			//except_24_val 
	asm volatile("int $0x19");			//except_25_val 
	asm volatile("int $0x1A");			//except_26_val
	asm volatile("int $0x1B");			//except_27_val
	asm volatile("int $0x1C");			//except_28_val
	asm volatile("int $0x1D");			//except_29_val
	asm volatile("int $0x1E");			//except_30_val
	asm volatile("int $0x1F");			//except_31_val
	asm volatile("int $0x80");			//system call (cp1)

	return PASS;
}

// /* Checkpoint 2 tests */

// /* rd_dentry_name_test()
//  * Tests if we can correctly read the file name by comparing string to 
//  * actual file name
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: prints string of file name we are trying to find and
//  *					the actual file name that we found
//  */
// int rd_dentry_name_test() {
// 	int i, j, temp;
// 	char* namelist[NUM_OF_FILES] = {					//file names that we are going to find
// 		".", "sigtest", "shell",
// 		"grep", "syserr", "rtc",
// 		"fish", "counter", "pingpong",
// 		"cat", "frame0.txt", "verylargetextwithverylongname.txt",
// 		"ls", "testprint", "created.txt", 
// 		"frame1.txt", "hello"
// 	};
// 	dentry_t dentry;
// 	for(i = 0; i < NUM_OF_FILES; i++){				//iterate through namelist to find each file
// 		temp = read_dentry_by_name((int8_t*)namelist[i], &dentry);	//get dentry that has the file name
// 		if (temp == -1) {					//if read fails, return FAIL
// 			return FAIL;
// 		}
// 		printf("Search File Name: %s \n", namelist[i]);	//print the filename that we are searching for
// 		printf("Found File Name:");						
// 		for (j = 0; j < FILENAME_LEN; j++) {						//print all the characters that we found
// 			printf("%c", dentry.filename[j]); 
// 		}
// 	printf("\n");
// 	}
	
// 	return PASS;				//return pass if not failed
// }

// /* rd_dentry_index_pos_test()
//  * Test if we can read dentry by index and test for correct test case. In addition, print file type
//  * as well as corresponding length of file
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: goes through every file by index and print the file name,
//  * 					file type and file size.
//  */
// int rd_dentry_index_pos_test() {
// 	dentry_t dentry;
// 	int i, j, temp;
// 	inode_t* cur_inode;
// 	for (i = 0 ; i < NUM_OF_FILES ; i++) {			//iterate through each file
// 		temp = read_dentry_by_index(i, &dentry);	//read each file by its index
// 		if (temp == -1) {							//return FAIL if read fail
// 			return FAIL;
// 		}
// 		printf("file_name: ");
// 		for (j = 0; j < FILENAME_LEN; j++) {		//print file name string by each character
// 			printf("%c", dentry.filename[j]);
// 		}
// 		printf(", file_type: %d, file_size: ", dentry.filetype);	//print the filetype
// 		cur_inode = &file_sys.inode_blocks[dentry.inode_num];		//get the file size and print it
// 		printf("%d", cur_inode->length);							//print corresponding length
// 	printf("\n");
// 	}
// 	return PASS;
// }

// /* rd_dentry_index_neg_test()
//  * Test if it fails out of bounds
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: prints that the index is out of bound
//  */
// int rd_dentry_index_neg_test() {
// 	dentry_t dentry;								
// 	int j, temp;
// 	printf("At index 63, it is out of bound.\n");	//Notifying that this test will try to read dentry at 63
// 	temp = read_dentry_by_index(63, &dentry);		//Trying to grab the dentry address
// 	if (temp == -1) {								//The read_dentry_by_index should be returning -1
// 		return PASS;								//Meaning that it PASSED the NEGATIVE TEST
// 	}
// 	for (j = 0; j < FILENAME_LEN; j++) {
// 		printf("%c", dentry.filename[j]);
// 	}
// 	printf("\n");
// 	return FAIL;									//This means that it read the dentry at the index 63
// }

// /* rd_data_test()
//  * Reads the data
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: Prints out the read data onto the screen
//  */
// int rd_data_test() {
// 	dentry_t dentry;
// 	int temp, result;
// 	char* namelist[NUM_OF_FILES] = {				//User can choose which file to test for reading
// 		".", "sigtest", "shell",
// 		"grep", "syserr", "rtc",
// 		"fish", "counter", "pingpong",
// 		"cat", "frame0.txt", "verylargetextwithverylongname.txt",
// 		"ls", "testprint", "created.txt", 
// 		"frame1.txt", "hello"
// 	};
// 	// int array below is the corresponding file size in bytes (corresponds according to each file index)
// 	// For instance, 10 = fram0.txt; 2 = shell; 11 = verylargetextwithverylongname.txt
// 	int charCount[NUM_OF_FILES] = {
// 		0, 6224, 5605, 6149, 8037, 0, 36164, 5605, 5445, 5445, 187, 5277, 5349, 5189, 21, 174, 5349
// 	};
// 	int i = 10;												// Change index based on which file you want to print out
// 	int32_t size = charCount[i];							// get file size based on index
// 	int8_t buf[size];
// 	temp = read_dentry_by_name(namelist[i], &dentry);			// read_dentry_by_name for corresponding file
// 	uint32_t inode = dentry.inode_num;
// 	result = read_data(inode, 0, buf, size);					// read_data on all characters inside file and put it inside buffer
// 	for (i = 0 ; i < result ; i++) {
// 			printf("%c", buf[i]);								//iterate through buffer to print all charactres
// 	}
// 	printf("\nfile_name:");
// 	int j;
// 	for (j = 0; j < FILENAME_LEN; j++) {		//print file name string by each character
// 		printf("%c", dentry.filename[j]);
// 	}
// 	printf("\ndata bytes copied: %d\n", result);
// 	if (result != size) return FAIL;
// 	return PASS;
// }

// /* rd_data_middle()
//  * Reads the data
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: Prints test case outputs and chunks of file text
//  */
// int rd_data_middle() {
// 	dentry_t dentry;
// 	int i;
// 	int temp, result;
// 	int8_t buf[200];										// create large buffer to be reused
// 	temp = read_dentry_by_name("verylargetextwithverylongname.txt", &dentry);			// read_dentry_by_name for corresponding file
// 	uint32_t inode = dentry.inode_num;
// 	result = read_data(inode, 5200, buf, 77);				// read_data on characters from 5200 to the end
// 	printf("Reading final 77 characters of 'verylargetextwithverylongname.txt':\n" );
// 	for (i = 0 ; i < 77 ; i++) {							// 77 characters read
// 			printf("%c", buf[i]);							// iterate through buffer to print all charactres copied
// 	}
// 	printf("\ndata bytes copied: %d\n", result);
// 	printf("\n");
// 	if (result != 77) return FAIL;							// check result is equal to number of bytes copied

// 	result = read_data(inode, 4094, buf, 50);				// reading the middle over block boundary
// 	printf("Reading random 50 characters of 'verylargetextwithverylongname.txt' over first block boundary:\n"  );
// 	for (i = 0 ; i < 50 ; i++) {							// 50 characters read
// 		printf("%c", buf[i]);								// iterate through buffer to print all charactres
// 	}
// 	printf("\ndata bytes copied: %d\n", result);
// 	if (result != 50) return FAIL;							// check result is equal to number of bytes copied

// 	result = read_data(inode, 5277, buf, 50);				// reading from the character after the last in the file
// 	printf("Reading starting from end of 'verylargetextwithverylongname.txt':\n"  );
// 	printf("\ndata bytes copied: %d\n", result);
// 	if (result != 0) return FAIL;							// check result is equal to number of bytes copied

// 	result = read_data(inode, 6314, buf, 50);				// reading from beyond the end of the file
// 	printf("Reading starting from past end of 'verylargetextwithverylongname.txt':\n"  );
// 	printf("\ndata bytes copied: %d\n", result);
// 	if (result != 0) return FAIL;							// check result is equal to number of bytes copied

// 	return PASS;
// }

// int reading_more_test() {
// 	dentry_t dentry;
// 	int temp, result;
// 	char* namelist[NUM_OF_FILES] = {				//User can choose which file to test for reading
// 		".", "sigtest", "shell",
// 		"grep", "syserr", "rtc",
// 		"fish", "counter", "pingpong",
// 		"cat", "frame0.txt", "verylargetextwithverylongname.txt",
// 		"ls", "testprint", "created.txt", 
// 		"frame1.txt", "hello"
// 	};
// 	int i = 10;												// Change index based on which file you want to print out
// 	int32_t size = 200;											// manually set size to 200 to go over actual file size in bytes
// 	int8_t buf[size];
// 	temp = read_dentry_by_name(namelist[i], &dentry);
// 	uint32_t inode = dentry.inode_num;
// 	result = read_data(inode, 0, buf, size);					// results should be 0 since size greater then actual file size
// 	if (result == 0) {
// 		return PASS;
// 	}
// 	return FAIL;
// }
// /* terminal_read_test()
//  * Test if the terminal_read functions correctly reads in keyboard input
//  * into a buffer
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: prints terminal input request to the terminal
//  *				 as well as outputs size of message typed and keys pressed
//  */
// int terminal_read_test(){
// 	// write input request to terminal
// 	int8_t buf[NUM_KEYS];
// 	int8_t buf1[24] = "Reading from terminal\n$ ";
// 	int bytes_read;
// 	terminal_write(NULL, buf1, 24);
// 	// read bytes from terminal
// 	bytes_read = terminal_read(NULL, buf, NUM_KEYS);
// 	// print out what is read
// 	printf("\n");
// 	printf("Chars read: %d      Message read: ", bytes_read);
// 	terminal_write(NULL, buf, bytes_read);
// 	printf("\n");
// 	return PASS;
// }

// /* terminal_write_test()
//  * Test if the terminal_write functions correctly writes to the terminal
//  * from a buffer
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: prints buffer input to terminal
//  */
// int terminal_write_test(){
// 	// write input request to terminal
// 	int nbytes;
// 	nbytes = 140;
// 	int8_t buf1[140] = "Printing a very long string of input to the terminal to test the terminal_write function and make sure that it does what it is supposed to.\n";
// 	terminal_write(NULL, buf1, nbytes);
// 	return PASS;
// }

// /* rtc_test()
//  * Test if the rtc correctly changes frequency using read/write/open/close
//  * Inputs: None
//  * Outputs: PASS/FAIL
//  * Side Effects: prints out 1s to keyboard based on a changing rtc rate
//  */
// int rtc_test(){
// 	// declare variables
// 	int i, j, ret_code, fd;
// 	i = 0;
// 	int8_t buf[1];
// 	// Set rtc to open rate
// 	fd = open_rtc("rtc");
// 	int32_t buf1[1];
// 	// Print out 1's and then change frequency
// 	for (j = 1; j <= 0x1000; j*=2){										// change the frequency through a variety of values
// 		printf("Freq: %d\n", j);
// 		buf1[0] = j;
// 		ret_code = write_rtc(NULL, (int8_t*)buf1, 4);					// 4 bytes is length of uint32_t type
// 		if (ret_code == 4) {											// 4 bytes written is desired amount
// 			i = 0;
// 			while(i < 10)	{read_rtc(NULL, buf, 1); putc('1'); i++; }	// write 10 1's
// 		}
// 		else if (ret_code == -1) {										// incorrect frequency was inputted
// 			printf("write_RTC returned -1, so invalid frequency was entered.");
// 		}
// 		putc('\n');
// 		putc('\n');
// 	}
// 	// Test frequency values in the range from -1 to 4
// 	for (j = -1; j <= 5; j++){											// change the frequency through a variety of values
// 		printf("Freq: %d\n", j);
// 		buf1[0] = j;
// 		ret_code = write_rtc(NULL, (int8_t*)buf1, 4);					// 4 bytes is length of uint32_t type
// 		if (ret_code == 4) {											// 4 bytes written is desired amount
// 			i = 0;
// 			while(i < 10)	{read_rtc(NULL, buf, 1); putc('1'); i++; }	// write 10 1's
// 		}
// 		else if (ret_code == -1) {										// incorrect frequency was inputted
// 			printf("write_RTC returned -1, so invalid frequency was entered.");
// 		}
// 		putc('\n');
// 		putc('\n');
// 	}
// 	// Retu
// 	close_rtc(NULL);
// 	return PASS;
// }


// /* Checkpoint 3 tests */
// int system_call_test(){
// 	int8_t buf[128];
// 	while(1){
// 		printf("Running system call test");
// 		int nbytes;
// 		nbytes = read(0, buf, 128);
// 		write(1, buf, nbytes);
// 	}
// 	return PASS;
// }

// int syscall_file_read(){
// 	int8_t buf[100];
// 	int fd, retcode;
// 	fd = open("verylargetextwithverylongname.txt");
// 	retcode = read(fd, buf, 100);
// 	if (retcode != 100) return FAIL;
// 	write(1, buf, 100);
// 	return PASS;
// }

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	/* Checkpoint 1 tests */
	// TEST_OUTPUT("idt_test", idt_test());

	// Comment out tests you dont want to test
	// TEST_OUTPUT("kernel_test", kernelTest());				//Asserts that one memory location above kernelPage upper bound and one below kernelPage lower bound cannot be accessed
	// TEST_OUTPUT("video_memory_test", videoMemoryTest());	//Asserts that memory location 1 above and below videoMemory cannot be accessed
	// TEST_OUTPUT("null_test", nullTest());					//Asserts that cannot dereference null pointer
	// TEST_OUTPUT("exceptions_test", exceptions_test());			//Tests if exceptions are called properly

	/* Checkpoint 2 tests */
	// TEST_OUTPUT("rd_dentry_name_test", rd_dentry_name_test());
	// TEST_OUTPUT("rd_dentry_index_pos_test", rd_dentry_index_pos_test());
	// TEST_OUTPUT("rd_dentry_index_neg_test", rd_dentry_index_neg_test());
	// TEST_OUTPUT("rd_data_test", rd_data_test());
	// TEST_OUTPUT("rd_data_middle", rd_data_middle());
	// TEST_OUTPUT("reading_more_test", reading_more_test());
	// TEST_OUTPUT("rtc_test", rtc_test());
	// TEST_OUTPUT("terminal_write_test", terminal_write_test());
	// syscall_file_read();
	return;
}
