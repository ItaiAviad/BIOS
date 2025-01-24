#ifndef EXT2_H

#define EXT2_H

#include <types.h>
#include <math.h>
#include <vfs.h>

#define EXT2_START_OFFSET (0x400)

#define EXT2_BLOCK_GROUP_TABLE_OFFSET (0x400) // Offset after the initial fs offset

#define EXT2_ROOT_INODE 2

#ifdef EXT2_DEBUG
    #define EXT2_DEBUG_PRINT(fmt, ...) printf("%s VFS: " fmt, LOG_SYM_DEBUG, ##__VA_ARGS__)
#else
    #define EXT2_DEBUG_PRINT(fmt, ...)
#endif

#ifdef EXT2_INFO
    #define EXT2_INFO_PRINT(fmt, ...) printf("%s VFS: " fmt, LOG_SYM_INF, ##__VA_ARGS__)
#else
    #define EXT2_INFO_PRINT(fmt, ...)
#endif

#ifdef EXT2_ERR
    #define EXT2_ERR_PRINT(fmt, ...) printf("%s VFS: " fmt, LOG_SYM_ERR, ##__VA_ARGS__)
#else
    #define EXT2_ERR_PRINT(fmt, ...)
#endif

typedef enum ext2_type_indicators_ext2{
    EXT2_FT_UNKNOWN = 0,
    EXT2_FT_REG_FILE = 1,
    EXT2_FT_DIR = 2,
    EXT2_FT_CHRDEV = 3,
    EXT2_FT_BLKDEV = 4,
    EXT2_FT_FIFO = 5,
    EXT2_FT_SOCK = 6,
    EXT2_FT_SYMLINK = 7

} ext2_type_indicator;

typedef struct  __attribute__((packed)) ext2_super_block {
    uint32_t inodecount;         // Total number of inodes
    uint32_t blockcount;         // Total number of blocks
    uint32_t r_blockcount;       // Number of reserved blocks
    uint32_t free_blockcount;    // Number of free blocks
    uint32_t free_inodecount;    // Number of free inodes
    uint32_t first_data_block;     // Number of the first block containing data
    uint32_t log_block_size;       // Block size = 1024 << log_block_size
    uint32_t log_frag_size;        // Fragment size
    uint32_t blockper_group;     // Number of blocks per group
    uint32_t fragper_group;      // Number of fragments per group
    uint32_t number_of_inodes_per_group;     // Number of inodes per group
    uint32_t mtime;                // Last mount time (UNIX timestamp)
    uint32_t wtime;                // Last write time (UNIX timestamp)
    uint16_t mnt_count;            // Number of times the filesystem has been mounted
    uint16_t max_mnt_count;        // Maximum number of mounts before a check is required
    uint16_t magic;                // Filesystem magic number (should be 0xEF53)
    uint16_t state;                // Filesystem state
                                     // (1: clean, 2: errors detected)
    uint16_t errors;               // What to do when an error is detected
                                     // (1: continue, 2: remount read-only, 3: panic)
    uint16_t minor_rev_level;      // Minor revision level
    uint32_t lastcheck;            // Time of last check (UNIX timestamp)
    uint32_t checkinterval;        // Maximum time between checks
    uint32_t creator_os;           // OS that created the filesystem
    uint32_t rev_level;            // Revision level
    uint16_t def_resuid;           // Default UID for reserved blocks
    uint16_t def_resgid;           // Default GID for reserved blocks

    // Fields for revision 1 (dynamic) superblock only
    uint32_t first_inode;            // First inode for standard files
    uint16_t inode_size;           // Size of inode structure
    uint16_t block_group_nr;       // Block group number of this superblock
    uint32_t feature_compat;       // Compatible feature set
    uint32_t feature_incompat;     // Incompatible feature set
    uint32_t feature_ro_compat;    // Read-only compatible feature set
    uint8_t  uuid[16];             // 128-bit UUID for volume
    char     volume_name[16];      // Volume name
    char     last_mounted[64];     // Directory where last mounted
    uint32_t algorithm_usage_bitmap; // For compression

    // Performance hints
    uint8_t  prealloc_blocks;      // Number of blocks to preallocate
    uint8_t  prealloc_dir_blocks;  // Number of blocks to preallocate for directories
    uint16_t padding1;

    // Journaling support (not used in ext2, relevant for ext3/ext4)
    uint8_t  journal_uuid[16];     // UUID of journal superblock
    uint32_t journal_inum;         // Inode number of journal file
    uint32_t journal_dev;          // Device number of journal file
    uint32_t last_orphan;          // Start of list of orphaned inodes

    // Additional fields (not fully used in ext2)
    uint32_t hash_seed[4];         // HTREE hash seed
    uint8_t  def_hash_version;     // Default hash version
    uint8_t  reserved_char_pad;
    uint16_t reserved_word_pad;
    uint32_t default_mount_opts;
    uint32_t first_meta_bg;        // First metablock block group
    uint32_t reserved[190];        // Padding to make superblock size 1024 bytes
} ext2_super_block;

typedef struct  __attribute__((packed)) ext2_block_group_descriptor{
    uint32_t addr_block_usage_bitmap;
    uint32_t addr_inode_usage_bitmap;
    uint32_t start_block_inode_table;
    uint16_t number_of_unallocated_blocks;
    uint16_t number_of_unallocated_inodes;
    uint16_t number_of_dirs_in_group;
} ext2_block_group_descriptor;


typedef struct  __attribute__((packed)) ext2_inode {
    uint16_t i_mode;        // File mode
    uint16_t i_uid;         // Low 16 bits of Owner UID
    uint32_t i_size;        // Size in bytes
    uint32_t i_atime;       // Access time
    uint32_t i_ctime;       // Creation time
    uint32_t i_mtime;       // Modification time
    uint32_t i_dtime;       // Deletion time
    uint16_t i_gid;         // Low 16 bits of Group ID
    uint16_t i_links_count; // Links count
    uint32_t i_blocks;      // Blocks count
    uint32_t i_flags;       // File flags
    uint32_t i_osd1;        // OS-dependent value
    uint32_t i_block[15];   // Pointers to blocks
    uint32_t i_generation;  // File version (used by NFS)
    uint32_t i_file_acl;    // File ACL
    uint32_t i_dir_acl;     // Directory ACL (only if i_size > 4GiB)
    uint32_t i_faddr;       // Fragment address
    uint8_t  i_osd2[12];    // OS-dependent value
} ext2_inode;


typedef struct  __attribute__((packed)) ext2_dir_entry {
    uint32_t inode;
    uint16_t size_of_entry;
    uint8_t name_length;
    uint8_t type_indicator;
    char name[];
} ext2_dir_entry;

static inline size_t s_block_get_block_size(ext2_super_block* s_block){
    return 1024 << s_block->log_block_size;
}

static inline size_t get_number_of_block_groups(ext2_super_block* superblock){
    return upper_divide(superblock->inodecount, superblock->number_of_inodes_per_group);
}

void ext2_init(filesystem_data* fs_data);
ext2_super_block ext2_read_super_block(filesystem_data* fs_data);
ext2_block_group_descriptor ext2_read_block_group_descriptor(filesystem_data* fs_data, ext2_super_block* s_block , uint64_t group_num);


ext2_inode* ext2_read_inode_metadata(filesystem_data* fs_data, ext2_super_block* super_block, uint64_t inode_num);

/**
 * @brief Reads the metadata of an inode
 * @param fs_data The data about the filesystem
 * @param super_block The superblock
 * @param 
 * @param path The path to find the inode in
 * @return 0 on fail, inode num at success 
 */
void* ext2_read_inode(filesystem_data* fs_data, ext2_super_block* super_block, uint64_t inode_num, size_t* size_read);

/**
 * @brief Finds the inode number at 
 * @param fs_data The data about the filesystem
 * @param path The path to find the inode in
 * @return 0 on fail, inode num at success 
 */
uint64_t ext2_get_inode_number_at_path(filesystem_data* fs_data, char* path);


ext2_dir_entry ext2_find_inode_in_dir_by_name(filesystem_data* fs_data, ext2_super_block* s_block, int dir_inode_num, char* name, size_t name_size); //Returns 0 on fail
#endif