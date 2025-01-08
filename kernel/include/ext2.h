#ifndef EXT2_H

#define EXT2_H

#include <types.h>
#include <vfs.h>

#define EXT2_START_OFFSET 0x400

#define EXT2_ROOT_INODE 2

typedef struct ext2_super_block {
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
    uint32_t inodeper_group;     // Number of inodes per group
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

void ext2_init(filesystem_data* fs_data);
ext2_super_block ext2_read_super_block(filesystem_data* fs_data);

inline size_t s_block_get_block_size(ext2_super_block s_block){
    return 1024 << s_block.log_block_size;
}

#endif