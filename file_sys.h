#ifndef _FILE_SYS_H
#define _FILE_SYS_H
#ifndef ASM

#include "types.h"
#include "lib.h"


#define FILENAME_LEN 32
#define BOOT_RSV 52
#define BOOT_DENTRIES_SIZE 63
#define DENTRY_RSV 24
#define INODE_DATA_BLOCK_SIZE 1023
#define BLOCK_SIZE 4096
#define FD_MAX 10

typedef struct file_t file_t;

/* File table structure */
typedef struct file_table_t {
    int32_t (*read) (file_t* file, int8_t* buf, int32_t nbytes);
    int32_t (*write) (file_t* file, int8_t* buf, int32_t nbytes);
    int32_t (*open) (file_t* file);
    int32_t (*close) (file_t* file);
} file_table_t;

/* File structure */
struct file_t {
    file_table_t *f_tbl;
    uint32_t inode_num;             
    uint32_t f_pos;
    uint32_t flags;
    uint32_t priv_data;
};

void fd_open();

/* This is a dentry */
typedef struct dentry_t {
    int8_t filename[FILENAME_LEN];  // file name
    int32_t filetype;               // file type
    int32_t inode_num;              // inode #
    int8_t reserved[DENTRY_RSV];
} dentry_t;

/* This is a boot block. */
typedef struct boot_block_t {
    int32_t dir_count;      // # of dir. entries
    int32_t inode_count;    // # of inodes
    int32_t data_count;     // data blocks
    int8_t reserved[BOOT_RSV];
    dentry_t dentries[BOOT_DENTRIES_SIZE];
} boot_block_t;

/* This is a inode */
typedef struct inode_t {
    int32_t length;                                 // length in B
    int32_t data_block_num[INODE_DATA_BLOCK_SIZE];  // Innode data block array
} inode_t;

/* This is a data block*/
typedef struct data_block_t {
    uint8_t data_byte[BLOCK_SIZE];  // data block array
} data_block_t;

/* This is the file system*/
typedef struct file_sys_t {
    boot_block_t* boot_block;
    inode_t* inode_blocks;
    data_block_t* data_blocks;
} file_sys_t;

/* Declare file system */
file_sys_t file_sys;

/*Initialize file system*/
void file_sys_init(unsigned int addr);

/*These are for reading file data*/
int32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length);
int32_t file_read(file_t* file, int8_t* buf, int32_t nbytes);
int32_t dir_read(file_t* file, int8_t* buf, int32_t nbytes);

/*Dummy functions*/
int32_t file_write (file_t* file, int8_t* buf, int32_t nbytes);
int32_t file_open (file_t* file);
int32_t file_close (file_t* file);
int32_t dir_write (file_t* file, int8_t* buf, int32_t nbytes);
int32_t dir_open (file_t* file);
int32_t dir_close (file_t* file);


file_table_t fs_file;
file_table_t dir_file;

#endif /* ASM */
#endif /* _FILE_SYS_H */
