#include "paging.h"


/* page_init()
 * 
 * Page Initialization Function
 * Inputs: None
 * Outputs: None
 * Side Effects: Sets various bits of Page Directory and Page Table with corresponding bits
 * Coverage: None
 * Files: paging.h, x86_desc.h
 */
void page_init() {

    //set each entry to not present
    int i;

    // Iterating through all elements of page directory
    for(i = 0; i < KIB; i++)
    {
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        page_directory[i].val = 0x00000000;             // Setting all values of directory to zero            
    }

    //directory kb init
    page_directory[0].kb.P = 1;         //Present bit set to 1
    page_directory[0].kb.R_W = 1;       //Read/Write bit set to 1
    page_directory[0].kb.offset = ((unsigned long)first_page_table >> OFFSET);      //Base address set to table address

    //directory mb init
    page_directory[1].mb.P = 1;         //Present bit set to 1
    page_directory[1].mb.R_W = 1;       //Read/Write bit set to 1
    page_directory[1].mb.PS = 1;        //Page size bit set to 1
    page_directory[1].mb.offset = 1;        //kernel page is 4MB to 8MB

    page_directory[VIDMAP_LOC].val = 0x0;
    // Initialize directory entry for vidmap
    page_directory[VIDMAP_LOC].kb.P = 1;         // Present bit set to 1
    page_directory[VIDMAP_LOC].kb.R_W = 1;       //Read/Write bit set to 1
    page_directory[VIDMAP_LOC].kb.U_S = 1;
    page_directory[VIDMAP_LOC].kb.offset = ((unsigned long)vid_page_table >> OFFSET);      //Base address set to table address


    // holds the physical address where we want to start mapping these pages to.
    // in this case, we want to map these pages to the very beginning of memory.
    // we will fill all 1024 entries in the table, mapping 4 megabytes
    for(i = 0; i < KIB; i++)
    {
        first_page_table[i].val = 0x00000000;           // Setting all values of table to zero
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes ;)
        if (i == VIDEOMEM) {
            first_page_table[i].P = 1;          //Present bit set to 1
            first_page_table[i].R_W = 1;        //Read/Write bit set to 1
            first_page_table[i].offset = i;     //Base address given address
        }
    }

    

    // holds the physical address where we want to start mapping these pages to.
    // in this case, we want to map these pages to the very beginning of memory.
    //we will fill all 1024 entries in the table, mapping 4 megabytes
    for(i = 0; i < KIB; i++)
    {
        vid_page_table[i].val = 0x00000000;           // Setting all values of table to zero
    }

    /* Loading + Enabling Paging with CR0, CR3, CR4 */
    loadPageDirectory((unsigned int *) page_directory);
    enablePaging();
}

/* flush_tlb()
 * 
 * Function for flushing tlb
 * Inputs: None
 * Outputs: None
 * Side Effects: flushes tlb
 * Coverage: None
 * Files: 
 */
void flush_tlb(){
    asm volatile ("                 \n\
            movl    %%cr3, %%eax      \n\
            movl    %%eax, %%cr3      \n\
            "
            :
            :
            : "eax", "cc"           
    );

}

/* map_mb_page
 * 
 * Map MB page
 * Inputs: int32_t phys_addr, int32_t virt_page, int32_t user_access
 * Outputs: 0
 * Side Effects: Connects correct virtual address to physical address
 */
int32_t map_mb_page(int32_t phys_MB_offset, int32_t virt_MB_offset, int32_t user_access) {
    page_directory[virt_MB_offset].val = 0;

    page_directory[virt_MB_offset].mb.P = 1;         //Present bit set to 1
    page_directory[virt_MB_offset].mb.R_W = 1;       //Read/Write bit set to 1
    page_directory[virt_MB_offset].mb.PS = 1;        //Page size bit set to 1
    page_directory[virt_MB_offset].mb.U_S = user_access;
    page_directory[virt_MB_offset].mb.offset = phys_MB_offset;

    flush_tlb();
    return 0;

}

/* map_table_entry
 * 
 * Maps the table entry.
 * Inputs: int32_t phys_addr, int32_t page_dir_offset, int32_t virt_KB_offset, int32_t user_access
 * Outputs: 0
 * Side Effects: Connects correct virtual address to physical address
 */
int32_t map_table_entry(int32_t phys_addr, int32_t page_dir_offset, int32_t virt_KB_offset, int32_t user_access){

    page_table_entry_t* table = (page_table_entry_t*)(page_directory[page_dir_offset].kb.offset << OFFSET);
    table[virt_KB_offset].val = 0x0;
    // Initialize page table entry
    table[virt_KB_offset].P = 1;
    table[virt_KB_offset].R_W = 1;        //Read/Write bit set to 1
    table[virt_KB_offset].U_S = user_access;
    table[virt_KB_offset].offset = phys_addr >> OFFSET;

    flush_tlb();
    return 0;
}

/* remap_vidmap
 * 
 * remaps vidmap
 * Inputs: uint8_t terminal_idx
 * Outputs: 0
 * Side Effects: Used in process_execute when we set up program paging/memory & in scheduler
 */
void remap_vidmap(uint8_t terminal_idx){
    vid_page_table[0].offset = (uint32_t)terminal[terminal_idx].video_mem >> OFFSET;
    flush_tlb();
}
