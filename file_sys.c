#include "file_sys.h"

/* FS Function Pointers */
file_table_t fs_file = {
    .read = file_read,
    .write = file_write,
    .open = file_open,
    .close = file_close
};

/* Directory Function Pointers */
file_table_t dir_file = {
    .read = dir_read,
    .write = dir_write,
    .open = dir_open,
    .close = dir_close
};

/* dir_read
 * 
 * Reads all the characters from the correct dentry that has the file name and 
 * put it into a buffer
 * Inputs: file_t* file, int8_t* buf, int32_t nbytes
 * Outputs: number of bytes
 * Side Effects: None
 */
int32_t dir_read(file_t* file, int8_t* buf, int32_t nbytes){
    file_t* f = file;
    if (f->f_pos >= file_sys.boot_block->dir_count)
        return 0;
    int i, temp;
	dentry_t dentry;
    temp = read_dentry_by_index(f->f_pos, &dentry);	//get dentry that has the file name
    if (temp == -1) {					//if read fails, return 0
        return 0;
    }
    for (i = 0; i < FILENAME_LEN && i < nbytes; i++) {		//put all the characters that we found in buffer
        buf[i] = dentry.filename[i]; 
    }
    //Increment the file position
    f->f_pos++;
    return i;
}

/* file_read
 * 
 * Reads the data in the specified file 
 * 
 * Inputs: file_t* file, int8_t* buf, int32_t nbytes
 * Outputs: 0 if read to end of the file, -1 if failed to read, amount of characters read elsewise
 * Side Effects: None
 */
int32_t file_read(file_t* file, int8_t* buf, int32_t nbytes){
    uint32_t n_inode = file->inode_num;
    uint32_t offset = file->f_pos;
    int32_t read_val;
    inode_t* cur_inode;                             //cur_inode holds the address of the starting inode block
    cur_inode = &file_sys.inode_blocks[n_inode];
    read_val = read_data(n_inode, offset, buf, nbytes);
    file->f_pos += read_val;
    return read_val;
}

/* file_sys_init(unsigned int addr)
 * 
 * Initializes the file system for illinix 391
 * Inputs: addr of the start of the file system.
 * Outputs: None
 * Side Effects: Initialize the file system
 */
void file_sys_init(unsigned int addr){
    file_sys.boot_block = (boot_block_t*) addr;                                                        //Start of the boot_block
    file_sys.inode_blocks = (inode_t*)(file_sys.boot_block + 1);                                       //Start of the inode block
    file_sys.data_blocks = (data_block_t*) (file_sys.inode_blocks + file_sys.boot_block->inode_count); //Start of the data block

}

/* read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
 * 
 * Reads Scans through the directory entries in the “boot block” to find the file name
 * Inputs: fname the file name, dummy dentry pointer
 * Outputs: return 0 if successful and -1 if failed
 * Side Effects: Set dentry pointer to the location of the specific file from fname
 */
int32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry) {
    int i, j, flag;                                                 //initialize variables

    for (i = 0 ; i < BOOT_DENTRIES_SIZE ; i++) {                    //for each dentry block
        flag = 1;                                                   //Set flag = 1
        read_dentry_by_index(i, dentry);                            //call read_dentry_by_index to set the dentry ptr for each file
        for (j = 0 ; j < FILENAME_LEN ; j++) {                      //for each character of file name
            if (fname[j] == NULL && dentry->filename[j] == NULL) {  //end condition that we found the correct dentry
                break;
            }
            if (dentry->filename[j] != fname[j]) {                  //end condition that this is the wrong dentry
                flag = 0;
                break;
            }
        }
        if (flag == 1) {                                            //If flag did not change, we found the correct file name
            return 0;                                               //Success
        }
    }
    return -1;                                                      //Else failed
}

/* read_dentry_by_index(uint32_t index, dentry_t* dentry)
 * 
 * Populates the dentry parameter -> file name, file type, inode number
 * Inputs: index - the dentry offset that we are checking, dentry - pointer that we want to change
 * Outputs: return 0 if successful and -1 if failed
 * Side Effects: Sets the dentry pointer to the corresponding dentry of the index
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    if (index >= BOOT_DENTRIES_SIZE) {                          //Parameter checking index >= 63
        return -1;
    }
    memcpy(dentry, &file_sys.boot_block->dentries[index], 64);  //memorycopy dentries[index] of the size 64 and put its address into dentry ptr.
    return 0;
}

/* read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
 * 
 * reads the values in the file of specified length
 * Inputs: inode - which file to read, offset - offset that chooses starting datablock in bytes, 
 *         buf - buffer to put in the values that we read, length - length of characters we want to read
 * Outputs: 0 if read to end of the file, -1 if failed to read, amount of characters read elsewise
 * Side Effects: It fills in the buf to the values that we read
 */
int32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length) {
    int32_t inode_num;                              //inode_num is total number of inodes in this file_system
    inode_num = file_sys.boot_block->inode_count;
    if ((int32_t)inode >= inode_num) {              //Parameter checking to see if inode that we are trying access is greater than the max inode_num
        return -1;                                  //Then return -1
    }
    inode_t* cur_inode;                             //cur_inode holds the address of the starting inode block
    cur_inode = &file_sys.inode_blocks[inode];
    if (offset >= (uint32_t)cur_inode->length) return 0;    //make sure within length of file
    uint32_t block_num;                                     //block_num holds the actual offset number for which 4kb datablock we are starting at.
    block_num = offset/BLOCK_SIZE;
    uint32_t data_block_offset = offset%BLOCK_SIZE;         //data_block_offset holds the offset for specific byte in the starting data_block.
    uint32_t i;
    for (i = 0 ; i < length ; i++, data_block_offset++) {   //copy the characters into the buffer of size "length"
        if (data_block_offset >= BLOCK_SIZE) {              //If we reach 1 + end of the block, then we should move to the next datablock
            data_block_offset = 0;                          //So set the data_block_offset to 0
            block_num++;                                    //Block_num should be incremented as 0
        }
        if (cur_inode->data_block_num[block_num] >= (uint32_t)file_sys.boot_block->data_count) {    //If we reach the end of the data block array, then return -1
            return -1;
        }
        // Memorycopy From (the file system starting datablocks, access the correct data block and take the data from correct data offset) to the buf[i] of size 1 byte
        memcpy(buf+i, &file_sys.data_blocks[cur_inode->data_block_num[block_num]].data_byte[data_block_offset], 1);
        if ((uint32_t)(offset+i) == (uint32_t)cur_inode->length) { // Check if this is after the end of the file, must return 0
            return i+1;
        }
    }
    return length;      //return the number of byte read
}

/* file_write
 * 
 * Dummy function to return 0
 * 
 * Inputs: file_t* file, int8_t* buf, int32_t nbytes
 * Outputs: 0
 * Side Effects: None
 */
int32_t file_write (file_t* file, int8_t* buf, int32_t nbytes){
    return 0;
}

/* file_open
 * 
 * Dummy function to return 0
 * 
 * Inputs: file_t* file
 * Outputs: 0
 * Side Effects: None
 */
int32_t file_open (file_t* file) {
    return 0;
}

/* file_close
 * 
 * Dummy function to return 0
 * 
 * Inputs: file_t* file
 * Outputs: 0
 * Side Effects: None
 */
int32_t file_close (file_t* file){
    return 0;
}

/* dir_write
 * 
 * Dummy function to return 0
 * 
 * Inputs: file_t* file, int8_t* buf, int32_t nbytes
 * Outputs: 0
 * Side Effects: None
 */
int32_t dir_write (file_t* file, int8_t* buf, int32_t nbytes){
    return 0;
}

/* dir_open
 * 
 * Dummy function to return 0
 * 
 * Inputs: file_t* file
 * Outputs: 0
 * Side Effects: None
 */
int32_t dir_open (file_t* file) {
    return 0;
}

/* dir_close
 * 
 * Dummy function to return 0
 * 
 * Inputs: file_t* file
 * Outputs: 0
 * Side Effects: None
 */
int32_t dir_close (file_t* file){
    return 0;
}

