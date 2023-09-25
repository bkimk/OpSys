#ifndef _PAGING_H
#define _PAGING_H
#ifndef ASM

#include "x86_desc.h"
#include "lib.h"

#define KIB 1024
#define FOURKIB 4096
#define OFFSET 12
#define VIDEOMEM 184
#define VIDMAP_LOC 33

/* Page Directory (4KByte) Elements */
typedef struct page_directory_entry_kb_t {
        uint32_t P         : 1;     // Present
        uint32_t R_W       : 1;     // Read/Write
        uint32_t U_S       : 1;     // User/Supervisor
        uint32_t PWT       : 1;     // Write-Through
        uint32_t PCD       : 1;     // Cache Disable
        uint32_t A         : 1;     // Accessed
        uint32_t RSV       : 1;     // Reserved
        uint32_t PS        : 1;     // Page Size
        uint32_t G         : 1;     // Global Page
        uint32_t AVL       : 3;     // Available
        uint32_t offset    : 20;    // Page Table Base Address
}__attribute__ ((packed)) page_directory_entry_kb_t;

/* Page Directory (4MByte) Elements */
typedef struct page_directory_entry_mb_t {
        uint32_t P         : 1;     // Present
        uint32_t R_W       : 1;     // Read/Write
        uint32_t U_S       : 1;     // User/Supervisor
        uint32_t PWT       : 1;     // Write-Through
        uint32_t PCD       : 1;     // Cache Disabled
        uint32_t A         : 1;     // Accessed
        uint32_t D         : 1;     // Dirty
        uint32_t PS        : 1;     // Page Size (1 indicates 4 MBytes)
        uint32_t G         : 1;     // Global Page
        uint32_t AVL       : 3;     // Available
        uint32_t PAT       : 1;     // Page Table Attribute Index
        uint32_t RSV       : 9;     // Reserved
        uint32_t offset    : 10;    // Page Base Address
}__attribute__ ((packed)) page_directory_entry_mb_t;

/* Page Table Elements */
typedef union page_table_entry_t {
    uint32_t val;
    struct {
        uint32_t P         : 1;     // Present
        uint32_t R_W       : 1;     // Read/Write
        uint32_t U_S       : 1;     // User/Supervisor
        uint32_t PWT       : 1;     // Write-Through
        uint32_t PCD       : 1;     // Cache Disable
        uint32_t A         : 1;     // Accessed
        uint32_t D         : 1;     // Dirty
        uint32_t PAT       : 1;     // Page Attribute Table
        uint32_t G         : 1;     // Global
        uint32_t AVL       : 3;     // Available
        uint32_t offset    : 20;    // Page Base Address
    } __attribute__ ((packed));
} page_table_entry_t;


/* Page Directory Holding Both Types Of Directories (KB and MB) */
typedef union wombo_combo_t {
    uint32_t val;
    page_directory_entry_kb_t kb;
    page_directory_entry_mb_t mb;
} wombo_combo_t;

/* Initializing Directory and Table */
wombo_combo_t page_directory[KIB] __attribute__((aligned(FOURKIB)));
page_table_entry_t first_page_table[KIB] __attribute__((aligned(FOURKIB)));
page_table_entry_t vid_page_table[KIB] __attribute__((aligned(FOURKIB)));

/* Declaring Paging Initialization Function */
void page_init();

/* Mapping Function */
int32_t map_mb_page(int32_t phys_MB_offset, int32_t virt_MB_offset, int32_t user_access);
int32_t map_table_entry(int32_t phys_addr, int32_t page_dir_offset, int32_t virt_KB_offset, int32_t user_access);
void remap_vidmap(uint8_t terminal_idx);
void flush_tlb();
#endif /* ASM */
#endif /* _PAGING_H */
