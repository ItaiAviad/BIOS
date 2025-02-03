#include <ext2.h>
#include <disk.h>
#include <math.h>
 

ext2_super_block ext2_read_super_block(filesystem_data* fs_data){
    ext2_super_block super_block;
    read(fs_data->disk_number, fs_data->start_offset, sizeof(ext2_super_block), &super_block);
    if (super_block.magic != 0xEF53) {
        printf("Error: Invalid superblock magic number (0x%x)\n", super_block.magic);
        memset(&super_block, 0, sizeof(ext2_super_block)); // Return a zeroed-out superblock
    }
    return super_block;
}

ext2_block_group_descriptor ext2_read_block_group_descriptor(filesystem_data* fs_data, ext2_super_block* s_block , uint64_t group_num){
    ext2_block_group_descriptor block_group_descriptor;
    uint64_t block_size = s_block_get_block_size(s_block);
    uint64_t bgdt_offset = fs_data->start_offset + (block_size > 1024 ? block_size : 1024);
    read(fs_data->disk_number, bgdt_offset + sizeof(ext2_block_group_descriptor) * group_num, sizeof(ext2_block_group_descriptor), &block_group_descriptor);
    return block_group_descriptor;
}

ext2_inode* ext2_read_inode_metadata(filesystem_data* fs_data, ext2_super_block* super_block, uint64_t inode_num){
    uint64_t num_block_groups = get_number_of_block_groups(super_block);
    uint64_t block_group_num = (inode_num - 1) / super_block->number_of_inodes_per_group;
    ext2_block_group_descriptor block_group_descriptor = ext2_read_block_group_descriptor(fs_data, super_block, block_group_num);
    uint64_t inode_table_index = (inode_num - 1) % super_block->number_of_inodes_per_group;
    uint64_t addr = ((block_group_descriptor.start_block_inode_table-1) * s_block_get_block_size(super_block)) + (inode_table_index * super_block->inode_size);
    ext2_inode* ret = malloc(super_block->inode_size);
    read(fs_data->disk_number, fs_data->start_offset + addr, super_block->inode_size, ret);
    return ret;
}

ext2_dir_entry ext2_find_inode_in_dir_by_name(filesystem_data* fs_data, ext2_super_block* s_block, int dir_inode_num, char* inode_name, size_t name_size){
    uint64_t size_read = 0;
    void* data = ext2_read_inode(fs_data, s_block, dir_inode_num, &size_read);
    ext2_dir_entry* curr_entry = data;



    while((((void*)curr_entry) - data + 1) < size_read){

        if(strncmp(inode_name, curr_entry->name, min(curr_entry->name_length, name_size)) == 0){
            return *(curr_entry);
        } 

        curr_entry = ((uint64_t) curr_entry) + curr_entry->size_of_entry;
    }

    free(data);
    ext2_dir_entry ret = {0};
    return ret;

}

linkedListNode* ext2_list_dir(filesystem_data* fs_data, char* path){
    linkedListNode* ret = NULL;
    ext2_super_block s_block = ext2_read_super_block(fs_data);
    uint64_t dir_inode_num = ext2_get_inode_number_at_path(fs_data, &s_block ,path);

    if(!dir_inode_num){
        EXT2_INFO_PRINT("Couldn't find inode!");
        return;
    }

    uint64_t size_read = 0;
    void* data = ext2_read_inode(fs_data, &s_block, dir_inode_num, &size_read);
    ext2_dir_entry* curr_entry = data;

    while((((void*)curr_entry) - data + 1) < size_read){
        char* str = malloc(curr_entry->name_length+1);
        memcpy(str, curr_entry->name, curr_entry->name_length);
        str[curr_entry->name_length] = 0;
        append_node(&ret, str);
        curr_entry = ((uint64_t) curr_entry) + curr_entry->size_of_entry;
    }
    free(data);
    return ret;
}

uint64_t ext2_get_inode_number_at_path(filesystem_data* fs_data, ext2_super_block* s_block, char* path){
    char* path_preproccesed = preprocess_path(path);
    uint64_t ret = 0;
    if (!path_preproccesed) {
        EXT2_ERR_PRINT("Memory allocation failed!");
        return ret;
    }

    if (*path_preproccesed == '\0') {
        EXT2_ERR_PRINT("Invalid Path!\n");
        free(path_preproccesed);
        return ret;
    }


    EXT2_DEBUG_PRINT("Searching for node with path: %s\n", path_preproccesed);

    if (strcmp(path_preproccesed, "/") == 0) {
        free(path_preproccesed);
        return EXT2_ROOT_INODE;
    }

    uint64_t current_inode = EXT2_ROOT_INODE;

    char *current_path_position = path_preproccesed;
    char *current_seperator_position = path_preproccesed;

    // Skip the root `/` explicitly
    if (*current_path_position == PATH_SEPERATOR) {
        current_path_position++;
    }

    while (*current_path_position) {

        char *next_separator_position = strchr(current_path_position, PATH_SEPERATOR);
        if (!next_separator_position) {
            next_separator_position = strchr(current_path_position, '\0');
        }

        ext2_dir_entry found_inode = ext2_find_inode_in_dir_by_name(fs_data, s_block, current_inode, current_path_position, next_separator_position-current_path_position);
        current_inode = found_inode.inode;
        if(!next_separator_position || *(next_separator_position) == '\0'){
            free(path_preproccesed);
            return current_inode;
        }
        if(((ext2_type_indicator)found_inode.type_indicator) != EXT2_FT_DIR){
            // The path doesn't exist, return a zeroed object
            free(path_preproccesed);
            return ret;
        }
        current_path_position = next_separator_position + 1; // Skip separator

    }



    free(path_preproccesed);
    return ret;
}

// ------------------------------------------------------------------------
// Helper to read from an array of block pointers (single-indirect).
// We do NOT assume any special error handling in read(...).
// ------------------------------------------------------------------------
static void read_single_indirect_blocks(
    filesystem_data* fs_data,
    void*            data,
    uint32_t         block_num,
    uint64_t         size_to_read,
    uint64_t         block_size,
    uint64_t*        blocks_read
) {
    if (block_num == 0) {
        return;
    }

    // This block contains an array of 32-bit block numbers
    uint32_t* block_addrs = malloc(block_size);
    if (!block_addrs) {
        printf("EXT2: Could not allocate memory for single-indirect block array.\n");
        return;
    }

    // Convert block_num to a byte offset on disk using fs_data->start_offset
    uint64_t offset_on_disk = ((uint64_t)block_num - 1) * block_size + fs_data->start_offset;
    read(fs_data->disk_number, offset_on_disk, block_size, block_addrs);

    size_t entries = block_size / sizeof(uint32_t);
    for (size_t i = 0; i < entries; i++) {
        if ((*blocks_read) * block_size >= size_to_read) {
            break;
        }
        if (block_addrs[i] == 0) {
            break;  // no more blocks
        }

        // Figure out how many bytes remain
        uint64_t left    = size_to_read - (*blocks_read * block_size);
        size_t   to_read = (left < block_size) ? (size_t)left : (size_t)block_size;
        size_t   dst     = (size_t)(*blocks_read * block_size);

        // Convert the data block number into a byte offset
        uint64_t data_block_offset = (uint64_t)(block_addrs[i] - 1) * block_size + fs_data->start_offset;
        read(fs_data->disk_number, data_block_offset, to_read, (uint8_t*)data + dst);

        (*blocks_read)++;
    }

    free(block_addrs);
}

// ------------------------------------------------------------------------
// Helper to read from a double-indirect block pointer (points to multiple
// single-indirect arrays).
// ------------------------------------------------------------------------
static void read_double_indirect_blocks(
    filesystem_data* fs_data,
    void*            data,
    uint32_t         block_num,
    uint64_t         size_to_read,
    uint64_t         block_size,
    uint64_t*        blocks_read
) {
    if (block_num == 0) {
        return;
    }

    // The double-indirect block itself is an array of block pointers
    uint32_t* outer_block = malloc(block_size);
    if (!outer_block) {
        printf("EXT2: Could not allocate memory for double-indirect block.\n");
        return;
    }

    // Convert block_num to a byte offset
    uint64_t offset_on_disk = (uint64_t)(block_num - 1) * block_size + fs_data->start_offset;
    read(fs_data->disk_number, offset_on_disk, block_size, outer_block);

    size_t outer_entries = block_size / sizeof(uint32_t);
    for (size_t i = 0; i < outer_entries; i++) {
        if ((*blocks_read) * block_size >= size_to_read) {
            break;
        }
        if (outer_block[i] == 0) {
            break;
        }

        // Each entry points to a single-indirect block
        read_single_indirect_blocks(fs_data, data, outer_block[i],
                                    size_to_read, block_size, blocks_read);
    }

    free(outer_block);
}

// ------------------------------------------------------------------------
// Helper to read from a triple-indirect block pointer (points to multiple
// double-indirect arrays).
// ------------------------------------------------------------------------
static void read_triple_indirect_blocks(
    filesystem_data* fs_data,
    void*            data,
    uint32_t         block_num,
    uint64_t         size_to_read,
    uint64_t         block_size,
    uint64_t*        blocks_read
) {
    if (block_num == 0) {
        return;
    }

    // The triple-indirect block is an array of block pointers,
    // each pointing to a double-indirect array
    uint32_t* triple_block = malloc(block_size);
    if (!triple_block) {
        printf("EXT2: Could not allocate memory for triple-indirect block.\n");
        return;
    }

    // Convert block_num to a byte offset
    uint64_t offset_on_disk = (uint64_t)(block_num - 1) * block_size + fs_data->start_offset;
    read(fs_data->disk_number, offset_on_disk, block_size, triple_block);

    size_t entries = block_size / sizeof(uint32_t);
    for (size_t i = 0; i < entries; i++) {
        if ((*blocks_read) * block_size >= size_to_read) {
            break;
        }
        if (triple_block[i] == 0) {
            break;
        }

        // Each entry points to a double-indirect block
        read_double_indirect_blocks(fs_data, data, triple_block[i],
                                    size_to_read, block_size, blocks_read);
    }

    free(triple_block);
}

// ------------------------------------------------------------------------
// The main function that reads an inode's data (direct + indirect blocks).
// Uses the above helpers for single/double/triple-indirect sections.
// ------------------------------------------------------------------------
void* ext2_read_inode(filesystem_data* fs_data, ext2_super_block* super_block, uint64_t inode_num, size_t* size_read){
    // 1) Read superblock to get block size
    uint64_t block_size = s_block_get_block_size(super_block);

    // 2) Read the inode metadata
    ext2_inode* inode_meta = ext2_read_inode_metadata(fs_data, super_block, inode_num);
    uint64_t size_to_read = ((uint64_t)inode_meta->i_dir_acl << 32) | inode_meta->i_size;

    // 3) Allocate a buffer for the entire file
    void* data = malloc(size_to_read);
    if (!data) {
        printf("EXT2: Couldn't allocate memory for inode data.\n");
        *size_read = 0;
        return NULL;
    }

    // We'll read blocks into 'data', tracking how many blocks have been read
    uint64_t blocks_read = 0;

    // --------------------------------------------------------------------
    // 1) Direct blocks (i_block[0..11])
    // --------------------------------------------------------------------
    for (int i = 0; i < 12; i++) {
        if (blocks_read * block_size >= size_to_read) {
            break;
        }
        if (inode_meta->i_block[i] == 0) {
            break;
        }

        uint64_t total_size_left = size_to_read - (blocks_read * block_size);
        size_t   to_read         = (total_size_left < block_size)
                                 ? (size_t)total_size_left
                                 : (size_t)block_size;
        size_t   dst             = (size_t)(blocks_read * block_size);

        // Convert this direct block number to a byte offset
        uint64_t offset_on_disk = ((uint64_t)inode_meta->i_block[i] - 1) * block_size
                                  + fs_data->start_offset;

        // Read from disk into our data buffer
        read(fs_data->disk_number, offset_on_disk, to_read, (uint8_t*)data + dst);

        blocks_read++;
    }

    // --------------------------------------------------------------------
    // 2) Single indirect block (i_block[12])
    // --------------------------------------------------------------------
    if (blocks_read * block_size < size_to_read && inode_meta->i_block[12] != 0) {
        read_single_indirect_blocks(fs_data, data, inode_meta->i_block[12],
                                    size_to_read, block_size, &blocks_read);
    }

    // --------------------------------------------------------------------
    // 3) Double indirect block (i_block[13])
    // --------------------------------------------------------------------
    if (blocks_read * block_size < size_to_read && inode_meta->i_block[13] != 0) {
        read_double_indirect_blocks(fs_data, data, inode_meta->i_block[13],
                                    size_to_read, block_size, &blocks_read);
    }

    // --------------------------------------------------------------------
    // 4) Triple indirect block (i_block[14])
    // --------------------------------------------------------------------
    if (blocks_read * block_size < size_to_read && inode_meta->i_block[14] != 0) {
        read_triple_indirect_blocks(fs_data, data, inode_meta->i_block[14],
                                    size_to_read, block_size, &blocks_read);
    }

    // Calculate how many bytes we actually read
    uint64_t actual_bytes_read = blocks_read * block_size;
    if (actual_bytes_read > size_to_read) {
        actual_bytes_read = size_to_read;  // safeguard
    }
    free(inode_meta);
    *size_read = (size_t)actual_bytes_read;
    return data;
}