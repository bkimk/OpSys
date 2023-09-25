#include "keyboard.h"

/* STDIN Function Pointers */
file_table_t stdin_file = {
    .read = terminal_read,
    .write = terminal_write,
    .open = terminal_open,
    .close = terminal_close
};

/* STDOUT Function Pointers */
file_table_t stdout_file = {
    .read = terminal_read,
    .write = terminal_write,
    .open = terminal_open,
    .close = terminal_close
};

/* map of the keyboard layout, based on scan code set 1 */
static const char keyboard_map[NUM_KEYS] = {
    0, 0,'1','2','3','4','5','6','7','8','9','0','-','=','\b', '\t','q','w',
    'e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/', 0,0,
    0,' ',0,17,18,19,0,0,0,0,0,0,0,0,0,0,0,0,'-',0, 0,0,'+',0,0,0,0,0,0,0,'<',0,0,0
};          //17,18,19 are for fn f keys

/* map of the keyboard layout while capitalization is enabled, based on scan code set 1 */
static const char keyboard_map_cap[NUM_KEYS] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '\?', 0, 0, 0, ' ', 0
};

/* keyboard_init
 * 
 * Initializes keyboard
 * Inputs: None
 * Outputs: None
 * Side Effects: enables the IRQ of keyboard interrupt
 */
void keyboard_init(){
    // keyboard automatically set to scan code set 1
    // initialize keyboard parameters
    keyboard.shift_pressed = 0;
    keyboard.ctrl_pressed = 0;
    keyboard.cl_pressed = 0;
    keyboard.cl_on = 0;
    // enable keyboard interrupts
    enable_irq(IRQ_KBD);
}

/* terminal_boot
 * 
 * Boots terminal 0
 * Inputs: None
 * Outputs: None
 * Side Effects: sets up paging for terminal video memory
 */
void terminal_boot(){
    // Boot terminals upon start up
    // Map tables for each terminal
    // Set up process id
    int i;
    for (i = 0; i < 3; i++) {
        map_table_entry(VIDEO+(i+1)*FOUR_KB, 0, VID_KB_OFF+(i+1), 1);
        terminal[i].video_mem = (char*)(VIDEO+(i+1)*FOUR_KB);
        terminal[i].p_id = -1;
        terminal[i].open = 0;
    }

    // Prepare to display terminal 0
    terminal[0].video_mem = (char*)(VIDEO);
    terminal[0].open = 1;
    map_table_entry((uint32_t)terminal[0].video_mem, VIDMAP_LOC, 0, 1);
    t_idx = 0;
}

/* terminal_buffer
 * 
 * Add character to the terminal buffer during a read request
 * Inputs: c - character to be added
 * Outputs: None
 * Side Effects: Edits the terminal reading buffer
 */
static void terminal_buffer(uint8_t c){
    if (keyboard.ctrl_pressed && c == 'l') {            // if screen is cleared, reset the idx to 0
        terminal[t_idx].line_buf_idx = 0;
    }
    else if (c == '\n'){                                // if enter is pressed, buffer an enter, end buffering
        terminal[t_idx].line_buf[terminal[t_idx].line_buf_idx] = c;
        terminal[t_idx].read_flag = 0;
    }   
    else if (c == '\b'){                                // if backspace pressed, remove last item buffered
        if (terminal[t_idx].line_buf_idx != 0)
            terminal[t_idx].line_buf_idx--;
        int i = 0;
        if (terminal[t_idx].tab_idx[terminal[t_idx].line_buf_idx] == 1){    // if tab is pressed, remove 4 times
            while(i < TERM_NUM){
                if (terminal[t_idx].line_buf_idx != 0)
                    terminal[t_idx].line_buf_idx--;
                i++;
            }
        }
    }
    else if (c == '\t'){                            // if tab is pressed, mark tab buffer as 1 to keep track
        int i = 0;
        while(terminal[t_idx].line_buf_idx != terminal[t_idx].read_length - 1 && i < TAB_LEN){
            terminal[t_idx].line_buf[terminal[t_idx].line_buf_idx] = ' ';
            terminal[t_idx].tab_idx[terminal[t_idx].line_buf_idx] = 1;
            i++;
            terminal[t_idx].line_buf_idx++;

        }
    }
    else{
        terminal[t_idx].line_buf[terminal[t_idx].line_buf_idx] = c;   // store regular key to buffer
        terminal[t_idx].tab_idx[terminal[t_idx].line_buf_idx] = 0;
        terminal[t_idx].line_buf_idx++;
    }

    // check whether run out of room
    if (terminal[t_idx].line_buf_idx == terminal[t_idx].read_length-1){          // last press is enter
        terminal[t_idx].line_buf[terminal[t_idx].read_length-1] = '\n';
        putc('\n');
        terminal[t_idx].read_flag = 0;
    }
}

/* keyboard_handler
 * 
 * Handles interrupts from keyboard presses, prints char to console
 * Inputs: None
 * Outputs: None
 * Side Effects: scans keyboard port and prints corresponding character
 */
void keyboard_handler(void){
    
    cli();  
    send_eoi(IRQ_KBD);                          // send EOI to PIC for keyboard interrupt
    
    uint8_t scan = inb(KBD_PORT);               // read in char from port 0x60

    // all releases are scan codes over 128
    if (scan < NUM_KEYS) {
        // deal with special cases
        if (scan == SCAN_R_SHIFT){
            keyboard.shift_pressed |= LOW_4BIT; // edit right byte
        }
        else if (scan == SCAN_L_SHIFT){
            keyboard.shift_pressed |= HIGH_4BIT; // edit left byte
        }
        else if (scan == SCAN_R_CTRL){
            keyboard.ctrl_pressed |= LOW_4BIT;  // edit right byte
        }
        else if (scan == SCAN_L_CTRL){
            keyboard.ctrl_pressed |= HIGH_4BIT;  // edit left byte
        }
        else if (scan == SCAN_L_ALT){
            keyboard.alt_pressed |= LOW_4BIT;
        }
        else if (scan == SCAN_CL && !keyboard.cl_pressed){
            keyboard.cl_pressed = ALL_8BIT;
            keyboard.cl_on = ~keyboard.cl_on;
        }
        else {
            char c;
            c = keyboard_map[scan];
            // deal with regular keyboard press
            // change c to be part of the capital keyboard if shift xor caps-lock are pressed
            if ((keyboard.shift_pressed > 0) != (keyboard.cl_on > 0)) c = keyboard_map_cap[scan];
            // deal with ctrl-l
            if ((keyboard.ctrl_pressed && c == 'l') || (keyboard.ctrl_pressed && c == 'L')) {
                clear();
            }
            else if (keyboard.alt_pressed){ //if alt is pressed
                if(c >= F1 && c <= F3){ //and f1,f2, of f3 is pressed
                    terminal_switch(c-F1);  //switch terminal accordingly
                }
            }
            else if (c != 0 && (c < F1 || c > F3)){ // skip special chars for now
                putc(c); // print char to screen
                if (terminal[t_idx].read_flag)     
                    terminal_buffer(c);     // add to terminal reading buffer
            }            
        }
    }
    // deal with special keyboard releases
    else if (scan == SCAN_R_SHIFT_R) {
        keyboard.shift_pressed &= HIGH_4BIT; // keep left byte
    }
    else if (scan == SCAN_L_SHIFT_R) {
        keyboard.shift_pressed &= LOW_4BIT; // keep right byte
    }
    else if (scan == SCAN_R_CTRL_R) {
        keyboard.ctrl_pressed &= HIGH_4BIT;  // keep left byte
    }
    else if (scan == SCAN_L_CTRL_R) {
        keyboard.ctrl_pressed &= LOW_4BIT;  // keep right byte
    }
    else if (scan == SCAN_CL_R){
        keyboard.cl_pressed = 0;
    }
    else if (scan == SCAN_L_ALT_R){
        keyboard.alt_pressed = 0;
    }

    enable_irq(IRQ_KBD);                        // enable IRQ again
    sti();
}

/* terminal_open
 * 
 * Reinitialize terminal variables, open terminal
 * Inputs: file - not used, for system call purposes
 * Outputs: 0 upon success
 * Side Effects: None
 */
int32_t terminal_open(file_t* file){
    // resetting terminal variables for reading
    terminal[t_idx].read_flag = 0;
    terminal[t_idx].line_buf_idx = 0;
    return 0;
}

/* terminal_close
 * 
 * Close terminal file
 * Inputs: file - not used, for system call purposes
 * Outputs: -1 upon closing
 * Side Effects: None
 */
int32_t terminal_close(file_t* file){
    return 0;
}

/* terminal_write
 * 
 * Write input buffer to terminal
 * Inputs: file - not used, for system call purposes
 *         buf - input buffer to write
 *         nbyte - number of bytes to write
 * Outputs: Number of bytes that were written
 * Side Effects: Prints buffer to terminal
 */
int32_t terminal_write(file_t* file, int8_t* buf, int32_t nbytes){
    file_t* f = file;
    if (f->f_tbl == &stdin_file) return -1;
    if (buf == NULL) return -1;             // Null check
    uint8_t string[nbytes];                 // copy from user to kernel memory
    memcpy(string, buf, nbytes);
    int i;
    for (i = 0; i < nbytes; i++){
        user_putc(string[i]);               // print bytes
    }
    return nbytes;                          // return number of bytes printed
}

/* terminal_read
 * 
 * Read from keyboard presses into the terminal
 * Inputs: file - not used, for system call purposes
 *         buf - input buffer to write
 *         nbyte - number of bytes to write
 * Outputs: Number of bytes that were read
 * Side Effects: Immedietely print everything that is typed
 *               Changes terminal read flags and variables
 */
int32_t terminal_read(file_t* file, int8_t* buf, int32_t nbytes){
    file_t* f = file;
    if (f->f_tbl == &stdout_file) return -1;
    if (buf == NULL) return -1;                                     // Null check
    terminal[s_t_idx].read_flag = 1;                                  // turn on read interrupts
    if (nbytes > LINE_SIZE) nbytes = LINE_SIZE;
    else if (nbytes < 0) return -1;
    
    terminal[s_t_idx].read_length = nbytes;
    while(terminal[s_t_idx].read_flag);                               // interrupt handler will read to buffer
    uint32_t bytes = terminal[s_t_idx].line_buf_idx+1;                // store number of bytes read
    terminal[s_t_idx].line_buf_idx = 0;                               // reset the buffer index
    memcpy(buf, &terminal[s_t_idx].line_buf, bytes);                  // copy to user memory
    return bytes;
}

/* terminal_switch
 * 
 * Switch terminal
 * Inputs: uint32_t new_term
 * Outputs: None
 * Side Effects: switch terminals and update terminal index
 */

void terminal_switch(uint32_t new_term){
    cli();
    if (!terminal[new_term].open) {                          // if terminal 2 is not boot, boot it
        if (get_free_pid() == -1) {sti(); return;}
        terminal[new_term].open = 1;
    }

    terminal[t_idx].video_mem = (char*)(VIDEO+(t_idx+1)*FOUR_KB);        // set pointer to corresponding video page in physical memory

    memcpy(terminal[t_idx].video_mem, (void*)VIDEO, FOUR_KB);                   // copy and save video memory to corresponding video page
    memcpy((void*)VIDEO, terminal[new_term].video_mem, FOUR_KB);                // restore corresponding terminal to video memory
    
    terminal[new_term].video_mem = (char*)(VIDEO);

    t_idx = new_term;

    sync_cursor();                                                      // update cursor
    sti();
}
