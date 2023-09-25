#include "system_call.h"

/* sys_halt
 * 
 * Halts system by calling program halt
 * Inputs: int32_t status
 * Outputs: None
 * Side Effects: halts program
 */
void sys_halt(int32_t status){
    program_halt((int8_t)status);
}

/* sys_execute
 * 
 * Exectues system by calling program execute
 * Inputs: int8_t* cmmd
 * Outputs: None
 * Side Effects: executes program
 */
int32_t sys_execute(int8_t* cmmd){
    int32_t res = program_execute(cmmd);
    return res;
}

/* sys_read
 * 
 * Reads from correct file descriptor from correct globalPCB index
 * Inputs: uint32_t fd, int8_t* buf, uint32_t nbyte
 * Outputs: Read the correct file descriptor value.
 * Return -1 if failed.
 * Side Effects: none
 */
int32_t sys_read(uint32_t fd, int8_t* buf, uint32_t nbyte){
    if (fd >= 0 && fd < FILENUM){
        if (globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].flags){      // Check if flag is on
            if (globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl->read){        // Check if read is allowed
                return globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl->read(&globalPCB[terminal[s_t_idx].p_id]->file_desc[fd], buf, nbyte);
            }
        }
    }
    return -1;
}

/* sys_write
 * 
 * Writes to correct fd = file descriptor array. It will write nbytes from the incoming buf.
 * Inputs: uint32_t fd, a file desciptor index, int8_t* buf a buffer, 
 * uint32_t nbyte the length in terms of bytes.
 * Outputs: None
 * Side Effects: None
 */
int32_t sys_write(uint32_t fd, int8_t* buf, uint32_t nbyte){
    // Parameter checking of whether fd is within the range of the number of files.
    if (fd >= 0 && fd < FILENUM){
        // Checking whether the file descriptor is active for the specific fd.
        if (globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].flags){
            // If there is a write function
            if (globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl->write){
                // Then write it.
                return globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl->write(&globalPCB[terminal[s_t_idx].p_id]->file_desc[fd], buf, nbyte);
            }
        }
    }
    return -1;
}

/* sys_open
 * 
 * The function for opening the file.
 * Inputs: int8_t* filename
 * Outputs: return -1 if file, return fd if success
 * Side Effects: None
 */
int32_t sys_open(int8_t* filename){
    if (filename == 0) return -1;
    if (filename[0] == NULL) return -1;
    uint32_t fd;
    fd = 0;
    // Searching for free file descriptor
    int i;
    for(i = 2; i < FILENUM; i++){            // 0 and 1 are taken
        if (!globalPCB[terminal[s_t_idx].p_id]->file_desc[i].flags){
            fd = i;
            break;
        }
    }
    // Failed to find free file descriptor
    if (fd == 0) return -1;
    
    dentry_t dentry;
    // Compare the file name with each of these if statements.
    if (strncmp(filename, "rtc", LEN_RTC) == 0){
        globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl = &rtc_file;
    }
    else if (strncmp(filename, "stdin", LEN_STDIN) == 0){
        fd = 0;
        globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl = &stdin_file;
    }
    else if (strncmp(filename, "stdout", LEN_STDOUT) == 0) {
        fd = 1;
        globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl = &stdout_file;
    }
    else if (read_dentry_by_name(filename, &dentry) == 0){
        if (dentry.filetype == 1){ // Set the f_tbl to point to the dir_file table and its inode_num
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl = &dir_file;
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].inode_num = dentry.inode_num;           
        }
        else { // Set the f_tbl to point to the file system file type and its inode_num
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl = &fs_file;
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].inode_num = dentry.inode_num;
        }
    }
    else {  return -1;  }       // invalid open call

    globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].flags = 1;
    globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_pos = 0;
    // If there is a open function
    if (globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl->open)  
        globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl->open(globalPCB[terminal[s_t_idx].p_id]->file_desc+fd);
    else { return -1; }

    return fd;
}
/* sys_close
 * 
 * The function for closing the file.
 * Inputs: int32_t fd = file descriptor array index
 * Outputs: return -1 if fail, return 0 if success
 * Side Effects: None
 */
int32_t sys_close(uint32_t fd){
    // Check the boundary of fd array since we should not close stdin and stdout which is located in 0,1.
    if (fd > 1 && fd < FILENUM){
        // Set flags == 1 which means it is taken
        if (globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].flags == 1){
            // Break down the fd array and set flags, pos, tbl to 0,0,null respectively.
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl->close(globalPCB[terminal[s_t_idx].p_id]->file_desc+fd);
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_tbl = NULL;
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].flags = 0;
            globalPCB[terminal[s_t_idx].p_id]->file_desc[fd].f_pos = 0;
            return 0;
        }
    }
    return -1;
}

/* sys_vidmap
 * 
 * The function for mappig video memory.
 * Inputs: uint8_t** screen_start
 * Outputs: return -1 if fail, 0 if success
 * Side Effects: set screen start to 132MB
 */
 int32_t sys_vidmap(uint8_t** screen_start){

    // bound check
    if((uint32_t*)screen_start < (uint32_t*)USER_START || (uint32_t*)screen_start >= (uint32_t*)USER_END) {
        return -1;
    }

    // vid_page_table[t_idx].val = 0x0;
    // // Initialize page table entry
    // vid_page_table[t_idx].P = 1;
    // vid_page_table[t_idx].R_W = 1;        // Read/Write bit set to 1
    // vid_page_table[t_idx].U_S = 1;

    // // change t_idx in vidmap() to remane the same, just_update video_mem

    *screen_start = (uint8_t*)(S_START);        // 132 MB 
    // flush TLB
    flush_tlb();
    return 0;
 }

/* sys_getargs(uint8_t* buf, int32_t nbytes)
 * 
 * This function is for getting the arguments from the commandline and putting it into the buffer.
 * Inputs: uint8_t* buf, int32_t nbytes
 * Outputs: return -1 if fail, return 0 if success
 * Side Effects: None
 */
int32_t sys_getargs(uint8_t* buf, int32_t nbytes) {
    // Note that we parse the commandline and saves the argument in PCB during the execute function.
    // Take the argument saved in PCB and move it to the buf.
    if (!buf) return -1;        //If buffer is NULL return -1
    if (globalPCB[terminal[s_t_idx].p_id]->argument[0] == NULL || nbytes == 0) {  //If argument is NULL or nbytes are 0 return -1
        return -1;
    }
    int i;
    uint8_t c;
    for (i = 0 ; i < nbytes && (c = globalPCB[terminal[s_t_idx].p_id]->argument[i]) != NULL; i++) {
        buf[i] = globalPCB[terminal[s_t_idx].p_id]->argument[i];   //Put values into the buffer which are from the PCB.
    }
    if (c) {
        return -1;
    }
    buf[i] = c;
    return 0;
}

/* set_handler
 * 
 * The function for signal (Extra Credit). A dummy function.
 * Inputs: None
 * Outputs: return -1
 * Side Effects: None
 */
int32_t set_handler(int32_t signum, void* handler_address) {
    return -1;
}

/* sigreturn(void)
 * 
 * The function for signal (Extra Credit). A dummy function.
 * Inputs: None
 * Outputs: return -1
 * Side Effects: None
 */
int32_t sigreturn (void) {
    return -1;
}
