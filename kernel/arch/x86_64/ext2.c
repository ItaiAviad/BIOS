#include <ext2.h>
#include <disk.h>
#include <math.h>
 

ext2_super_block ext2_read_super_block(filesystem_data* fs_data){
    ext2_super_block super_block;
    read_disk(fs_data->disk_number, fs_data->start_offset, sizeof(ext2_super_block), &super_block);
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
    read_disk(fs_data->disk_number, bgdt_offset + sizeof(ext2_block_group_descriptor) * group_num, sizeof(ext2_block_group_descriptor), &block_group_descriptor);
    return block_group_descriptor;
}

ext2_inode* ext2_read_inode_metadata(filesystem_data* fs_data, ext2_super_block* super_block, uint64_t inode_num){
    uint64_t num_block_groups = get_number_of_block_groups(super_block);
    uint64_t block_group_num = (inode_num - 1) / super_block->number_of_inodes_per_group;
    ext2_block_group_descriptor block_group_descriptor = ext2_read_block_group_descriptor(fs_data, super_block, block_group_num);
    uint64_t inode_table_index = (inode_num - 1) % super_block->number_of_inodes_per_group;
    uint64_t addr = ((block_group_descriptor.start_block_inode_table-1) * s_block_get_block_size(super_block)) + (inode_table_index * super_block->inode_size);
    ext2_inode* ret = malloc(super_block->inode_size);
    read_disk(fs_data->disk_number, fs_data->start_offset + addr, super_block->inode_size, ret);
    return ret;
}

ext2_dir_entry ext2_find_inode_in_dir_by_name(filesystem_data* fs_data, ext2_super_block* s_block, int dir_inode_num, char* inode_name, size_t name_size){

    size_t size = ext2_get_inode_size(fs_data, s_block, dir_inode_num);
    void* data = malloc(size);


    uint64_t size_read = ext2_read_inode(fs_data, s_block, dir_inode_num, 0, size, data);
    ext2_dir_entry* curr_entry = data;



    while((((void*)curr_entry) - data + 1) < size_read){

        if(strncmp(inode_name, curr_entry->name, MIN(curr_entry->name_length, name_size)) == 0){
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
        return NULL;
    }

    size_t size = ext2_get_inode_size(fs_data, &s_block, dir_inode_num);
    void* data = malloc(size);


    uint64_t size_read = ext2_read_inode(fs_data, &s_block, dir_inode_num, 0, size, data);
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

size_t ext2_get_inode_size(filesystem_data* fs_data, ext2_super_block* s_block, uint64_t inode_num){
    ext2_inode* inode_meta = ext2_read_inode_metadata(fs_data, s_block, inode_num);
    return ((uint64_t)inode_meta->i_dir_acl << 32) | inode_meta->i_size;
}

uint64_t ext2_get_inode_number_at_path(filesystem_data* fs_data, ext2_super_block* s_block, char* path){
    uint64_t ret = 0;
    char* path_preproccesed = preprocess_path(path);
    uint64_t current_inode = EXT2_ROOT_INODE;

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

    // Handle the edge case where the path is: "/"
    if (strcmp(path_preproccesed, "/") == 0) { 
        free(path_preproccesed);
        return EXT2_ROOT_INODE;
    }

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


size_t ext2_read_inode(filesystem_data* fs_data, ext2_super_block* s_block, uint64_t inode_num, size_t offset_bytes, size_t count_bytes, void* out_buffer){
    size_t max_read_size = ext2_get_inode_size(fs_data, s_block, inode_num);
    uint64_t block_size = s_block_get_block_size(s_block);
    uint64_t curr_read_offset = offset_bytes;
    uint64_t end_offset = MIN(max_read_size, offset_bytes+count_bytes) - 1;

    while (curr_read_offset <= end_offset)
    {
        uint64_t read_start = ext2_get_nth_block_offset_of_inode(fs_data, s_block, inode_num, curr_read_offset / block_size) + (curr_read_offset % block_size);
        uint64_t read_size = MIN(block_size, (end_offset - curr_read_offset) + 1);
        read_disk(fs_data->disk_number, read_start, read_size, out_buffer+(curr_read_offset - offset_bytes));
        curr_read_offset += read_size;
    }

    return (curr_read_offset - offset_bytes);

}


/**
 * Reads a full block from 'block_num' into 'buffer', based on block_size.
 * Returns 0 on success, non-zero on failure (you can adapt this logic).
 * You already have a read(...) function; we wrap it for clarity.
 */
static int read_full_block(filesystem_data* fs_data, uint32_t block_num, uint64_t block_size, void* buffer)
{
    if (block_num == 0) {
        return -1; // invalid block number
    }

    // Convert block_num to a byte offset on disk
    uint64_t offset_on_disk = ((uint64_t)block_num - 1) * block_size
                              + fs_data->start_offset;

    read_disk(fs_data->disk_number, offset_on_disk, block_size, buffer);

    return 0;
}

/**
 * A helper that returns the "index"-th 32-bit block pointer from the indirect
 * block "indirect_block_num". In EXT2, an indirect block is just an array
 * of 32-bit block numbers.
 *
 * If something fails or the index is out of range, returns 0.
 */
static uint32_t get_indirect_block_entry(filesystem_data* fs_data, uint32_t indirect_block_num, uint64_t block_size, uint32_t index){
    if (indirect_block_num == 0) {
        return 0;
    }

    uint32_t* block_table = malloc(block_size);
    if (!block_table) {
        printf("EXT2: Could not allocate memory for indirect block.\n");
        return 0;
    }

    if (read_full_block(fs_data, indirect_block_num, block_size, block_table) != 0) {
        free(block_table);
        return 0;
    }

    uint32_t entries_per_block = (uint32_t)(block_size / sizeof(uint32_t));
    uint32_t result = 0;
    if (index < entries_per_block) {
        result = block_table[index];
    }
    free(block_table);
    return result;
}

/**
 * ext2_get_nth_block_offset:
 *  Given:
 *    - fs_data:          Your filesystem_data structure (has disk_number, start_offset)
 *    - super_block:      The ext2 superblock (used to get block_size if needed)
 *    - inode:            The inode struct, which has i_block[15] pointers
 *    - block_index:      The logical block index in this file (0-based)
 *
 *  Returns:
 *    The on-disk byte offset where this block is located, or 0 if invalid/unallocated.
 */
uint64_t ext2_get_nth_block_offset_of_inode(filesystem_data* fs_data, ext2_super_block* super_block, uint64_t inode_num, uint64_t block_index){
    // 1) Get the actual block size
    uint64_t block_size = s_block_get_block_size(super_block);

    // 2) Each indirect block is an array of 32-bit pointers
    uint64_t addresses_per_block = block_size / sizeof(uint32_t);

    ext2_inode* inode = ext2_read_inode_metadata(fs_data, super_block, inode_num);

    // --------------------------------------------------------------------
    // Direct blocks: i_block[0..11]
    // --------------------------------------------------------------------
    if (block_index < 12) {
        uint32_t block_num = inode->i_block[block_index];
        if (block_num == 0) {
            return 0; // block not allocated
        }
        return ((uint64_t)block_num - 1) * block_size + fs_data->start_offset;
    }
    block_index -= 12;

    // --------------------------------------------------------------------
    // Single-indirect: i_block[12]
    // --------------------------------------------------------------------
    if (block_index < addresses_per_block) {
        uint32_t single_indirect_num = inode->i_block[12];
        if (single_indirect_num == 0) {
            return 0; // no single-indirect block allocated
        }
        uint32_t block_num = get_indirect_block_entry(fs_data, single_indirect_num,
                                                      block_size,
                                                      (uint32_t)block_index);
        if (block_num == 0) {
            return 0;
        }
        return ((uint64_t)block_num - 1) * block_size + fs_data->start_offset;
    }
    block_index -= addresses_per_block;

    // --------------------------------------------------------------------
    // Double-indirect: i_block[13]
    //    - The double-indirect block is an array of pointers to single-indirect blocks
    //    - Each single-indirect block is an array of data-block pointers
    // --------------------------------------------------------------------
    {
        uint64_t double_capacity = addresses_per_block * addresses_per_block;
        if (block_index < double_capacity) {
            uint32_t double_indirect_num = inode->i_block[13];
            if (double_indirect_num == 0) {
                return 0;
            }

            // First level index: which single-indirect block?
            uint32_t outer_index = (uint32_t)(block_index / addresses_per_block);
            // Second level index: which entry inside that single-indirect block?
            uint32_t inner_index = (uint32_t)(block_index % addresses_per_block);

            // Get the single-indirect block
            uint32_t single_block_num = get_indirect_block_entry(fs_data, 
                                                                 double_indirect_num,
                                                                 block_size, 
                                                                 outer_index);
            if (single_block_num == 0) {
                return 0;
            }

            // Get the final data block from that single-indirect block
            uint32_t block_num = get_indirect_block_entry(fs_data,
                                                          single_block_num,
                                                          block_size,
                                                          inner_index);
            if (block_num == 0) {
                return 0;
            }
            return ((uint64_t)block_num - 1) * block_size + fs_data->start_offset;
        }
        block_index -= double_capacity;
    }

    // --------------------------------------------------------------------
    // Triple-indirect: i_block[14]
    //    - The triple-indirect block is an array of pointers to double-indirect blocks
    //    - Each double-indirect block is an array of pointers to single blocks
    //    - Each single-indirect block is an array of pointers to data blocks
    // --------------------------------------------------------------------
    {
        uint64_t double_capacity = addresses_per_block * addresses_per_block;
        uint64_t triple_capacity = double_capacity * addresses_per_block;
        if (block_index < triple_capacity) {
            uint32_t triple_indirect_num = inode->i_block[14];
            if (triple_indirect_num == 0) {
                return 0;
            }

            // outer_index: which double-indirect block do we use?
            uint32_t outer_index = (uint32_t)(block_index / double_capacity);
            // remainder: which block inside that double-indirect block?
            uint32_t remainder   = (uint32_t)(block_index % double_capacity);

            // Read the triple-indirect block
            uint32_t double_block_num = get_indirect_block_entry(fs_data,
                                                                 triple_indirect_num,
                                                                 block_size,
                                                                 outer_index);
            if (double_block_num == 0) {
                return 0;
            }

            // Next, we have double-indirect logic again
            uint32_t outer_index2 = remainder / (uint32_t)addresses_per_block;
            uint32_t inner_index2 = remainder % (uint32_t)addresses_per_block;

            // Read the next level (single-indirect block)
            uint32_t single_block_num = get_indirect_block_entry(fs_data,
                                                                 double_block_num,
                                                                 block_size,
                                                                 outer_index2);
            if (single_block_num == 0) {
                return 0;
            }

            // Finally, get the data block pointer
            uint32_t block_num = get_indirect_block_entry(fs_data,
                                                          single_block_num,
                                                          block_size,
                                                          inner_index2);
            if (block_num == 0) {
                return 0;
            }
            return ((uint64_t)block_num - 1) * block_size + fs_data->start_offset;
        }
    }

    // If we get here, the requested block_index is beyond triple-indirect range
    return 0;
}
