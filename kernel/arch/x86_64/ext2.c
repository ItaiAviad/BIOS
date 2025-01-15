#include <ext2.h>
#include <disk.h>
 

ext2_super_block ext2_read_super_block(filesystem_data* fs_data){
    ext2_super_block super_block;
    read(fs_data->disk_number, fs_data->start_offset, sizeof(ext2_super_block), &super_block);
    if (super_block.magic != 0xEF53) {
        printf("Error: Invalid superblock magic number (0x%x)\n", super_block.magic);
        memset(&super_block, 0, sizeof(ext2_super_block)); // Return a zeroed-out superblock
    }
    return super_block;
}

ext2_block_group_descriptor ext2_read_block_group_descriptor(filesystem_data* fs_data, ext2_super_block s_block , uint64_t group_num){
    ext2_block_group_descriptor block_group_descriptor;
    uint64_t block_size = s_block_get_block_size(s_block);
    uint64_t bgdt_offset = fs_data->start_offset + (block_size > 1024 ? block_size : 1024);
    read(fs_data->disk_number, bgdt_offset + sizeof(ext2_block_group_descriptor) * group_num, sizeof(ext2_block_group_descriptor), &block_group_descriptor);
    return block_group_descriptor;
}

ext2_inode ext2_read_inode(filesystem_data* fs_data, uint64_t inode_num){
    ext2_super_block super_block = ext2_read_super_block(fs_data);
    uint64_t num_block_groups = get_number_of_block_groups(super_block);
    uint64_t block_group_num = (inode_num - 1) / super_block.number_of_inodes_per_group;
    ext2_block_group_descriptor block_group_descriptor = ext2_read_block_group_descriptor(fs_data, super_block, block_group_num);
    uint64_t inode_table_index = (inode_num - 1) % super_block.number_of_inodes_per_group;
    uint64_t addr = ((block_group_descriptor.start_block_inode_table-1) * s_block_get_block_size(super_block)) + (inode_table_index * super_block.inode_size);
    ext2_inode ret;
    printf("reading: 0x%x", fs_data->start_offset + addr);
    read(fs_data->disk_number, fs_data->start_offset + addr, super_block.inode_size, &ret);
    return ret;
}