
#include "scheduler.h"

uint32_t s_t_idx = 0;


/* pit_init
 * 
 * Initializes pit
 * Inputs: None
 * Outputs: None
 * Side Effects: Initializes pit by sending reload value to low, high byte
 */
void pit_init(){
    // change to use flags
    cli();
    // init pit interrrupts, assign frequency with upper/lower bytes
    outb(PIT_INIT_CODE, PIT_CMD_REG);       
    // lower byte                
    outb(PIT_FREQ_LOW, PIT_DATA_PORT_0);
    // upper byte
    outb(PIT_FREQ_HIGH, PIT_DATA_PORT_0);
    sti();
    enable_irq(0);
 
}

/* pit_handler
 * 
 * Handler for pit
 * Inputs: None
 * Outputs: None
 * Side Effects: Sends eoi, executes the scheduler function and enables irq 0.
 */
void pit_handler(){
    // mask and acknowledge interrupt
    cli();
    send_eoi(0);
    // reenable interrupt
    enable_irq(0);

    scheduler();
    sti();
}

/* start_terminal_process
 * 
 * starts the terminal process
 * Inputs: None
 * Outputs: None
 * Side Effects: executes shell when starting the terminal
 */
static void start_terminal_process() {
    sti();
    sys_execute("shell");
}


/* schedule_process
 * 
 * Finds next scheduled terminal
 * Inputs: None
 * Outputs: None
 * Side Effects: Changes s_t_idx
 */
static void schedule_process(){
    // find next terminal 
    int i;
    for (i = 0; i < TERM_NUM; i++){
        // if next terminal is on switch to it
        s_t_idx = (s_t_idx+1)%TERM_NUM;
        if (terminal[s_t_idx].open){
            break;
        }
    }
}


/* scheduler
 * 
 * Runs scheduler algorithm
 * Inputs: None
 * Outputs: None
 * Side Effects: switches context and applies scheduling algorithm
 */
void scheduler(){
    cli();
    // Check for program halt
    // if (terminal[s_t_idx].halt){ sys_halt(HALT_CODE); terminal[s_t_idx].halt = 0; }

    // Context Switch
    uint32_t old_terminal = s_t_idx;

    // Update s_t_idx
    schedule_process();

    // if scheduled process is same as current process, nothing to do
    if (old_terminal == s_t_idx) { sti(); return; }

    uint32_t new_ebp, new_esp;

    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    globalPCB[terminal[old_terminal].p_id]->saved_ebp = saved_ebp;
    globalPCB[terminal[old_terminal].p_id]->saved_esp = saved_esp;

    if(terminal[s_t_idx].p_id == -1){
        start_terminal_process();
    }
    else{
        // Map page
        map_mb_page(PHYS_8MB_OFF + terminal[s_t_idx].p_id, MB128, 1);
        // Page correct user process's vidmap to video memory
        remap_vidmap(s_t_idx);
        // Change value for ESP0 in TSS
        tss.esp0 = EIGHTMB-(terminal[s_t_idx].p_id*EIGHTKB)-FOURBYTE;

        new_ebp = globalPCB[terminal[s_t_idx].p_id]->saved_ebp;
        new_esp = globalPCB[terminal[s_t_idx].p_id]->saved_esp;

        asm volatile (
            "                       \n\
            movl %%edx, %%esp          \n\
            movl %%ecx, %%ebp          \n\
            "
            :
            : "d" (new_esp), "c" (new_ebp)
        );
    }
    sti();
}

