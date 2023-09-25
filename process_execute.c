#include "process_execute.h"

/* program_halt
 * 
 * halts the program
 * Inputs: int8_t status
 * Outputs: None
 * Side Effects: current process is being terminated and
 * parent process is new current process. Unmap current page, map parent page.
 * Change esp0 to parent process. Decrement process id. Flus TLB.
 */
void program_halt(int8_t status){
    cli();
    /** @TODO: account for case where other terminal has parent */
    // free file memory
    int i;
    for (i = 0; i < FILENUM; i++) {  
        sys_close(i); 
    }
    // set halting process to inactive
    globalPCB[terminal[s_t_idx].p_id]->active = 0;

    // clean argument
    i = 0;
    while(globalPCB[terminal[s_t_idx].p_id]->argument[i] != NULL){
        globalPCB[terminal[s_t_idx].p_id]->argument[i] = NULL;
        i++;
    }
    
    if (globalPCB[terminal[s_t_idx].p_id]->parent_id == -1) { 
        globalPCB[terminal[s_t_idx].p_id] = NULL; 
        terminal[s_t_idx].p_id = -1; sys_execute("shell"); 
    }

    uint32_t old_p_id = terminal[s_t_idx].p_id;
    terminal[s_t_idx].p_id = globalPCB[terminal[s_t_idx].p_id]->parent_id;

    globalPCB[old_p_id] = NULL;

    // tss for parent
    tss.esp0 = EIGHTMB-(terminal[s_t_idx].p_id)*EIGHTKB-FOURBYTE; // check

    // map parents paging in
    // phys MB offset, virt MB offset, priviledge level
    map_mb_page(PHYS_8MB_OFF + terminal[s_t_idx].p_id, MB128, 1);

    // read saved esp and ebp of parent process
    uint32_t orig_ebp = globalPCB[terminal[s_t_idx].p_id]->saved_ebp;
    uint32_t orig_esp = globalPCB[terminal[s_t_idx].p_id]->saved_esp;
    sti();

    // restore esp and ebp into registers
    asm volatile (
        "                       \n\
        movl %%edx, %%esp          \n\
        movl %%ecx, %%ebp          \n\
        movl %%ebx, %%eax          \n\
        jmp execute_return      \n\
        "
        :
        : "d" (orig_esp), "c" (orig_ebp), "b" ((uint32_t)status)
    );
}

/* program_execute
 * 
 * executes the program
 * Inputs: const int8_t* command
 * Outputs: 0 or -1 if fails
 * Side Effects: child process is created and process id number
 * is incremented. New page is mapped. PCB is initialized.
 * esp0 value is set to new kernel stack. TLB is flushed.
 */
int32_t program_execute(const int8_t* command){   

    /* -------- Check for Free P-ID -------- */
    cli();
    uint32_t new_pid;
    if((new_pid = get_free_pid()) == -1){ sti(); return -1; }

    // Read command
    if (command == NULL) { sti(); return -1; }

    int32_t i = 0, j = 0, file_size;
    int32_t inst_ptr;
    int8_t first_word[MAXWORD];
    int8_t args[MAXWORD];
    while (command[i] != ' ' && command[i] != NULL) {
        first_word[i] = command[i];
        i++;
    }
    first_word[i] = NULL;

    // Read argument and save for later
    if (command[i] == ' '){
        i++;
    }
    while (command[i] != NULL) {
        args[j] = command[i];
        j++, i++;
    }
    args[j] = NULL;

    /* -------- Read Executable File -------- */

    // Open file
    int8_t buf[FOURBYTE], eip_buf[FOURBYTE];
    dentry_t d;
    if (read_dentry_by_name(first_word, &d) != 0) { sti(); return -1;}
    if (read_data(d.inode_num, 0, buf, FOURBYTE) != FOURBYTE) { sti(); return -1;}

    // Check whether file is executable
    int32_t execute_check = (int32_t)(*buf);
    if (execute_check != EXE_CHECK_1 && execute_check != EXE_CHECK_2 && execute_check != EXE_CHECK_3 && execute_check != EXE_CHECK_4) {
        sti(); return -1;
    }

    // Calculate instruction pointer location
    if (read_data(d.inode_num, INSTR_RD_OFF, eip_buf, FOURBYTE) != FOURBYTE) { sti(); return -1;}
    inst_ptr = *((int32_t*)eip_buf);

    // Find file length
    file_size = file_sys.inode_blocks[d.inode_num].length;

    /* -------- Create New PCB and Fill Process Memory  -------- */

    int32_t prev_p_id = terminal[s_t_idx].p_id;
    terminal[s_t_idx].p_id = new_pid;
    
    // Set up program paging/memory
    map_mb_page(PHYS_8MB_OFF + terminal[s_t_idx].p_id, MB128, 1);
    remap_vidmap(s_t_idx);

    // Copy executable file
    if(read_data(d.inode_num, 0, (int8_t*)USER_FILE_VIRT_LOC, file_size)!=file_size) { sti(); return -1;}

    // Assign PCB fields
    globalPCB[terminal[s_t_idx].p_id] = (pcb_t*)(EIGHTMB - (EIGHTKB * (terminal[s_t_idx].p_id+1)));
    globalPCB[terminal[s_t_idx].p_id]->process_id = terminal[s_t_idx].p_id;
    globalPCB[terminal[s_t_idx].p_id]->parent_id = prev_p_id;

    // Store arguments
    i = 0;
    globalPCB[terminal[s_t_idx].p_id]->argument[i] = NULL;
    while(args[i] != NULL) {
        globalPCB[terminal[s_t_idx].p_id]->argument[i] = args[i];
        i++;
    }
    globalPCB[terminal[s_t_idx].p_id]->argument[i] = NULL;

    // Prepare file descriptor array
    for (i = 0; i < FILENUM; i++) {  
        sys_close(i);
    }
    sys_open("stdin");
    sys_open("stdout");

    // Set PCB as active
    globalPCB[terminal[s_t_idx].p_id]->active = 1;

    /* -------- Context Switch -------- */

    // Change value for ESP0 and SS in TSS
    tss.esp0 = EIGHTMB-(terminal[s_t_idx].p_id*EIGHTKB)-FOURBYTE;

    // Save esp/ebp to parent process
    if (globalPCB[terminal[s_t_idx].p_id]->parent_id != -1){
        register uint32_t saved_ebp asm("ebp");
        register uint32_t saved_esp asm("esp");
        globalPCB[prev_p_id]->saved_esp = saved_esp;
        globalPCB[prev_p_id]->saved_ebp = saved_ebp;
    }
    sti();

    // Push iret context and iret to new process
    asm volatile (
        "                               \n\
        movw $" STR(DS_)  ", %%ax       \n\
        movw  %%ax, %%ds                \n\
        pushl $" STR(USER_DS) "         \n\
        pushl $" STR(USER_ESP) "        \n\
        pushfl                          \n\
        pushl $" STR(USER_CS) "         \n\
        pushl %0                        \n\
        iret                            \n\
        execute_return:                 \n\
        leave                           \n\
        ret                             \n\
        "
        : 
        : "b" (inst_ptr)
        : "memory"
    );

    return 0;
}

/* get_free_pid
 * 
 * Find and return smallest inactive process ID
 * Inputs: None
 * Outputs: Available process id and -1 if none exists
 * Side Effects: None
 */
int32_t get_free_pid(){
    // Find and return smallest inactive process ID
    int i;
    for (i = 0; i < PROCESSNUM; i++){
        if (!globalPCB[i]){
            return i;
        }
    }
    return -1;
}
